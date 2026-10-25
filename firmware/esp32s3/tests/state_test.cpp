#define ITO_HOST_TEST 1
#include "../ito_prototype_esp32s3/controller.h"
#include <assert.h>
void fixture(){
 clockMs=5000;lastSensorComplete=clockMs;watchdogOK=mockWatchdogOK=true;lastPoll=clockMs;bootTime=0;offTime=0;mode=safety::Mode::Idle;fault="NONE";initialized=ready=true;
 inputs=safety::Inputs{};inputs.sensorsFresh=inputs.fanOk=inputs.usb=inputs.button=true;inputs.busV=24;
 for(auto& t:inputs.temperature)t=25;
 for(auto& a:air){a.valid=true;a.stamp=clockMs;a.c=25;a.rh=50;a.dew=13.85;}airNext=100000;airPhase=0;
 Serial.connected=true;Serial.capacity=1024;Serial.input.clear();txHead=txTail=0;bus=Bus{};for(auto& p:gpio)p=0;
}
void tick(unsigned n){clockMs+=n;lastPoll=lastSensorComplete=clockMs;loop();}
void assertOff(){assert(gpio[D3]==0&&gpio[D6]==0&&gpio[D9]==0);}
int main(){
 fixture();ready=false;requestPulse(true,20,0,500);assert(mode==safety::Mode::Idle);assertOff();
 fixture();requestPulse(false,0,1,500);assertOff();assert(mode==safety::Mode::Idle);
 fixture();requestPulse(false,1,501,500);assertOff();
 fixture();requestPulse(true,20,0,2001);assertOff();
 fixture();inputs.button=false;requestPulse(true,20,0,500);assertOff();
 fixture();requestPulse(true,20,0,500);assert(mode==safety::Mode::RelayWait&&gpio[D9]);tick(201);assert(mode==safety::Mode::Fault);assertOff();
 fixture();requestPulse(true,20,0,500);inputs.loadPresent=true;tick(201);assert(mode==safety::Mode::Heater&&gpio[D3]);
 requestPulse(false,1,100,500);assert(mode==safety::Mode::Heater&&gpio[D6]==0);tick(500);assert(mode==safety::Mode::Idle);assertOff();
 requestPulse(true,20,0,500);assert(mode==safety::Mode::Idle); // settle interval
 fixture();requestPulse(false,1,100,1000);inputs.loadPresent=true;tick(201);assert(mode==safety::Mode::Tec&&gpio[D6]&&gpio[D3]==0&&bus.out==1);
 gpio[D10]=1;tick(1);assert(mode==safety::Mode::Fault);assertOff();
 fixture();requestPulse(true,20,0,500);Serial.connected=false;tick(1);assert(mode==safety::Mode::Fault);assertOff();
 fixture();bus.ok=false;requestPulse(true,20,0,500);assert(mode==safety::Mode::Fault);assertOff();
 fixture();requestPulse(true,200,0,500);inputs.loadPresent=true;tick(201);assert(mode==safety::Mode::Fault);assertOff();
 fixture();latch("FIRST");latch("SECOND");assert(!strcmp(fault,"FIRST"));assertOff();
 fixture();requestPulse(false,1,100,500);inputs.loadPresent=true;tick(201);bus.ok=false;stopOutputs();assertOff();assert(mode==safety::Mode::Fault);
 fixture();requestPulse(true,20,0,500);inputs.loadPresent=true;tick(201);char resetCmd[]="reset";command(resetCmd);assertOff();assert(mode==safety::Mode::Idle);
 fixture();air[1].valid=false;requestPulse(true,20,0,500);assertOff();assert(mode==safety::Mode::Idle);
 fixture();requestPulse(true,20,0,500);air[1].valid=false;tick(1);assertOff();assert(mode==safety::Mode::Fault);
 fixture();requestPulse(false,2,100,500);assert(mode==safety::Mode::Idle);assertOff();
 fixture();watchdogOK=false;requestPulse(false,1,100,500);assertOff();assert(mode==safety::Mode::Idle);
 fixture();lastSensorComplete=0;requestPulse(false,1,100,500);assertOff();assert(mode==safety::Mode::Idle);
 fixture();inputs.temperature[0]=NAN;requestPulse(false,1,100,500);assertOff();
 fixture();mockWatchdogOK=false;tick(1);assert(mode==safety::Mode::Fault);assertOff();
 fixture();requestPulse(false,1,100,500);inputs.loadPresent=true;tick(201);tick(500);assertOff();assert(mode==safety::Mode::Idle);
 fixture();unsigned a[3];assert(parseArgs("1 100 500",a,3));
 assert(!parseArgs("-1 100 500",a,3));assert(!parseArgs("1 4294967396 500",a,3));
 assert(!parseArgs("1 100 500 extra",a,3));assert(!parseArgs("1 100",a,3));
 fixture();requestPulse(false,1,100,500);inputs.loadPresent=true;tick(201);
 Serial.input=std::string(100,'x')+"\n";serialPoll();serialPoll();serialPoll();assertOff();
 fixture();Serial.capacity=0;for(int n=0;n<100;n++){status();serialFlush();}assertOff();
 fixture();mockMv[0]=3100;pollSensors(clockMs);assert(isnan(inputs.temperature[4]));mockMv[0]=1650;
 fixture();initDevices();bus.dacCodes[0]=1;pollSensors(clockMs);assert(mode==safety::Mode::Fault);assertOff();
 fixture();initDevices();bus.direction=0xff;pollSensors(clockMs);assert(mode==safety::Mode::Fault);assertOff();
 puts("PASS: real firmware state transitions, interlock, pulse timeout, calibration lock, deadman, USB loss, I2C failure, power limit");
}

