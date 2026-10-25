import test from 'node:test';
import assert from 'node:assert/strict';
import { createTelemetryStore } from '../lib/mqtt-store.mjs';
import { demo } from '../lib/telemetry.mjs';

test('MQTT samples expire; replaying a duplicate never refreshes freshness', () => {
  let clock = 10000;
  const store = createTelemetryStore({now:() => clock});
  const sample = Buffer.from(JSON.stringify(demo()));
  assert.equal(store.ingest(sample), true);
  clock += 4000;
  assert.equal(store.ingest(sample), false);
  clock += 1001;
  assert.equal(store.read(), null);
});
test('retained, malformed, oversized and unsafe samples are rejected', () => {
  const store = createTelemetryStore();
  const data = demo();
  assert.equal(store.ingest(Buffer.from(JSON.stringify(data)), {retain:true}), false);
  assert.equal(store.ingest(Buffer.from('{')), false);
  assert.equal(store.ingest(Buffer.alloc(8193)), false);
  data.outputs[0] = true;
  assert.equal(store.ingest(Buffer.from(JSON.stringify(data))), false);
  assert.equal(store.read(), null);
});
test('live progress, device restart, and broker disconnect', () => {
  const store = createTelemetryStore();
  const data = {...demo(), sequence:10, uptime_ms:10000};
  assert.equal(store.ingest(Buffer.from(JSON.stringify(data))), true);
  data.sequence = 9; data.uptime_ms = 11000;
  assert.equal(store.ingest(Buffer.from(JSON.stringify(data))), false);
  data.sequence = 1; data.uptime_ms = 500;
  assert.equal(store.ingest(Buffer.from(JSON.stringify(data))), true);
  store.clear();
  assert.equal(store.read(), null);
});
