#include <Arduino.h>
#ifdef ITO_HOST_TEST
#include "test_config.h"
#include "fake_bus.h"
#else
#include <Adafruit_TinyUSB.h>
#include "config.h"
#include "bus.h"
#endif
#include "safety.h"
#include "ambient.h"
#include <stdio.h>
#include <string.h>
#if !defined(ARDUINO_Seeed_XIAO_nRF52840)
#error Select Seeeduino:nrf52:xiaonRF52840 (non-mbed core 1.1.13)
#endif
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
unsigned rawHot=0,rawInput=0;
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
void tachISR(){tachPulses++;}
void emit(const char* s){size_t n=strlen(s);if(Serial&&Serial.availableForWrite()>=(int)n)Serial.write((const uint8_t*)s,n);}
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
void watchdogStart(){NRF_WDT->CONFIG=1;NRF_WDT->CRV=8192;NRF_WDT->RREN=1;NRF_WDT->TASKS_START=1;}
void pollSensors(uint32_t now){
 inputs.ioOk=true;
 rawInput=analogRead(A1);rawHot=analogRead(A0);
 inputs.inputV=rawInput*cfg::ADC_VOLTS_PER_COUNT+cfg::ADC_OFFSET_V;
 inputs.inputA=inputs.inputV*cfg::INPUT_AMPS_PER_VOLT+cfg::INPUT_OFFSET_A;
 inputs.temperature[4]=safety::ntc(rawHot*cfg::ADC_VOLTS_PER_COUNT+cfg::ADC_OFFSET_V,cfg::VDD_V,cfg::NTC_R25[4],cfg::NTC_BETA[4],cfg::TEMP_OFFSET_C[4]);
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
  noInterrupts();uint32_t pulses=tachPulses;tachPulses=0;interrupts();
  rpm=(uint64_t(pulses)*60000)/(cfg::FAN_PULSES_PER_REV*uint32_t(now-lastTachWindow));lastTachWindow=now;
 }
 inputs.fanOk=rpm>=cfg::MIN_FAN_RPM;
}
void status(){char line[240];snprintf(line,sizeof(line),"mode=%u fault=%s ready=%u raw=%u,%u Iin=%.3f Vbus=%.2f Ih=%.3f rpm=%u T=%.1f,%.1f,%.1f,%.1f,%.1f\n",
 unsigned(mode),fault,ready,rawInput,rawHot,inputs.inputA,inputs.busV,inputs.heaterA,rpm,
 inputs.temperature[0],inputs.temperature[1],inputs.temperature[2],inputs.temperature[3],inputs.temperature[4]);emit(line);}
void requestPulse(bool heat,unsigned a,unsigned b,unsigned ms){
 uint32_t now=millis();
 if(cfg::REQUIRE_AMBIENT&&(!ambientOK(now)||uint32_t(now-air[0].stamp)>1000||uint32_t(now-air[1].stamp)>1000)){emit("REJECT: ambient missing/stale\n");return;}
 if(mode!=safety::Mode::Idle||!ready||!initialized||!safety::elapsed(now,offTime,cfg::OFF_SETTLE_MS)||
    !inputs.button||!inputs.fanOk||!inputs.usb||!inputs.sensorsFresh||inputs.loadPresent||ms<100||ms>cfg::MAX_PULSE_MS){emit("REJECT: state/calibration/button/fan/timing\n");return;}
 if(heat){if(a==0||a>255){emit("REJECT: duty\n");return;}duty=a;}
 else {if(a<1||a>4||b==0||b>cfg::TEC_MAX_CODE[a-1]||b>4095){emit("REJECT: channel/code\n");return;}channel=a-1;code=b;}
 // No direct transition between modes. Hardware outputs still off here.
 if(!bus.reg8(0x20,1,0)||!dac()){latch("I2C_ARM");return;}
 pendingHeat=heat;duration=ms;mode=safety::Mode::RelayWait;modeTime=now;digitalWrite(D9,HIGH);
}
void command(char* s){
 if(!strcmp(s,"status")){status();return;}
 if(!strcmp(s,"ambient")){ambientStatus();return;}
 if(!strcmp(s,"stop")){stopOutputs();emit("STOP\n");return;}
 if(!strcmp(s,"reset")){
  stopOutputs();if(!digitalRead(D10)){emit("REJECT: release button\n");return;}
  if(!initDevices()){latch("INIT");return;}initialized=true;
  const char* why=safety::check(inputs,false,cfg::INPUT_TRIP_A,cfg::INPUT_TRIP_V,cfg::HEATER_ON_TRIP_A,cfg::GLASS_TRIP_C,cfg::HOT_TRIP_C);
  if(why||inputs.loadPresent){emit("REJECT: fault still present\n");return;}
  mode=safety::Mode::Idle;fault="NONE";offTime=millis();emit("RESET_OK\n");return;
 }
 unsigned a,b,c;char extra;
 if(sscanf(s,"heat %u %u %c",&a,&b,&extra)==2&&strncmp(s,"heat ",5)==0){requestPulse(true,a,0,b);return;}
 if(sscanf(s,"tec %u %u %u %c",&a,&b,&c,&extra)==3&&strncmp(s,"tec ",4)==0){requestPulse(false,a,b,c);return;}
 emit("Commands: status | ambient | stop | reset | heat DUTY MS | tec CH CODE MS\n");
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
 digitalWrite(D3,LOW);pinMode(D3,OUTPUT);digitalWrite(D6,LOW);pinMode(D6,OUTPUT);digitalWrite(D9,LOW);pinMode(D9,OUTPUT);
 digitalWrite(D7,LOW);pinMode(D7,OUTPUT); // transistor off => fan PWM pulled high => full speed
 pinMode(D2,INPUT);pinMode(D10,INPUT);pinMode(D8,INPUT);
 attachInterrupt(digitalPinToInterrupt(D8),tachISR,FALLING);
 analogReference(AR_INTERNAL);analogReadResolution(12);
 Serial.begin(115200); // no wait for USB
 watchdogStart();bus.begin();bootTime=offTime=lastTachWindow=millis();
 initialized=initDevices();if(!initialized)latch("INIT");
 ready=safety::configuration(cfg::MEASUREMENTS_VERIFIED,cfg::ADC_VOLTS_PER_COUNT,cfg::VDD_V,cfg::INPUT_AMPS_PER_VOLT,cfg::HEATER_RESISTANCE_OHM,cfg::TEC_MAX_CODE);
 for(unsigned n=0;n<5;n++)inputs.temperature[n]=NAN;
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
 NRF_WDT->RR[0]=0x6e524635; // feed only after the complete loop, never in an ISR
 delay(1);
}
