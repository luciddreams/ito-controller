#pragma once
#include <stdint.h>
#include <math.h>
namespace safety {
enum class Mode { Idle, RelayWait, Heater, Tec, Fault };
struct Inputs {
 bool ioOk=true, faultN=true, button=false, loadPresent=false;
 bool sensorsFresh=false, fanOk=false, usb=false;
 float inputV=0, inputA=0, heaterA=0, busV=0;
 float temperature[5]={};
};
inline float ntc(float v,float supply,float r25,float beta,float offset=0) {
 if(!isfinite(v)||!isfinite(supply)||!isfinite(r25)||!isfinite(beta)||
    supply<3.0f||supply>3.5f||v<0.02f*supply||v>0.98f*supply||r25<=0||beta<=0)return NAN;
 float r=10000.0f*v/(supply-v);
 return 1.0f/(1.0f/298.15f+logf(r/r25)/beta)-273.15f+offset;
}
inline bool configuration(bool verified,float adcSlope,float vdd,float ampSlope,float resistance,
                          const unsigned codes[4]) {
 if(!verified||!isfinite(adcSlope)||adcSlope<0.0007f||adcSlope>0.0011f||
    !isfinite(vdd)||vdd<3.0f||vdd>3.5f||!isfinite(ampSlope)||ampSlope<3||ampSlope>4.2f||
    !isfinite(resistance)||resistance<7.0f||resistance>100.0f)return false;
 for(int n=0;n<4;n++)if(codes[n]>4095)return false;
 return true;
}
inline const char* check(const Inputs& i, bool active,float inputTrip,float inputVTrip,
                         float heaterTrip,float glassTrip,float hotTrip) {
 if(!i.ioOk)return "I2C";
 if(!i.faultN)return "EFUSE";
 if(!i.sensorsFresh)return "STALE_SENSOR";
 if(!isfinite(i.inputV)||!isfinite(i.inputA)||!isfinite(i.heaterA)||!isfinite(i.busV))return "INVALID_MEASUREMENT";
 if(i.inputV< -0.02f||i.inputV>=inputVTrip||i.inputA>=inputTrip)return "INPUT_CURRENT";
 if(fabsf(i.heaterA)>=heaterTrip)return "HEATER_CURRENT";
 for(int n=0;n<5;n++){
  if(!isfinite(i.temperature[n])||i.temperature[n]<-10||i.temperature[n]>100)return "NTC";
  if(i.temperature[n]>=(n==4?hotTrip:glassTrip))return "TEMPERATURE";
 }
 if(active&&!i.usb)return "USB_LOST";
 if(active&&!i.button)return "BUTTON_RELEASED";
 if(active&&!i.fanOk)return "FAN_STOPPED";
 return nullptr;
}
inline unsigned maxDuty(float busV,float r,float maxW) {
 if(!isfinite(busV)||!isfinite(r)||busV<19||busV>28||r<7||r>100)return 0;
 float d=maxW*r/(busV*busV)*255;
 return (unsigned)(d<0?0:d>255?255:d); // floor, never round upward
}
inline bool elapsed(uint32_t now,uint32_t start,uint32_t duration) {return uint32_t(now-start)>=duration;}
}

