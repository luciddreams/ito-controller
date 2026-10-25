'use client';
import { useEffect, useState } from 'react';
type Air = {valid:boolean;age_ms:number;temperature:number|null;humidity:number|null;dewpoint:number|null};
type Telemetry = {firmware:string;uptime_ms:number;sequence:number;sample_age_ms:number;mode:number;fault:string;ready:boolean;installed:number;calibrated:boolean;adc_verified:boolean;watchdog:boolean;io_ok:boolean;sensors_fresh:boolean;load:boolean;button:boolean;efuse_ok:boolean;monitor_only:boolean;outputs:boolean[];raw:number[];mv:number[];input_a:number|null;heater_a:number|null;bus_v:number|null;rpm:number;temperatures:(number|null)[];ambient:Air[]};
type Envelope = {source:'device'|'demo';received_at:number;telemetry:Telemetry};
type Point = {time:number;values:(number|null)[]};
const colors=['#70e0c5','#86aaff','#e7bf78','#d7a0e5','#f68c7e'];
const labels=['Glass 01','Glass 02','Glass 03','Glass 04','Hot side'];
const fmt=(v:number|null|undefined,d=1)=>v==null||!Number.isFinite(v)?'—':v.toFixed(d);
const duration=(ms:number)=>{const s=Math.floor(ms/1000);return `${Math.floor(s/3600)}h ${Math.floor(s%3600/60)}m ${s%60}s`;};
function Trend({points,now}:{points:Point[];now:number}) {
 points=points.filter(p=>now-p.time<=120000);
 const values=points.flatMap(p=>p.values).filter((n):n is number=>n!==null&&Number.isFinite(n));
 const lo=values.length?Math.floor(Math.min(...values)-2):15, hi=values.length?Math.ceil(Math.max(...values)+2):35;
 const end=now,start=end-120000;
 return <div className="chart"><svg viewBox="0 0 840 220" role="img" aria-label="Temperature history over the last two minutes">
 {[0,1,2,3,4].map(i=><g key={i}><line x1="42" x2="825" y1={15+i*43} y2={15+i*43} stroke="#283536" strokeDasharray="3 5"/><text x="0" y={19+i*43} fill="#8b9c9b" fontSize="11">{fmt(hi-(hi-lo)*i/4,0)}°</text></g>)}
 {labels.map((_,j)=>{let open=false,prev=0;const d=points.map(p=>{const v=p.values[j];if(v===null){open=false;return '';}const x=42+(p.time-start)/120000*783,y=15+(hi-v)/(hi-lo)*172;const cmd=open&&p.time-prev<4000?'L':'M';open=true;prev=p.time;return `${cmd}${x.toFixed(1)},${y.toFixed(1)}`;}).join(' ');return <path key={j} d={d} stroke={colors[j]} strokeWidth="2" fill="none"/>;})}
 <text x="42" y="215" fill="#8b9c9b" fontSize="11">−2 min</text><text x="425" y="215" fill="#8b9c9b" fontSize="11">−1 min</text><text x="803" y="215" fill="#8b9c9b" fontSize="11">Now</text></svg>
 {!values.length&&<div className="chart-empty">Waiting for valid temperature readings</div>}</div>;
}
export default function Dashboard(){
 const [packet,setPacket]=useState<Envelope|null>(null),[error,setError]=useState(''),[clock,setClock]=useState(0),[seen,setSeen]=useState(0),[points,setPoints]=useState<Point[]>([]),[paused,setPaused]=useState(false);
 useEffect(()=>{const id=setInterval(()=>setClock(Date.now()),1000);return()=>clearInterval(id);},[]);
 useEffect(()=>{if(paused)return;let stopped=false;let timer:ReturnType<typeof setTimeout>;let controller:AbortController;let lastSequence=-1,lastUptime=-1,lastSource='';
  async function poll(){controller=new AbortController();try{const r=await fetch('/api/telemetry',{cache:'no-store',signal:controller.signal});const data=await r.json();if(!r.ok)throw new Error(data.error||'Telemetry unavailable');if(stopped)return;const e=data as Envelope;const t=e.telemetry;
   setPacket(e);setError('');if(t.sequence!==lastSequence||t.uptime_ms!==lastUptime){const now=Date.now();setSeen(now);setClock(now);const restart=t.uptime_ms<lastUptime||lastSource!==e.source;setPoints(old=>[...(restart?[]:old).filter(p=>now-p.time<=120000),{time:now,values:t.temperatures.map((v,i)=>t.io_ok&&t.sample_age_ms<=500&&(i===4||t.sensors_fresh)?v:null)}]);lastSequence=t.sequence;lastUptime=t.uptime_ms;lastSource=e.source;}
  }catch(e){if(!stopped)setError(e instanceof Error?e.message:'Connection failed');}finally{if(!stopped)timer=setTimeout(poll,1000);}}
  poll();return()=>{stopped=true;clearTimeout(timer);controller?.abort();};
 },[paused]);
 const t=packet?.telemetry;const stale=!seen||clock-seen>4000||!!error||paused;const valid=!!t&&!stale;const samples=valid&&t!.io_ok&&t!.sample_age_ms<=500;
 const tempValid=samples&&t!.sensors_fresh;const demo=packet?.source==='demo';
 const status=paused?'Paused':error?'Disconnected':!packet?'Connecting':stale?'Stale telemetry':demo?'Demo data':'Live telemetry';
 const exportJSON=()=>{if(!packet)return;const blob=new Blob([JSON.stringify({...packet,exported_at:new Date().toISOString(),stale},null,2)],{type:'application/json'});const url=URL.createObjectURL(blob);const a=document.createElement('a');a.href=url;a.download=`ito-snapshot-${Date.now()}.json`;a.click();URL.revokeObjectURL(url);};
 return <main>
 <header><a className="brand" href="/" aria-label="ITO dashboard home"><span className="brand-icon">◈</span><span>ITO<span className="brand-sub">THERMAL OBSERVATORY</span></span></a><div className="header-right"><span className={`pill ${valid?'mint':'amber'}`}><i/>{status}</span><span className="revision">REV L · ESP32S3</span></div></header>
 <section className="heading"><div><div className="eyebrow">WORKBENCH / 01</div><h1>A clear view of your controller.</h1><p>Temperatures, environment and interlocks. One local instrument panel.</p></div><div className="actions"><button onClick={()=>setPaused(x=>!x)}>{paused?'Resume updates':'Pause updates'}</button><button onClick={exportJSON} disabled={!packet}>Export snapshot ↗</button></div></section>
 {demo&&<div className="notice demo">DEMO MODE — simulated readings. No physical controller is connected to this view.</div>}
 <div className="notice" role="status"><span className="notice-icon">◎</span><div><strong>Read-only commissioning</strong><span>Wi-Fi firmware blocks power pulses. Sensor values require calibration before use for control.</span></div><span className="lock">OUTPUT CONTROL LOCKED</span></div>
 {error&&<div className="notice error" role="alert">{error}</div>}
 <section className="metrics" aria-label="Electrical measurements">
 {[['Switched bus',samples?t?.bus_v:null,'V','INA226 · switched rail'],['Input current',samples?t?.input_a:null,'A',t?.calibrated&&t?.adc_verified?'Calibration verified':'Uncalibrated estimate'],['Heater current',samples?t?.heater_a:null,'A','INA226 · heater only'],['Fan speed',samples?t?.rpm:null,'RPM','Tachometer feedback']].map(([title,value,unit,sub],i)=><article className="metric" key={String(title)}><div className="metric-label">{title}<span>0{i+1}</span></div><div className="metric-value">{fmt(value as number|null,i===1||i===2?3:i===3?0:2)}<small>{unit}</small></div><p>{sub}</p></article>)}
 </section>
 <div className="main-grid"><section className="panel thermal"><div className="panel-heading"><div><div className="eyebrow">TEMPERATURE ARRAY</div><h2>Glass & hot side</h2></div><span className="muted">2-minute window</span></div>
 <div className="sensor-row">{labels.map((label,i)=><div key={label}><span><i style={{background:colors[i]}}/>{label}</span><strong>{fmt((i===4?samples:tempValid)?t?.temperatures[i]:null)}<small> °C</small></strong><small>{(i===4?samples:tempValid)&&t?.temperatures[i]!=null?'Measured · verify calibration':'No valid reading'}</small></div>)}</div>
 <Trend points={points} now={clock}/><div className="panel-foot">J21–J24 · glass NTCs<span>J25 · hot-side NTC</span></div></section>
 <section className="panel state"><div className="eyebrow">CONTROLLER STATE</div><h2>{valid?['Idle','Relay wait','Heating','TEC pulse','Fault latched'][t!.mode]:'Awaiting telemetry'}</h2><div className={`fault ${valid&&t?.fault!=='NONE'?'amber':''}`}>{valid?t?.fault:'No current fault status'}</div><p className="state-copy">{valid&&t?.fault==='STALE_SENSOR'?'Sensor readings are missing or stale. Connect the probes before commissioning.':'Faults and calibration locks remain enforced by the controller.'}</p>
 <dl><div><dt>Watchdog</dt><dd>{valid?(t?.watchdog?'Active':'Not active'):'—'}</dd></div><div><dt>I²C communication</dt><dd>{valid?(t?.io_ok?'Responding':'Read failed'):'—'}</dd></div><div><dt>Calibration</dt><dd>{valid?(t?.calibrated&&t?.adc_verified?'Verified':'Pending'):'—'}</dd></div><div><dt>Uptime</dt><dd>{valid?duration(t!.uptime_ms):'—'}</dd></div></dl></section></div>
 <div className="lower-grid"><section className="panel"><div className="panel-heading"><div><div className="eyebrow">ENVIRONMENT</div><h2>Air & dew point</h2></div><span className="muted">SHT31-D</span></div><div className="air-grid">{[0,1].map(i=>{const a=t?.ambient[i];const ok=valid&&a?.valid&&a.age_ms<=3500;return <article className="air" key={i}><div className="air-title">Ambient 0{i+1}<span>0x{(0x44+i).toString(16)}</span></div><div className="air-values"><div><strong>{fmt(ok?a.temperature:null)}<small> °C</small></strong><span>Temperature</span></div><div><strong>{fmt(ok?a.humidity:null)}<small> %</small></strong><span>Humidity</span></div></div><div className="dew">Dew point <b>{fmt(ok?a.dewpoint:null)} °C</b></div><small className="muted">{ok?'Valid sensor reading':'Sensor missing or stale'}</small></article>;})}</div></section>
 <section className="panel"><div className="panel-heading"><div><div className="eyebrow">HARDWARE INTERLOCKS</div><h2>Signals & outputs</h2></div><span className="muted">Readback</span></div><div className="signals">{[['Load rail present',t?.load,'D2'],['Dead-man held',t?.button,'D10'],['eFuse healthy',t?.efuse_ok,'TCA P4'],['Heater drive',t?.outputs[0],'D3'],['TEC arm',t?.outputs[1],'D6'],['Power arm',t?.outputs[2],'D9']].map(([name,on,pin],i)=><div key={String(name)}><span>{name}<small>{pin}</small></span><b className={valid&&(i!==2||samples)&&on?'signal-on':''}>{valid&&(i!==2||samples)?(on?'ACTIVE':'INACTIVE'):'UNKNOWN'}</b></div>)}</div><p className="muted signal-note">Input labels show logical state; active-low inputs may have a low physical voltage.</p></section></div>
 <details className="panel diagnostics"><summary>Raw ADC & device details <span>Technical view +</span></summary><div className="diagnostic-grid"><div>Input ADC<strong>{valid?t?.raw[0]:'—'} counts / {valid?t?.mv[0]:'—'} mV</strong></div><div>Hot-side ADC<strong>{valid?t?.raw[1]:'—'} counts / {valid?t?.mv[1]:'—'} mV</strong></div><div>Installed TEC channels<strong>{valid?[1,2,3,4].filter(n=>t!.installed&(1<<(n-1))).map(n=>`U${n+10}`).join(', ')||'None':'—'}</strong></div><div>Firmware<strong>{t?.firmware||'—'}</strong></div></div></details>
 <footer><span>ITO CONTROLLER <b> / </b> LOCAL TELEMETRY</span><span aria-live="polite">{seen?`Last new sample ${Math.max(0,Math.floor((clock-seen)/1000))}s ago`:'Waiting for first sample'} · 1s refresh</span></footer>
 </main>;
}
