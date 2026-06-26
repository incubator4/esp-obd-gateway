#!/usr/bin/env python3
"""Collect PlatformIO firmware.bin files into a versioned release directory."""

from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import sys
import zipfile
from datetime import datetime, timezone
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parents[1]
BUILD_DIR = PROJECT_DIR / ".pio" / "build"

RELEASE_FIRMWARES = [
    {
        "env": "gateway",
        "artifact": "gateway.bin",
        "label": "OBD Gateway (4D GEN4 ESP32-S3)",
        "board": "4d_systems_esp32s3_gen4_r8n16",
        "chip": "esp32s3",
        "pio_env": "gateway",
    },
    {
        "env": "display_c6_13",
        "artifact": "display-c6-13.bin",
        "label": "Display — Waveshare ESP32-C6-LCD-1.3",
        "board": "esp32-c6-devkitm-1",
        "chip": "esp32c6",
        "pio_env": "display_c6_13",
    },
    {
        "env": "display_c6_147",
        "artifact": "display-c6-147.bin",
        "label": "Display — Waveshare ESP32-C6-LCD/Touch-LCD-1.47",
        "board": "esp32-c6-devkitm-1",
        "chip": "esp32c6",
        "pio_env": "display_c6_147",
    },
    {
        "env": "display_s3_169",
        "artifact": "display-s3-169.bin",
        "label": "Display — Waveshare ESP32-S3-LCD/Touch-LCD-1.69",
        "board": "esp32-s3-devkitc-1",
        "chip": "esp32s3",
        "pio_env": "display_s3_169",
    },
]

FLASHING_MD = """# Flashing release firmware

Prefer **PlatformIO** with the matching environment — partition offsets are handled automatically.

```bash
# Gateway
pio run -e gateway -t upload

# Display (pick your board)
pio run -e display_c6_13 -t upload
pio run -e display_c6_147 -t upload
pio run -e display_s3_169 -t upload
```

Or use the Makefile shortcuts:

```bash
make flash ENV=gateway PORT=/dev/ttyUSB0
make flash-display BOARD=c6_147 PORT=/dev/ttyUSB0
```

## Artifacts in this bundle

| File | PlatformIO env | Target |
|------|----------------|--------|
| `gateway.bin` | `gateway` | 4D GEN4-S3 OBD gateway |
| `display-c6-13.bin` | `display_c6_13` | Waveshare C6 1.3" |
| `display-c6-147.bin` | `display_c6_147` | Waveshare C6 1.47" (touch auto-detect) |
| `display-s3-169.bin` | `display_s3_169` | Waveshare S3 1.69" (touch auto-detect) |

Touch vs non-touch on 1.47\" / 1.69\" boards is detected at boot over I2C; use the same firmware for both variants.

## esptool (advanced)

If you flash `.bin` files directly, use the **app partition offset** from your board's partition table
(typically `0x10000` for Arduino builds). Example:

```bash
esptool.py --chip esp32c6 --port /dev/ttyUSB0 write_flash 0x10000 display-c6-147.bin
esptool.py --chip esp32s3 --port /dev/ttyUSB0 write_flash 0x10000 gateway.bin
```

When in doubt, use `pio run -e <env> -t upload`.
"""


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def package_release(version: str, dist_root: Path) -> tuple[Path, Path]:
    version = version.lstrip("v")
    bundle_name = f"esp-obd-gateway-v{version}"
    bundle_dir = dist_root / bundle_name
    zip_path = dist_root / f"{bundle_name}.zip"

    if bundle_dir.exists():
        shutil.rmtree(bundle_dir)
    bundle_dir.mkdir(parents=True, exist_ok=True)

    manifest_firmwares = []
    missing = []

    for entry in RELEASE_FIRMWARES:
        src = BUILD_DIR / entry["env"] / "firmware.bin"
        dst = bundle_dir / entry["artifact"]
        if not src.is_file():
            missing.append(entry["env"])
            continue

        shutil.copy2(src, dst)
        size = dst.stat().st_size
        manifest_firmwares.append(
            {
                "env": entry["env"],
                "pio_env": entry["pio_env"],
                "file": entry["artifact"],
                "label": entry["label"],
                "board": entry["board"],
                "chip": entry["chip"],
                "size_bytes": size,
                "sha256": sha256_file(dst),
            }
        )
        print(f"  {entry['artifact']}  ({size:,} bytes)  <- {src}")

    if missing:
        raise SystemExit(
            "Missing build outputs for: "
            + ", ".join(missing)
            + "\nRun: pio run -e "
            + " -e ".join(missing)
        )

    manifest = {
        "project": "esp-obd-gateway",
        "version": version,
        "created_at": datetime.now(timezone.utc).isoformat(),
        "firmwares": manifest_firmwares,
    }
    manifest_path = bundle_dir / "manifest.json"
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

    flashing_path = bundle_dir / "FLASHING.md"
    flashing_path.write_text(FLASHING_MD, encoding="utf-8")

    if zip_path.exists():
        zip_path.unlink()
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(bundle_dir.rglob("*")):
            if path.is_file():
                archive.write(path, arcname=f"{bundle_name}/{path.relative_to(bundle_dir)}")

    print(f"\nRelease bundle: {bundle_dir}")
    print(f"Release archive: {zip_path}")
    return bundle_dir, zip_path


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--version",
        default="dev",
        help="Release version (e.g. 1.0.0 or v1.0.0)",
    )
    parser.add_argument(
        "--dist",
        type=Path,
        default=PROJECT_DIR / "dist",
        help="Output directory (default: dist/)",
    )
    args = parser.parse_args()

    print(f"Packaging esp-obd-gateway v{args.version.lstrip('v')}...")
    package_release(args.version, args.dist)
    return 0


if __name__ == "__main__":
    sys.exit(main())
