#include "../ito_prototype/ambient.h"
#include <assert.h>
#include <stdio.h>
int main(){
 using namespace ambient;
 assert(fabsf(dewpoint(25,50)-13.8516)<.01);
 assert(fabsf(dewpoint(20,100)-20)<.001);
 assert(isnan(dewpoint(20,0))&&isnan(dewpoint(NAN,50))&&isnan(dewpoint(25,101)));
 uint8_t p[]={0x66,0x66,0x93,0x80,0x00,0xa2};Reading r;
 assert(crc(p,2)==0x93);p[5]=crc(p+3,2);
 assert(decode(p,100,r));assert(fabsf(r.c-25)<.01&&fabsf(r.rh-50)<.01);
 assert(fresh(r,3600)&&!fresh(r,3601));p[2]^=1;assert(!decode(p,200,r)&&!r.valid);
 p[2]^=1;p[5]^=1;assert(!decode(p,200,r)&&!r.valid);
 r.valid=true;r.stamp=0xfffffff0;assert(fresh(r,50));
 puts("PASS: SHT31 CRC, corrupted bytes, conversions, dew point, invalid RH, stale data and rollover");
}
