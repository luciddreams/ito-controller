#pragma once
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
namespace telemetry {
struct Snapshot {
 uint32_t uptime=0, sequence=0, sampleAge=0;
 unsigned mode=0, installed=0, rpm=0;
 bool ready=false, calibrated=false, adcVerified=false, watchdog=false;
 bool ioOk=false, sensorsFresh=false, load=false, button=false, efuse=false;
 bool heater=false, tec=false, relay=false, monitorOnly=false;
 unsigned raw[2]={}, mv[2]={};
 float inputA=NAN, heaterA=NAN, busV=NAN, temperatures[5]={NAN,NAN,NAN,NAN,NAN};
 float airT[2]={NAN,NAN}, rh[2]={NAN,NAN}, dew[2]={NAN,NAN};
 bool airValid[2]={};
 uint32_t airAge[2]={};
 char fault[40]="NONE";
};
// JSON has no NaN/Infinity: missing readings are null, never zero.
inline const char* number(float value,char* out,size_t n){
 if(isfinite(value))snprintf(out,n,"%.3f",double(value));else snprintf(out,n,"null");
 return out;
}
inline bool encode(const Snapshot& s,char* out,size_t size){
 char v[14][32];float values[]={s.inputA,s.heaterA,s.busV,s.temperatures[0],s.temperatures[1],s.temperatures[2],s.temperatures[3],s.temperatures[4],s.airT[0],s.rh[0],s.dew[0],s.airT[1],s.rh[1],s.dew[1]};
 for(unsigned i=0;i<14;i++)number(values[i],v[i],sizeof(v[i]));
 // Fault labels originate only from fixed firmware literals.
 int n=snprintf(out,size,
 "{\"schema\":1,\"firmware\":\"L.2-S3\",\"uptime_ms\":%lu,\"sequence\":%lu,\"sample_age_ms\":%lu,\"mode\":%u,\"fault\":\"%s\",\"ready\":%s,\"installed\":%u,\"calibrated\":%s,\"adc_verified\":%s,\"watchdog\":%s,\"io_ok\":%s,\"sensors_fresh\":%s,\"load\":%s,\"button\":%s,\"efuse_ok\":%s,\"monitor_only\":%s,\"outputs\":[%s,%s,%s],\"raw\":[%u,%u],\"mv\":[%u,%u],\"input_a\":%s,\"heater_a\":%s,\"bus_v\":%s,\"rpm\":%u,\"temperatures\":[%s,%s,%s,%s,%s],\"ambient\":[{\"valid\":%s,\"age_ms\":%lu,\"temperature\":%s,\"humidity\":%s,\"dewpoint\":%s},{\"valid\":%s,\"age_ms\":%lu,\"temperature\":%s,\"humidity\":%s,\"dewpoint\":%s}]}",
 (unsigned long)s.uptime,(unsigned long)s.sequence,(unsigned long)s.sampleAge,s.mode,s.fault,s.ready?"true":"false",s.installed,
 s.calibrated?"true":"false",s.adcVerified?"true":"false",s.watchdog?"true":"false",s.ioOk?"true":"false",s.sensorsFresh?"true":"false",s.load?"true":"false",s.button?"true":"false",s.efuse?"true":"false",s.monitorOnly?"true":"false",
 s.heater?"true":"false",s.tec?"true":"false",s.relay?"true":"false",s.raw[0],s.raw[1],s.mv[0],s.mv[1],v[0],v[1],v[2],s.rpm,v[3],v[4],v[5],v[6],v[7],
 s.airValid[0]?"true":"false",(unsigned long)s.airAge[0],v[8],v[9],v[10],s.airValid[1]?"true":"false",(unsigned long)s.airAge[1],v[11],v[12],v[13]);
 return n>0&&size_t(n)<size;
}
}
