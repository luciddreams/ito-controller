#include "../ito_prototype_esp32s3/safety.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
using namespace safety;
int main(){
 Inputs i;i.sensorsFresh=i.fanOk=i.usb=i.button=true;
 for(auto& t:i.temperature)t=25;
 auto checkNow=[&](){return check(i,true,3.8,1.0,3.4,40,55);};
 assert(!checkNow());
 i.ioOk=false;assert(!strcmp(checkNow(),"I2C"));i.ioOk=true;
 i.faultN=false;assert(!strcmp(checkNow(),"EFUSE"));i.faultN=true;
 i.inputV=1.0;assert(!strcmp(checkNow(),"INPUT_CURRENT"));i.inputV=0;
 i.inputA=3.8;assert(checkNow());i.inputA=0;
 i.heaterA=-3.5;assert(checkNow());i.heaterA=0;
 i.sensorsFresh=false;assert(checkNow());i.sensorsFresh=true;
 for(unsigned n=0;n<5;n++){
  i.temperature[n]=NAN;assert(checkNow());i.temperature[n]=(n==4?55:40);assert(checkNow());i.temperature[n]=25;
 }
 i.usb=false;assert(checkNow());i.usb=true;
 i.button=false;assert(checkNow());i.button=true;
 i.fanOk=false;assert(checkNow());i.fanOk=true;
 i.inputV=NAN;assert(checkNow());i.inputV=0;
 assert(fabsf(ntc(1.65,3.3,10000,3950)-25)<0.01);
 assert(isnan(ntc(0,3.3,10000,3950)));assert(isnan(ntc(3.3,3.3,10000,3950)));
 assert(maxDuty(24,7.2,10)==31);assert(maxDuty(NAN,7.2,10)==0);
 assert(maxDuty(30,7.2,10)==0);assert(maxDuty(24,0,10)==0);
 assert(elapsed(100,0xfffffff0u,100));assert(!elapsed(20,0xfffffff0u,100));
 unsigned codes[4]={0,0,0,0};assert(!configuration(false,3.6/4096,3.3,3.58,7.2,codes));
 assert(configuration(true,3.6/4096,3.3,3.58,7.2,codes));
 assert(!configuration(true,3.6/4096,3.3,3.58,0,codes));codes[0]=4096;
 assert(!configuration(true,3.6/4096,3.3,3.58,7.2,codes));
 puts("PASS: sensor/fault/current/temperature/deadman/fan/configuration/PWM/rollover cases");
}

