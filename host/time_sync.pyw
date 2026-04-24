# Lily58 Clock Time Sync
# Sends the current time to the keyboard every 30 seconds via raw HID.
#
# Requirements:
#   pip install hidapi
#
# Run manually:
#   python3 host/time_sync.py
#
# Auto-start on macOS (runs at login, no window):
#   1. Copy time_sync.py somewhere permanent, e.g. ~/lily58/time_sync.py
#   2. Create ~/Library/LaunchAgents/com.lily58.timesync.plist with:
#
#      <?xml version="1.0" encoding="UTF-8"?>
#      <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
#        "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
#      <plist version="1.0"><dict>
#        <key>Label</key><string>com.lily58.timesync</string>
#        <key>ProgramArguments</key>
#        <array>
#          <string>/usr/bin/python3</string>
#          <string>/Users/YOUR_USERNAME/lily58/time_sync.py</string>
#        </array>
#        <key>RunAtLoad</key><true/>
#        <key>KeepAlive</key><true/>
#      </dict></plist>
#
#   3. Load it:  launchctl load ~/Library/LaunchAgents/com.lily58.timesync.plist
#
# Auto-start on Windows (no console window):
#   1. Rename this file to time_sync.pyw
#   2. Press Win+R, type:  shell:startup
#   3. In the Startup folder, create a shortcut pointing to:
#        Target:  pythonw.exe "C:\path\to\time_sync.pyw"
#      (pythonw.exe is in your Python installation directory)
#   4. Log off and back in — the clock will sync automatically at login.
#

import datetime
import time
import os
import sys

try:
    import hid
except ImportError:
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "hidapi"])
    import hid

VID           = 0x4357
PID           = 0x0003
CMD_TIME_SYNC = 0x54  # 'T', outside VIA (0x01-0x13) and Vial (0xFE) ranges
USAGE_PAGE    = 0xFF60
SYNC_INTERVAL = 300  # seconds

def log(msg):
    ts = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    line = f"[{ts}] {msg}\n"
    print(line)


def find_keyboard():
    devices = hid.enumerate(VID, PID)
    if not devices:
        log(f"No HID device found with VID=0x{VID:04X} PID=0x{PID:04X}")
        return None
    for dev in devices:
        if dev.get("usage_page") == USAGE_PAGE:
            log(f"Found keyboard at path: {dev['path']}")
            return dev["path"]
    # Fallback: return first match if usage_page filter misses (some hidapi builds
    # don't populate usage_page on Windows — we pick the last interface which is
    # typically the raw HID one for QMK/Vial keyboards)
    log(f"usage_page filter missed — trying last interface as fallback")
    return devices[-1]["path"]


def sync_time(path):
    now = datetime.datetime.now()
    packet = bytes([0x00, CMD_TIME_SYNC, now.hour, now.minute, now.second] + [0x00] * 27)
    d = hid.device()
    try:
        d.open_path(path)
        d.write(packet)
        log(f"Synced time: {now.hour:02d}:{now.minute:02d}:{now.second:02d}")
        return True
    except Exception as e:
        log(f"Write failed: {e}")
        return False
    finally:
        d.close()


def main():
    log("time_sync started")
    while True:
        try:
            path = find_keyboard()
            if path:
                sync_time(path)
        except Exception as e:
            log(f"Unexpected error: {e}")
        time.sleep(SYNC_INTERVAL)


if __name__ == "__main__":
    main()
