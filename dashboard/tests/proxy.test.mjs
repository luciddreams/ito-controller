import test from 'node:test';
import assert from 'node:assert/strict';
import http from 'node:http';
import {GET} from '../app/api/telemetry/route.ts';
import {demo} from '../lib/telemetry.mjs';
test('proxy: setup, authentication, live telemetry, malformed payload, redirects and offline',async()=>{
 const token='test-only-telemetry-token-000000000000';
 delete process.env.ITO_DEMO;delete process.env.ITO_DEVICE_TOKEN;delete process.env.ITO_DEVICE_URL;
 assert.equal((await GET()).status,503);
 let mode='ok',observed='';const server=http.createServer((req,res)=>{observed=req.headers['x-ito-key'];if(mode==='badkey'){res.writeHead(401);res.end();}else if(mode==='redirect'){res.writeHead(302,{Location:'http://example.com'});res.end();}else if(mode==='large'){res.end('x'.repeat(9000));}else res.end(mode==='bad'?'{}':JSON.stringify(demo()));});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));
 try {
  process.env.ITO_DEVICE_TOKEN=token;process.env.ITO_DEVICE_URL=`http://127.0.0.1:${server.address().port}`;
  const ok=await GET();assert.equal(ok.status,200);const body=await ok.json();assert.equal(body.source,'device');assert.equal(body.telemetry.schema,1);assert.equal(observed,token);assert.ok(!JSON.stringify(body).includes(token));assert.equal(ok.headers.get('cache-control'),'no-store');
  for(mode of ['badkey','bad','redirect','large'])assert.equal((await GET()).status,502);
 } finally {await new Promise(r=>server.close(r));}
 assert.equal((await GET()).status,502);
 process.env.ITO_DEMO='true';assert.equal((await (await GET()).json()).source,'demo');delete process.env.ITO_DEMO;
});
