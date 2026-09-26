#pragma once
class Bus {
public:
 bool ok=true;uint8_t out=0;
 bool read(uint8_t,uint8_t*,unsigned){return ok;}
 void begin(){}
 bool write(uint8_t,const uint8_t*,unsigned){return ok;}
 bool reg8(uint8_t,uint8_t r,uint8_t v){if(r==1)out=v;return ok;}
 bool reg16(uint8_t,uint8_t,uint16_t){return ok;}
 bool read8(uint8_t,uint8_t r,uint8_t& v){v=r==3?0xf0:r==1?out:0x10;return ok;}
 bool read16(uint8_t a,uint8_t r,uint16_t& v){
  if(a==0x40)v=r==0xfe?0x5449:r==0xff?0x2260:r==0?0x4007:r==2?19200:0;
  else v=0;
  return ok;
 }
};
