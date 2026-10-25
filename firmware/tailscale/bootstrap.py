"""Fetch exact, reviewed revisions; never overwrite modified vendor checkouts."""
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parent
dependencies = (
    ('microlink', 'https://github.com/CamM2325/microlink.git',
     '216da3300f0493b0860247d43f7af5ce29df63a5'),
    ('pubsubclient', 'https://github.com/knolleary/pubsubclient.git',
     '2d228f2f862a95846c65a8518c79f48dfc8f188c'),
    # Full checkout retains the XIAO variant omitted by the registry package.
    ('arduino', 'https://github.com/espressif/arduino-esp32.git',
     'ee98cf3ca1bccebcfb9e74ed906b51983e416202'),
    ('lib-builder', 'https://github.com/espressif/esp32-arduino-lib-builder.git',
     'fdb4a87b69a6e246a6804648af2e9d1410e8f071'),
    ('lib-builder/components/arduino_tinyusb/tinyusb', 'https://github.com/hathach/tinyusb.git',
     'dc0038f6147584dce64323bb4895a3632c312bc1'),
)
for name, url, revision in dependencies:
    path = root / 'vendor' / name
    if not path.exists():
        path.parent.mkdir(exist_ok=True)
        subprocess.run(['git', 'init', str(path)], check=True)
        subprocess.run(['git', '-C', str(path), 'remote', 'add', 'origin', url], check=True)
        subprocess.run(['git', '-C', str(path), 'fetch', '--depth', '1', 'origin', revision], check=True)
        subprocess.run(['git', '-C', str(path), 'checkout', '--detach', 'FETCH_HEAD'], check=True)
    head = subprocess.check_output(['git', '-C', str(path), 'rev-parse', 'HEAD'], text=True).strip()
    if head != revision:
        dirty = subprocess.check_output(['git', '-C', str(path), 'status', '--porcelain'], text=True)
        if dirty:
            raise SystemExit(f'Preserve local changes in {path} before checkout')
        subprocess.run(['git', '-C', str(path), 'checkout', '--detach', revision], check=True)
