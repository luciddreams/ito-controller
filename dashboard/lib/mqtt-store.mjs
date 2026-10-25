import { validate } from './telemetry.mjs';

export function createTelemetryStore({now = Date.now, maxAge = 5000} = {}) {
  let snapshot = null;
  return {
    clear() { snapshot = null; },
    ingest(payload, {retain = false} = {}) {
      // A retained sample can be arbitrarily old after server/device restart.
      if (retain || payload.length > 8192) return false;
      let telemetry;
      try { telemetry = validate(JSON.parse(payload.toString())); }
      catch { return false; }
      if (!telemetry.monitor_only || telemetry.outputs.some(Boolean)) return false;
      if (snapshot) {
        const previous = snapshot.telemetry;
        if (telemetry.sequence === previous.sequence && telemetry.uptime_ms === previous.uptime_ms) return false;
        if (telemetry.uptime_ms >= previous.uptime_ms &&
            ((telemetry.sequence - previous.sequence) >>> 0) >= 0x80000000) return false;
      }
      snapshot = {source:'device', received_at:now(), telemetry};
      return true;
    },
    read() {
      if (!snapshot || now() - snapshot.received_at > maxAge) return null;
      return snapshot;
    }
  };
}
