#include "../ito_prototype_esp32s3/telemetry.h"
#include <assert.h>
#include <iostream>
int main(){
 telemetry::Snapshot s;s.sequence=42;s.monitorOnly=true;
 char small[20];assert(!telemetry::encode(s,small,sizeof(small)));
 s.temperatures[0]=22.25f;s.inputA=INFINITY;
 char out[2048];assert(telemetry::encode(s,out,sizeof(out)));
 assert(strstr(out,"\"input_a\":null"));assert(!strstr(out,"nan"));assert(strstr(out,"22.250"));
 std::cout<<out<<std::endl;
}
