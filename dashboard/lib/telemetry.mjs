export function validate(data) {
  const fail = () => { throw new Error('Invalid device telemetry'); };
  if (!data || typeof data !== 'object' || data.schema !== 1) fail();
  const integer = (x, max = 4294967295) => Number.isInteger(x) && x >= 0 && x <= max;
  const reading = x => x === null || (typeof x === 'number' && Number.isFinite(x));
  if (typeof data.firmware !== 'string' || data.firmware.length > 40 || typeof data.fault !== 'string' || !/^[A-Z0-9_]{1,40}$/.test(data.fault)) fail();
  for (const k of ['uptime_ms', 'sequence', 'sample_age_ms', 'rpm']) if (!integer(data[k])) fail();
  if (!integer(data.mode, 4) || !integer(data.installed, 15)) fail();
  for (const k of ['ready','calibrated','adc_verified','watchdog','io_ok','sensors_fresh','load','button','efuse_ok','monitor_only']) if (typeof data[k] !== 'boolean') fail();
  for (const k of ['input_a','heater_a','bus_v']) if (!reading(data[k])) fail();
  if (!Array.isArray(data.temperatures) || data.temperatures.length !== 5 || !data.temperatures.every(reading)) fail();
  if (!Array.isArray(data.outputs) || data.outputs.length !== 3 || !data.outputs.every(x => typeof x === 'boolean')) fail();
  for (const k of ['raw','mv']) if (!Array.isArray(data[k]) || data[k].length !== 2 || !data[k].every(x => integer(x, 10000))) fail();
  if (!Array.isArray(data.ambient) || data.ambient.length !== 2) fail();
  for (const a of data.ambient) if (!a || typeof a.valid !== 'boolean' || !integer(a.age_ms) || ![a.temperature,a.humidity,a.dewpoint].every(reading)) fail();
  return data;
}
export function deviceURL(value) {
  const u = new URL(value);
  const octets = u.hostname.split('.').map(Number);
  const ip = octets.length === 4 && octets.every(x => Number.isInteger(x) && x >= 0 && x <= 255);
  const privateIP = ip && (octets[0] === 10 || (octets[0] === 192 && octets[1] === 168) || (octets[0] === 172 && octets[1] >= 16 && octets[1] <= 31) || octets[0] === 127);
  if (u.protocol !== 'http:' || !privateIP || u.username || u.password || u.search || u.hash || u.pathname !== '/') throw new Error('Use a private IPv4 HTTP origin for ITO_DEVICE_URL');
  return new URL('/api/status', u);
}
export function demo() {
  const t = Date.now()/1000;
  return {schema:1,firmware:'L.2-S3 DEMO',uptime_ms:Math.floor(t%86400*1000),sequence:Math.floor(t),sample_age_ms:4,mode:4,fault:'COMMISSIONING',ready:false,installed:1,calibrated:false,adc_verified:false,watchdog:true,io_ok:true,sensors_fresh:true,load:false,button:false,efuse_ok:true,monitor_only:true,outputs:[false,false,false],raw:[0,1900],mv:[0,1580],input_a:0.014,heater_a:0,bus_v:0,rpm:2400,temperatures:[22.3+Math.sin(t/12)*.2,22.1,22.6,22.4,24.2],ambient:[{valid:true,age_ms:240,temperature:22.5,humidity:58.2,dewpoint:13.9},{valid:true,age_ms:720,temperature:22.8,humidity:56.1,dewpoint:13.6}]};
}
