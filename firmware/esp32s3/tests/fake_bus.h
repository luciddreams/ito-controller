#pragma once
class Bus {
public:
 bool ok=true;uint8_t out=0,direction=0xf0;unsigned dacCodes[4]={};
 bool read(uint8_t addr,uint8_t* p,unsigned n){
  memset(p,0,n);
  if(addr==0x60 && n==24)for(unsigned k=0;k<4;k++){
   p[6*k]=0xc0|(k<<4);p[6*k+1]=0x80|(dacCodes[k]>>8);p[6*k+2]=dacCodes[k];
  }
  return ok;
 }
 void begin(){}
 bool write(uint8_t a,const uint8_t* p,unsigned n){
  if(ok&&a==0x60&&n==3)dacCodes[(p[0]>>1)&3]=((p[1]&15)<<8)|p[2];
  return ok;
 }
 bool reg8(uint8_t,uint8_t r,uint8_t v){if(ok&&r==1)out=v;if(ok&&r==3)direction=v;return ok;}
 bool reg16(uint8_t,uint8_t,uint16_t){return ok;}
 bool read8(uint8_t,uint8_t r,uint8_t& v){v=r==3?direction:r==1?out:0x10;return ok;}
 bool read16(uint8_t a,uint8_t r,uint16_t& v){
  if(a==0x40)v=r==0xfe?0x5449:r==0xff?0x2260:r==0?0x4007:r==2?19200:0;
  else v=0;
  return ok;
 }
};
