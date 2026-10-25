# MQTT dashboard on luciddreams

Live at `https://mirror.opiumkit.com`, deployed 2026-10-25. The ESP32 publishes
to the Mosquitto broker on luciddreams through its native Tailscale connection.
This container only subscribes and displays fresh telemetry. It does not create,
replace, or reconfigure the broker.

The site is public on purpose: the dashboard is read-only and cannot send
commands to the board. Add a login before any admin or configuration features.

## How it runs

- Mosquitto runs in the `mosquitto` container on Docker's default bridge,
  published on host port 1883. It allows anonymous clients. The host firewall
  only accepts 1883 from the tailnet (100.64.0.0/10), Docker networks
  (172.16.0.0/12) and localhost, so it is closed to the internet.
- The dashboard is the Compose project `ito-dashboard` in `/opt/ito-dashboard`,
  bound to `127.0.0.1:3100`. It runs as a non-root user with a read-only root
  filesystem and `restart: unless-stopped`.
- `/opt/ito-dashboard/.env.mqtt` (mode 600, server only, never committed):

  ```
  ITO_TRANSPORT=mqtt
  ITO_DEMO=false
  ITO_MQTT_URL=mqtt://host.docker.internal:1883
  ITO_MQTT_TOPIC=ito/controller-01/telemetry
  ```

  No `ITO_MQTT_USER`/`ITO_MQTT_PASSWORD` while the broker is anonymous. Credentials
  go in those variables, never in the URL.
- Plesk serves `mirror.opiumkit.com` with its Let's Encrypt certificate. The proxy
  to the container is in
  `/var/www/vhosts/system/mirror.opiumkit.com/conf/vhost_nginx.conf` (copy kept in
  `/opt/ito-dashboard/plesk-nginx.conf`); Plesk shows it as the site's
  "Additional nginx directives", so editing that field replaces it. The regex
  `location ~ ^/` wins over Plesk's own `location /` (proxy to Apache) without
  duplicating it, while Plesk's `^~ /.well-known/acme-challenge/` still wins, so
  certificate renewals keep working. This Plesk version has no CLI option for
  these directives; after changing the file run
  `plesk sbin httpdmng --reconfigure-domain mirror.opiumkit.com` and `nginx -t`.

## Deploy or update

From the repository root on a machine with SSH access (`ssh luciddreams`):

```sh
git archive --format=tar HEAD:dashboard | ssh luciddreams 'tar -x -C /opt/ito-dashboard'
ssh luciddreams 'cd /opt/ito-dashboard && docker compose -p ito-dashboard up -d --build'
```

`git archive` sends only committed files, so local `.env*`, `node_modules` and
`.next` never reach the server, and the server's `.env.mqtt` is left in place.

## Check

```sh
ssh luciddreams 'curl -s http://127.0.0.1:3100/api/telemetry'
curl -s https://mirror.opiumkit.com/api/telemetry
```

A 503 `Waiting for fresh MQTT telemetry` is expected until the board publishes a
fresh, non-retained sample; a 200 with a rising `sequence` means it is live.
Missing sensors remain null. Off the PCB the board reports `mode=4`.

## Behaviour

The server rejects retained, invalid, oversized (>8192-byte), or non-monitor
samples. Duplicate samples do not refresh freshness. Readings expire after five
seconds without progress and are cleared when MQTT disconnects. MQTT transport
selection is explicit (`ITO_TRANSPORT=mqtt`), so a broker failure never switches
to the LAN HTTP device or demo readings. History remains in-browser, not durable.

## Open

- Mosquitto is anonymous, so any client on the tailnet or the server's Docker
  networks can publish to the telemetry topic. Before adding admin features,
  create a publish-only device account and a subscribe-only dashboard account
  limited to `ito/controller-01/telemetry`, then set the dashboard's
  `ITO_MQTT_USER`/`ITO_MQTT_PASSWORD` and the firmware's MQTT credentials.

Verified 2026-10-25: the board (16-peer MicroLink build) published over
Tailscale, and `https://mirror.opiumkit.com/` and `/api/telemetry` returned 200
with live, advancing samples.
