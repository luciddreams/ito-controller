#define ITO_HOST_TEST 1
#include "../ito_prototype/ito_prototype.ino"
#include <assert.h>
void fixture(){
 clockMs=5000;lastPoll=clockMs;bootTime=0;offTime=0;mode=safety::Mode::Idle;fault="NONE";initialized=ready=true;
 inputs=safety::Inputs{};inputs.sensorsFresh=inputs.fanOk=inputs.usb=inputs.button=true;inputs.busV=24;
 for(auto& t:inputs.temperature)t=25;
 for(auto& a:air){a.valid=true;a.stamp=clockMs;a.c=25;a.rh=50;a.dew=13.85;}airNext=100000;airPhase=0;
 Serial.connected=true;bus.ok=true;bus.out=0;for(auto& p:gpio)p=0;
}
void tick(unsigned n){clockMs+=n;lastPoll=clockMs;loop();}
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
 fixture();requestPulse(false,2,100,1000);inputs.loadPresent=true;tick(201);assert(mode==safety::Mode::Tec&&gpio[D6]&&gpio[D3]==0&&bus.out==2);
 gpio[D10]=1;tick(1);assert(mode==safety::Mode::Fault);assertOff();
 fixture();requestPulse(true,20,0,500);Serial.connected=false;tick(1);assert(mode==safety::Mode::Fault);assertOff();
 fixture();bus.ok=false;requestPulse(true,20,0,500);assert(mode==safety::Mode::Fault);assertOff();
 fixture();requestPulse(true,200,0,500);inputs.loadPresent=true;tick(201);assert(mode==safety::Mode::Fault);assertOff();
 fixture();latch("FIRST");latch("SECOND");assert(!strcmp(fault,"FIRST"));assertOff();
 fixture();requestPulse(false,1,100,500);inputs.loadPresent=true;tick(201);bus.ok=false;stopOutputs();assertOff();assert(mode==safety::Mode::Fault);
 fixture();requestPulse(true,20,0,500);inputs.loadPresent=true;tick(201);char resetCmd[]="reset";command(resetCmd);assertOff();assert(mode==safety::Mode::Idle);
 fixture();air[1].valid=false;requestPulse(true,20,0,500);assertOff();assert(mode==safety::Mode::Idle);
 fixture();requestPulse(true,20,0,500);air[1].valid=false;tick(1);assertOff();assert(mode==safety::Mode::Fault);
 puts("PASS: real firmware state transitions, interlock, pulse timeout, calibration lock, deadman, USB loss, I2C failure, power limit");
}
