import mqtt from 'mqtt';
import { createTelemetryStore } from './mqtt-store.mjs';

const key = Symbol.for('ito.mqtt.telemetry.v1');

export function mqttTelemetry() {
  if (!globalThis[key]) {
    const url = new URL(process.env.ITO_MQTT_URL || '');
    if (!['mqtt:', 'mqtts:'].includes(url.protocol) || url.username || url.password)
      throw new Error('Configure the broker URL and separate credential variables');
    const topic = process.env.ITO_MQTT_TOPIC;
    if (!topic || /[+#\u0000]/.test(topic) || topic.length > 200)
      throw new Error('Configure a single telemetry topic');
    const store = createTelemetryStore();
    const client = mqtt.connect(url.href, {
      username:process.env.ITO_MQTT_USER,
      password:process.env.ITO_MQTT_PASSWORD,
      clean:true, reconnectPeriod:2000, connectTimeout:5000,
      keepalive:15, protocolVersion:4, queueQoSZero:false
    });
    client.on('connect', () => {
      store.clear();
      client.subscribe(topic, {qos:0}, error => { if (error) store.clear(); });
    });
    client.on('message', (receivedTopic, payload, packet) => {
      if (receivedTopic === topic) store.ingest(payload, packet);
    });
    for (const event of ['close','offline','error']) client.on(event, () => store.clear());
    globalThis[key] = {client, store};
  }
  return globalThis[key].store.read();
}
