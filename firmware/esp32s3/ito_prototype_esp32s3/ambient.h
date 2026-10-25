#pragma once
#include <stdint.h>
#include <math.h>
namespace ambient {
inline uint8_t crc(const uint8_t* p,unsigned n){uint8_t c=0xff;for(unsigned i=0;i<n;i++){c^=p[i];for(unsigned b=0;b<8;b++)c=(c&0x80)?uint8_t((c<<1)^0x31):uint8_t(c<<1);}return c;}
struct Reading {float c=NAN,rh=NAN,dew=NAN;uint32_t stamp=0;bool valid=false;};
// Magnus dew point over liquid water, for ambient -20..60 C. Not frost point.
inline float dewpoint(float c,float rh){
 if(!isfinite(c)||!isfinite(rh)||c< -20||c>60||rh<=0||rh>100)return NAN;
 float g=logf(rh/100.0f)+17.62f*c/(243.12f+c);
 return 243.12f*g/(17.62f-g);
}
inline bool decode(const uint8_t p[6],uint32_t now,Reading& r){
 r.valid=false;
 if(crc(p,2)!=p[2]||crc(p+3,2)!=p[5])return false;
 float c=-45.0f+175.0f*((unsigned(p[0])<<8)|p[1])/65535.0f;
 float rh=100.0f*((unsigned(p[3])<<8)|p[4])/65535.0f;
 float dew=dewpoint(c,rh);
 if(!isfinite(dew))return false;
 r.c=c;r.rh=rh;r.dew=dew;r.stamp=now;r.valid=true;return true;
}
inline bool fresh(const Reading& r,uint32_t now,uint32_t age=3500){return r.valid&&uint32_t(now-r.stamp)<=age;}
}

