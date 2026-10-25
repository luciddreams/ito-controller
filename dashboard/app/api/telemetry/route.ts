import { validate, deviceURL, demo } from '../../../lib/telemetry.mjs';
import { mqttTelemetry } from '../../../lib/mqtt.mjs';
export const runtime = 'nodejs';
export const dynamic = 'force-dynamic';
const headers = {'Cache-Control':'no-store'};
export async function GET() {
  if (process.env.ITO_DEMO === 'true') return Response.json({source:'demo',received_at:Date.now(),telemetry:demo()}, {headers});
  if (process.env.ITO_TRANSPORT === 'mqtt') {
    try {
      const snapshot = mqttTelemetry();
      if (snapshot) return Response.json(snapshot, {headers});
      return Response.json({error:'Waiting for fresh MQTT telemetry from the controller.'}, {status:503,headers});
    } catch {
      return Response.json({error:'MQTT configuration unavailable. Check the server configuration.'}, {status:503,headers});
    }
  }
  const origin = process.env.ITO_DEVICE_URL;
  const token = process.env.ITO_DEVICE_TOKEN;
  if (!origin || !token || token.length < 32) return Response.json({error:'Configure ITO_DEVICE_URL and a 32+ character ITO_DEVICE_TOKEN in .env.local. See README.'}, {status:503,headers});
  let url;
  try { url = deviceURL(origin); } catch { return Response.json({error:'ITO_DEVICE_URL must be a private IPv4 HTTP origin, for example http://192.168.1.100.'}, {status:503,headers}); }
  try {
    const response = await fetch(url, {headers:{'X-ITO-Key':token},cache:'no-store',redirect:'error',signal:AbortSignal.timeout(2500)});
    if (response.status === 401) return Response.json({error:'Device rejected the telemetry key. Match ITO_DEVICE_TOKEN to ITO_WIFI_TOKEN.'}, {status:502,headers});
    if (!response.ok) throw new Error('Device unavailable');
    if (!response.body) throw new Error('No response');
    const reader = response.body.getReader(); const decoder = new TextDecoder(); let text = ''; let length = 0;
    try { for (;;) { const {value,done} = await reader.read(); if (done) break; length += value.byteLength; if (length > 8192) throw new Error('Response too large'); text += decoder.decode(value,{stream:true}); } text += decoder.decode(); }
    finally { await reader.cancel(); }
    return Response.json({source:'device',received_at:Date.now(),telemetry:validate(JSON.parse(text))}, {headers});
  } catch { return Response.json({error:'No valid telemetry from the XIAO. Check power, Wi-Fi, IP address and firmware. Last readings are not live.'}, {status:502,headers}); }
}

