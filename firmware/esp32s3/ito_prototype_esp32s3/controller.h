#include <Arduino.h>
#ifdef ITO_HOST_TEST
#ifdef ITO_TEST_DEFAULT_CONFIG
#include "config.h"
#else
#include "test_config.h"
#endif
#include "fake_bus.h"
#else
#include "config.h"
#include "bus.h"
#endif
#include "safety.h"
#include "ambient.h"
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "network.h"
#include <stdlib.h>
#include <errno.h>
Bus bus;
safety::Inputs inputs;
safety::Mode mode=safety::Mode::Idle;
const char* fault="NONE";
bool initialized=false, ready=false, pendingHeat=false;
uint32_t lastPoll=0, bootTime=0, offTime=0, modeTime=0, pulseTime=0, duration=0;
uint32_t sensorTime[4]={}, adsStart=0, lastTachWindow=0;
unsigned adsChannel=0, duty=0, channel=0, code=0, rpm=0;
bool adsPending=false,heaterHigh=false;
uint32_t heaterEdge=0;
volatile uint32_t tachPulses=0;
unsigned rawHot=0,rawInput=0,mvHot=0,mvInput=0;
float hotVoltage=NAN;
bool watchdogOK=false;
uint32_t lastSensorComplete=0;
void emit(const char* s);
ambient::Reading air[2];
unsigned airChannel=0,airPhase=0;
uint32_t airStarted=0,airNext=0;
bool airInitialized[2]={false,false};
bool ambientOK(uint32_t now){return ambient::fresh(air[0],now)&&ambient::fresh(air[1],now);}
void pollAmbient(uint32_t now){
 // Avoid adding bus traffic to software heater PWM. Pulses max2s; age gate below.
 if(mode==safety::Mode::Heater||mode==safety::Mode::RelayWait)return;
 if(airPhase==0&&int32_t(now-airNext)<0)return;
 uint8_t addr=0x44+airChannel;
 bool ok=true;
 if(airPhase==0){
  if(!airInitialized[airChannel]){
   const uint8_t heaterOff[]={0x30,0x66};
   ok=bus.write(addr,heaterOff,2);airInitialized[airChannel]=ok;
  }
  if(ok){const uint8_t measure[]={0x24,0x00};ok=bus.write(addr,measure,2);}
  if(ok){airStarted=now;airPhase=1;return;}
 }else{
  if(uint32_t(now-airStarted)<20)return; // no-clock-stretch high repeatability
  uint8_t bytes[6];ok=bus.read(addr,bytes,6)&&ambient::decode(bytes,now,air[airChannel]);
 }
 if(!ok){air[airChannel].valid=false;airInitialized[airChannel]=false;}
 airChannel^=1;airPhase=0;airNext=now+480; // approximately one sample/second/sensor
}
void ambientStatus(){
 for(unsigned n=0;n<2;n++){char s[150];snprintf(s,sizeof(s),"air%u addr=0x%02X valid=%u age_ms=%lu T=%.2f RH=%.2f dew=%.2f\n",n+1,0x44+n,ambient::fresh(air[n],millis()),(unsigned long)(millis()-air[n].stamp),air[n].c,air[n].rh,air[n].dew);emit(s);}
}
void IRAM_ATTR tachISR(){platform::tachIncrement(tachPulses);}
// Bounded queue: diagnostics must never block the control loop when USB stalls.
char txQueue[1024];unsigned txHead=0,txTail=0;
void emit(const char* s){
 if(!Serial)return;
 size_t n=strlen(s);unsigned free=(txTail+sizeof(txQueue)-txHead-1)%sizeof(txQueue);
 if(n>free)return;
 for(size_t i=0;i<n;i++){txQueue[txHead]=s[i];txHead=(txHead+1)%sizeof(txQueue);}
}
void serialFlush(){
 if(!Serial){txHead=txTail=0;return;}
 int capacity=Serial.availableForWrite();
 unsigned n=0;uint8_t bytes[64];
 while(txTail!=txHead&&n<sizeof(bytes)&&int(n)<capacity){bytes[n++]=txQueue[txTail];txTail=(txTail+1)%sizeof(txQueue);}
 if(n)Serial.write(bytes,n); // zero TX timeout set in setup; may drop diagnostics
}
void directOff(){digitalWrite(D3,LOW);digitalWrite(D6,LOW);digitalWrite(D9,LOW);heaterHigh=false;}
void latch(const char* why){directOff();if(mode!=safety::Mode::Fault)fault=why;mode=safety::Mode::Fault;offTime=millis();}
bool dac(unsigned selected=4,unsigned value=0){
 // MCP4728 multi-write: channel, VREF=internal 2.048 V, PD=normal, gain=1.
 for(unsigned n=0;n<4;n++){unsigned v=n==selected?value:0;uint8_t p[]={uint8_t(0x40|(n<<1)),uint8_t(0x80|(v>>8)),uint8_t(v)};
  if(!bus.write(0x60,p,3))return false;
 }return true;
}
bool initDevices(){
 // Output latch zero BEFORE changing TCA9534 direction.
 if(!bus.reg8(0x20,1,0)||!bus.reg8(0x20,2,0)||!bus.reg8(0x20,3,0xf0)||!dac())return false;
 uint16_t id=0;
 if(!bus.read16(0x40,0xfe,id)||id!=0x5449)return false;
 if(!bus.read16(0x40,0xff,id)||(id&0xfff0)!=0x2260)return false;
 // INA226 one average, 140us bus/shunt, continuous both. Shunt read directly.
 return bus.reg16(0x40,0,0x4007)&&bus.reg16(0x40,5,0x0800);
}
void stopOutputs(){directOff();offTime=millis();if(mode!=safety::Mode::Fault)mode=safety::Mode::Idle;
 if(!bus.reg8(0x20,1,0)||!dac())latch("I2C_STOP");
}
bool configurationOK(){
 return !monitor::enabled&&watchdogOK&&cfg::MEASUREMENTS_VERIFIED&&cfg::ADC_VERIFIED&&
  platform::calibrationValid(cfg::INPUT_ADC_GAIN,cfg::INPUT_ADC_OFFSET_V)&&
  platform::calibrationValid(cfg::HOT_ADC_GAIN,cfg::HOT_ADC_OFFSET_V)&&
  safety::configuration(true,0.001f,cfg::VDD_V,cfg::INPUT_AMPS_PER_VOLT,cfg::HEATER_RESISTANCE_OHM,cfg::TEC_MAX_CODE);
}
void pollSensors(uint32_t now){
 inputs.ioOk=true;
 inputs.inputV=platform::adcVoltage(A1,rawInput,mvInput,cfg::INPUT_ADC_GAIN,cfg::INPUT_ADC_OFFSET_V);
 hotVoltage=platform::adcVoltage(A0,rawHot,mvHot,cfg::HOT_ADC_GAIN,cfg::HOT_ADC_OFFSET_V);
 inputs.inputA=inputs.inputV*cfg::INPUT_AMPS_PER_VOLT+cfg::INPUT_OFFSET_A;
 inputs.temperature[4]=safety::ntc(hotVoltage,cfg::VDD_V,cfg::NTC_R25[4],cfg::NTC_BETA[4],cfg::TEMP_OFFSET_C[4]);
 uint8_t port=0,dir=0,out=0;uint16_t shunt=0,voltage=0,inaConfig=0;
 if(!bus.read8(0x20,0,port)||!bus.read8(0x20,3,dir)||!bus.read8(0x20,1,out)||
    !bus.read16(0x40,1,shunt)||!bus.read16(0x40,2,voltage)||!bus.read16(0x40,0,inaConfig)){
  inputs.ioOk=false;latch("I2C_READ");return;
 }
 unsigned expected=(mode==safety::Mode::Tec)?(1u<<channel):0;
 if(dir!=0xf0||(mode!=safety::Mode::Fault&&(out&15)!=expected)||inaConfig!=0x4007){inputs.ioOk=false;latch("REGISTER_RESET");return;}
 if(mode!=safety::Mode::Fault){
  uint8_t d[24];if(!bus.read(0x60,d,24)){inputs.ioOk=false;latch("DAC_READ");return;}
  for(unsigned n=0;n<4;n++){
   unsigned want=(mode==safety::Mode::Tec&&n==channel)?code:0;
   if((d[n*6]&0xf0)!=(0xc0|(n<<4))||d[n*6+1]!=(0x80|(want>>8))||d[n*6+2]!=(want&255)){
    latch("DAC_REGISTER");return;
   }
  }
 }
 inputs.faultN=port&16;
 float measuredA=int16_t(shunt)*0.00025f; // 2.5uV / 10mOhm
 inputs.heaterA=measuredA;inputs.busV=voltage*0.00125f;
 // At 10Hz, sample high and low windows; continuous conversion delay is <1ms.
 // Instantaneous samples complement hardware input limit; scope must verify peaks.
 if(mode==safety::Mode::Heater&&heaterHigh&&uint32_t(now-heaterEdge)>3&&fabsf(measuredA)>=cfg::HEATER_ON_TRIP_A){latch("HEATER_PEAK");return;}
 if(!adsPending){
  uint16_t conf=0x8000|((4+adsChannel)<<12)|0x0200|0x0100|0x00e0|3;
  if(!bus.reg16(0x48,1,conf)){inputs.ioOk=false;latch("ADS_START");return;}
  adsStart=now;adsPending=true;
 }else{
  uint16_t conf=0,value=0;
  if(!bus.read16(0x48,1,conf)){inputs.ioOk=false;latch("ADS_CONFIG");return;}
  if((conf&0x7fff)!=(uint16_t)(((4+adsChannel)<<12)|0x0200|0x0100|0x00e0|3)){latch("ADS_REGISTER");return;}
  if(conf&0x8000){
   if(!bus.read16(0x48,0,value)){inputs.ioOk=false;latch("ADS_READ");return;}
   inputs.temperature[adsChannel]=safety::ntc(int16_t(value)*0.000125f,cfg::VDD_V,cfg::NTC_R25[adsChannel],cfg::NTC_BETA[adsChannel],cfg::TEMP_OFFSET_C[adsChannel]);
   sensorTime[adsChannel]=now;adsChannel=(adsChannel+1)%4;adsPending=false;
  }else if(safety::elapsed(now,adsStart,30)){latch("ADS_TIMEOUT");return;}
 }
 inputs.sensorsFresh=true;for(unsigned n=0;n<4;n++)if(sensorTime[n]==0||uint32_t(now-sensorTime[n])>150)inputs.sensorsFresh=false;
 inputs.loadPresent=!digitalRead(D2);inputs.button=!digitalRead(D10);inputs.usb=bool(Serial);
 if(safety::elapsed(now,lastTachWindow,500)){
  uint32_t pulses=platform::tachTake(tachPulses);
  rpm=(uint64_t(pulses)*60000)/(cfg::FAN_PULSES_PER_REV*uint32_t(now-lastTachWindow));lastTachWindow=now;
 }
 inputs.fanOk=rpm>=cfg::MIN_FAN_RPM;
 lastSensorComplete=millis();
}
void status(){char line[320];snprintf(line,sizeof(line),"mode=%u fault=%s ready=%u raw=%u,%u mV=%u,%u Iin=%.3f Vbus=%.2f Ih=%.3f rpm=%u T=%.1f,%.1f,%.1f,%.1f,%.1f\n",
 unsigned(mode),fault,ready,rawInput,rawHot,mvInput,mvHot,inputs.inputA,inputs.busV,inputs.heaterA,rpm,
 inputs.temperature[0],inputs.temperature[1],inputs.temperature[2],inputs.temperature[3],inputs.temperature[4]);emit(line);
 snprintf(line,sizeof(line),"FW=L.2-S3 target=XIAO_ESP32S3 installed=0x%X calibrated=%u adc_verified=%u wdt=%u load=%u button=%u efuse_ok=%u\n",cfg::INSTALLED_TEC_MASK,cfg::MEASUREMENTS_VERIFIED,cfg::ADC_VERIFIED,watchdogOK,inputs.loadPresent,inputs.button,inputs.faultN);emit(line);
}
void requestPulse(bool heat,unsigned a,unsigned b,unsigned ms){
 if(monitor::enabled){emit("REJECT: Wi-Fi build is monitoring only; power pulses disabled\n");return;}
 uint32_t now=millis();
 if(!configurationOK()||uint32_t(now-lastSensorComplete)>50||
    safety::check(inputs,false,cfg::INPUT_TRIP_A,cfg::INPUT_TRIP_V,cfg::HEATER_ON_TRIP_A,cfg::GLASS_TRIP_C,cfg::HOT_TRIP_C)){
  emit("REJECT: configuration or fresh healthy measurements required\n");return;
 }
 if(heat&&!cfg::ALLOW_HEATER){emit("REJECT: heater disabled in config\n");return;}
 if(!heat&&(a<1||a>4||!(cfg::INSTALLED_TEC_MASK&(1u<<(a-1))))){emit("REJECT: channel not installed\n");return;}
 if(cfg::REQUIRE_AMBIENT&&(!ambientOK(now)||uint32_t(now-air[0].stamp)>1000||uint32_t(now-air[1].stamp)>1000)){emit("REJECT: ambient missing/stale\n");return;}
 if(mode!=safety::Mode::Idle||!ready||!initialized||!safety::elapsed(now,offTime,cfg::OFF_SETTLE_MS)||
    !inputs.button||!inputs.fanOk||!inputs.usb||!inputs.sensorsFresh||inputs.loadPresent||ms<100||ms>cfg::MAX_PULSE_MS){emit("REJECT: state/calibration/button/fan/timing\n");return;}
 if(heat){if(a==0||a>255){emit("REJECT: duty\n");return;}duty=a;}
 else {if(a<1||a>4||b==0||b>cfg::TEC_MAX_CODE[a-1]||b>4095){emit("REJECT: channel/code\n");return;}channel=a-1;code=b;}
 // No direct transition between modes. Hardware outputs still off here.
 if(!bus.reg8(0x20,1,0)||!dac()){latch("I2C_ARM");return;}
 pendingHeat=heat;duration=ms;mode=safety::Mode::RelayWait;modeTime=now;digitalWrite(D9,HIGH);
}
bool parseArgs(const char* s,unsigned* values,unsigned count){
 for(unsigned i=0;i<count;i++){
  while(*s==' ')s++;
  if(*s<'0'||*s>'9')return false;
  unsigned value=0;
  while(*s>='0'&&*s<='9'){
   if(value>6553||(value==6553&&*s>'5'))return false;
   value=value*10+(*s++-'0');
  }
  values[i]=value;
  if(i+1<count&&*s!=' ')return false;
 }
 while(*s==' ')s++;
 return *s==0;
}
void command(char* s){
 if(!strcmp(s,"wifi")){char text[120];monitor::status(text,sizeof(text));emit(text);return;}
 if(!strcmp(s,"status")){status();return;}
 if(!strcmp(s,"pins")){
  char p[180];snprintf(p,sizeof(p),"D2/load=%d D10/button=%d D3/heater=%d D6/TEC_ARM=%d D9/POWER_ARM=%d SDA=%d SCL=%d\n",digitalRead(D2),digitalRead(D10),digitalRead(D3),digitalRead(D6),digitalRead(D9),digitalRead(D4),digitalRead(D5));emit(p);return;
 }
 if(!strcmp(s,"ambient")){ambientStatus();return;}
 if(!strcmp(s,"stop")){stopOutputs();emit("STOP\n");return;}
 if(!strcmp(s,"reset")){
  stopOutputs();if(!watchdogOK){emit("REJECT: watchdog unavailable\n");return;}if(!digitalRead(D10)){emit("REJECT: release button\n");return;}
  if(!initDevices()){latch("INIT");return;}initialized=true;
  const char* why=safety::check(inputs,false,cfg::INPUT_TRIP_A,cfg::INPUT_TRIP_V,cfg::HEATER_ON_TRIP_A,cfg::GLASS_TRIP_C,cfg::HOT_TRIP_C);
  if(why||inputs.loadPresent||uint32_t(millis()-lastSensorComplete)>50){emit("REJECT: fault still present\n");return;}
  mode=safety::Mode::Idle;fault="NONE";offTime=millis();emit("RESET_OK\n");return;
 }
 unsigned args[3];
 if(strncmp(s,"heat ",5)==0&&parseArgs(s+5,args,2)){requestPulse(true,args[0],0,args[1]);return;}
 if(strncmp(s,"tec ",4)==0&&parseArgs(s+4,args,3)){requestPulse(false,args[0],args[1],args[2]);return;}
 emit("Commands: wifi | status | pins | ambient | stop | reset | heat DUTY MS | tec CH CODE MS\n");
}
void serialPoll(){static char line[80];static unsigned used=0;static bool discard=false;
 for(unsigned k=0;k<32&&Serial.available();k++){
  char c=Serial.read();if(c=='\r')continue;
  if(c=='\n'){if(!discard){line[used]=0;command(line);}used=0;discard=false;}
  else if(used<sizeof(line)-1&&!discard)line[used++]=c;
  else {discard=true;used=0;stopOutputs();}
 }
}
void setup(){
 platform::outputLow(D3);platform::outputLow(D6);platform::outputLow(D9);
 platform::outputLow(D7); // transistor off => fan PWM pulled high => full speed
 pinMode(D2,INPUT);pinMode(D10,INPUT);pinMode(D8,INPUT);
 attachInterrupt(digitalPinToInterrupt(D8),tachISR,FALLING);
 platform::adcBegin();
 Serial.begin(115200); // no wait for USB
 Serial.setTxTimeoutMs(0);
 watchdogOK=platform::watchdogStart();
 if(!watchdogOK){latch("WATCHDOG_INIT");return;}
 bus.begin();bootTime=offTime=lastTachWindow=millis();
 initialized=initDevices();if(!initialized)latch("INIT");
 ready=configurationOK();
 for(unsigned n=0;n<5;n++)inputs.temperature[n]=NAN;
 monitor::begin();
}
void publishTelemetry(){
 static uint32_t previous=0,sequence=0;
 uint32_t now=millis();if(!monitor::enabled||uint32_t(now-previous)<500)return;previous=now;
 telemetry::Snapshot s;s.uptime=now;s.sequence=++sequence;s.sampleAge=now-lastSensorComplete;
 s.mode=unsigned(mode);snprintf(s.fault,sizeof(s.fault),"%s",fault);
 s.ready=ready;s.installed=cfg::INSTALLED_TEC_MASK;s.calibrated=cfg::MEASUREMENTS_VERIFIED;
 s.adcVerified=cfg::ADC_VERIFIED;s.watchdog=watchdogOK;s.ioOk=inputs.ioOk;s.sensorsFresh=inputs.sensorsFresh;
 // Read present pin levels, rather than cached input flags left by a failed I2C poll.
 s.load=!digitalRead(D2);s.button=!digitalRead(D10);s.efuse=inputs.faultN;
 s.heater=digitalRead(D3);s.tec=digitalRead(D6);s.relay=digitalRead(D9);s.monitorOnly=monitor::enabled;
 s.raw[0]=rawInput;s.raw[1]=rawHot;s.mv[0]=mvInput;s.mv[1]=mvHot;
 s.inputA=inputs.inputA;s.heaterA=inputs.heaterA;s.busV=inputs.busV;s.rpm=rpm;
 for(unsigned i=0;i<5;i++)s.temperatures[i]=inputs.temperature[i];
 for(unsigned i=0;i<2;i++){s.airValid[i]=ambient::fresh(air[i],now);s.airAge[i]=now-air[i].stamp;s.airT[i]=air[i].c;s.rh[i]=air[i].rh;s.dew[i]=air[i].dew;}
 monitor::publish(s);
}
void loop(){
 uint32_t now=millis();
 // Direct checks before I2C, including immediate dead-man release.
 bool active=mode==safety::Mode::RelayWait||mode==safety::Mode::Heater||mode==safety::Mode::Tec;
 if(active&&digitalRead(D10))latch("BUTTON_RELEASED");
 if(active&&!Serial)latch("USB_LOST");
 if(active&&cfg::REQUIRE_AMBIENT&&!ambientOK(now))latch("AMBIENT_LOST");
 if((mode==safety::Mode::Heater||mode==safety::Mode::Tec)&&safety::elapsed(now,pulseTime,duration))stopOutputs();
 if(safety::elapsed(now,lastPoll,5)){
  lastPoll=now;pollSensors(now);
  if(safety::elapsed(now,bootTime,250)){
   active=mode==safety::Mode::RelayWait||mode==safety::Mode::Heater||mode==safety::Mode::Tec;
   const char* why=safety::check(inputs,active,cfg::INPUT_TRIP_A,cfg::INPUT_TRIP_V,cfg::HEATER_ON_TRIP_A,cfg::GLASS_TRIP_C,cfg::HOT_TRIP_C);
   if(why&&mode!=safety::Mode::Fault)latch(why);
   if(mode==safety::Mode::Idle&&safety::elapsed(now,offTime,cfg::OFF_SETTLE_MS)&&inputs.loadPresent)latch("RELAY_STUCK");
  }
 }
 now=millis(); // bus work may have taken time; never use stale time to enable outputs
 if(active&&(digitalRead(D10)||!Serial))latch("INTERLOCK_CHANGED");
 if((mode==safety::Mode::Heater||mode==safety::Mode::Tec)&&safety::elapsed(now,pulseTime,duration))stopOutputs();
 if(mode==safety::Mode::RelayWait&&safety::elapsed(now,modeTime,200)){
  if(!inputs.loadPresent||inputs.busV<19||inputs.busV>28){latch("RELAY_OR_VOLTAGE");}
  else if(pendingHeat){
   if(duty>safety::maxDuty(inputs.busV,cfg::HEATER_RESISTANCE_OHM,cfg::MAX_HEATER_W)||inputs.busV/cfg::HEATER_RESISTANCE_OHM>=cfg::HEATER_ON_TRIP_A)latch("HEATER_LIMIT");
   else {mode=safety::Mode::Heater;pulseTime=now;}
  }else{
   if(!dac(channel,code)||!bus.reg8(0x20,1,1u<<channel))latch("TEC_ENABLE");
   else {digitalWrite(D6,HIGH);mode=safety::Mode::Tec;pulseTime=now;}
  }
 }
 if(mode==safety::Mode::Heater){
  if(!inputs.loadPresent||inputs.busV<19||inputs.busV>28||duty>safety::maxDuty(inputs.busV,cfg::HEATER_RESISTANCE_OHM,cfg::MAX_HEATER_W))latch("HEATER_SUPPLY");
  else {unsigned period=1000/cfg::HEATER_PWM_HZ;bool high=(uint32_t(now-pulseTime)%period)<(period*duty/255);
   if(high!=heaterHigh){heaterEdge=now;heaterHigh=high;digitalWrite(D3,high?HIGH:LOW);}
  }
 }
 if(mode==safety::Mode::Tec&&!inputs.loadPresent)latch("RELAY_LOST");
 pollAmbient(millis());
 serialPoll();
 serialFlush();
 publishTelemetry();
 if(watchdogOK&&!platform::watchdogFeed()){watchdogOK=false;latch("WATCHDOG_FEED");}
 delay(1);
}

