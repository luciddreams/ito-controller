import test from 'node:test';
import assert from 'node:assert/strict';
import {validate,deviceURL,demo} from '../lib/telemetry.mjs';
test('valid snapshot and explicit missing measurements',()=>{const d=demo();d.temperatures[0]=null;d.input_a=null;assert.equal(validate(d),d);});
test('malformed or nonfinite measurements rejected',()=>{for(const edit of [d=>d.temperatures.pop(),d=>d.input_a=NaN,d=>d.mode=9,d=>d.watchdog='true',d=>d.ambient[0].humidity='50',d=>d.schema=2]){const d=demo();edit(d);assert.throws(()=>validate(d));}});
test('device origin is fixed private HTTP address',()=>{assert.equal(deviceURL('http://192.168.1.15').href,'http://192.168.1.15/api/status');for(const value of ['https://192.168.1.1','http://example.com','http://169.254.169.254','http://192.168.1.1/command','http://user:pass@192.168.1.1','http://8.8.8.8'])assert.throws(()=>deviceURL(value));});
