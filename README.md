# Lily58 Atom — Vial Keymap

![Build Firmware](https://github.com/vFones/lily58-config/actions/workflows/build.yml/badge.svg)

Personal [Vial](https://get.vial.today/) keymap for the **Lily58 PRO V2** running on an RP2040 MCU. Features a custom OLED animation system that displays per-key icons synced across both halves, a scrolling Atom logo, and a safety-gated bootloader entry.

![Lily58 Atom showcase](lily58_showcase.gif)

---

## Hardware

| Component       | Detail                                                                  |
| -----------------| -------------------------------------------------------------------------|
| Keyboard        | [Lily58 PRO V2](https://khor.store/products/lily58pro-2-0) (Khor Store) |
| MCU             | RP2040 (Raspberry Pi Pico)                                              |
| Split transport | Full-duplex serial — TX: GP0, RX: GP1                                   |
| Display         | SSD1306 OLED (128×32, 270° rotation)                                    |
| RGB             | WS2812 on GP11 (PIO1)                                                   |
| Matrix          | 5 rows × 6 cols per side, COL2ROW                                       |

---

## Quick Start

```bash
git clone https://github.com/YOUR_GITHUB_USERNAME/lily58-config.git
cd lily58-config
./setup.sh
```

`setup.sh` will:
1. Check that `git`, `make`, `python3`, and `arm-none-eabi-gcc` are installed
2. Clone [vial-qmk](https://github.com/vial-kb/vial-qmk) into `./vial-qmk/`
3. Initialise all QMK submodules
4. Copy the keymap into the correct location inside vial-qmk

Then build:

```bash
make -C vial-qmk lily58/khor:atom
```

The compiled firmware is written to `vial-qmk/lily58_khor_atom.uf2`.

### Install dependencies (Debian / Ubuntu)

```bash
sudo apt install git make python3 python3-pip gcc-arm-none-eabi binutils-arm-none-eabi
pip3 install qmk
```

---

## Flashing

1. Hold the **BOOT** button on the RP2040, then plug in USB — a `RPI-RP2` drive will appear.
2. Drag and drop `lily58_khor_atom.uf2` onto the drive. The board reboots automatically.
3. Repeat for the **right half**.

> Tip: pressing `BOOT` (top-left key on `_RAISETWO`) triggers the same bootloader entry from firmware, with a 3-second countdown displayed on the OLED. Any keypress during the countdown aborts it.

---

## Layers

### Layer 0 — QWERTY (base)

```
,-----------------------------------------.                    ,-----------------------------------------.
| ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |   =  |
|------+------+------+------+------+------|                    |------+------+------+------+------+------|
| Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |   -  |
|------+------+------+------+------+------|                    |------+------+------+------+------+------|
|LCTRL |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |   '  |
|------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
|LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |RCTRL |
`-----------------------------------------/       /     \      \-----------------------------------------'
                  |LRAIS | LGUI | LAlt | /Space  /       \Enter \  |BkSp  | RGUI |RAISE |
                  `----------------------------'           '------''--------------------'
```

### Layer 1 — RAISE (function / navigation / media)

```
,-----------------------------------------.                    ,-----------------------------------------.
|  `   |  F1  |  F2  |  F3  |  F4  |  F5  |                    |  F6  |  F7  |  F8  |  F9  | F10  | F11  |
|------+------+------+------+------+------|                    |------+------+------+------+------+------|
|      |      |      |      |      |      |                    |      |      | Prev | Next | Play | F12  |
|------+------+------+------+------+------|                    |------+------+------+------+------+------|
|TgWin |      |      |      |      |      |-------.    ,-------| Left | Down |  Up  |Right |      | F13  |
|------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
|      | Prev | Next | Play | Mute |      |-------|    |-------| End  | PgDn | PgUp | Home |  \   | F14  |
`-----------------------------------------/       /     \      \-----------------------------------------'
                  |      |TgWin |      | /       /       \      \  | Del  |RAISE2|      |
                  `----------------------------'           '------''--------------------'
```

### Layer 2 — RAISETWO (system)

```
,-----------------------------------------.                    ,-----------------------------------------.
| BOOT |      |      |      |      |      |                    |      |      |      |      |      |      |
|------+------+------+------+------+------|                    |------+------+------+------+------+------|
|REBOOT|      |      |      |      |      |                    |      |      |      |      |      |      |
|------+------+------+------+------+------|                    |------+------+------+------+------+------|
|      |      |      |      |      |      |-------.    ,-------|      |      |      |      |      |      |
|------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
|      |      |      |      |      |      |-------|    |-------|      |      |      |      |      |      |
`-----------------------------------------/       /     \      \-----------------------------------------'
                  |      |      |      | /       /       \      \  |      |      |      |
                  `----------------------------'           '------''--------------------'
```

Reached by holding `RAISE`, then holding `RAISE2` (the RGUI key on base layer).

---

## Special Keys

| Key | Behaviour |
|-----|-----------|
| `LRAISE` | Momentary `_RAISE` — left-side thumb key version of `RAISE` |
| `TG_WIN` | Toggle Windows / macOS mode — in Windows mode `LGUI` is remapped to `LCTL` so shortcuts feel native |
| `QK_BOOT` | Enter RP2040 bootloader after a 3-second OLED countdown (any key cancels) |
| `QK_REBOOT` | Soft-reboot the firmware |

---

## OLED Animations

Each OLED displays a scrolling **Atom logo** when idle. Pressing a mapped key on either half triggers a full-screen animated icon on the side where the key was pressed, synced via a custom RPC transport layer over the split serial link.

| Key | Icon |
|-----|------|
| `KC_MUTE` | Volume / mute animation |
| `KC_MPLY` | Play / pause animation |
| `KC_MNXT` | Skip-forward animation |
| `KC_MPRV` | Skip-back animation |
| `RAISE` / `LRAISE` | Raise layer icon |
| `RAISETWO` | Raise-2 layer icon |
| `KC_UP` | Arrow up |
| `KC_DOWN` | Arrow down |
| `KC_LEFT` | Arrow left |
| `KC_RGHT` | Arrow right |
| `KC_HOME` | Home icon |
| `KC_END` | End icon |
| `KC_PGUP` | Page up icon |
| `KC_PGDN` | Page down icon |
| `KC_F13` | F13 icon |
| `KC_F14` | F14 icon |

Icons are stored as 16-bit OLED bitmap arrays and rendered with an optional **glitch effect** (XORshift32-based scanline displacement and bit-flipping) for visual flair.

---

## Enabled QMK Features

| Feature | State | Notes |
|---------|-------|-------|
| Vial | ✅ | Full runtime remapping via Vial GUI |
| NKRO | ✅ | N-key rollover |
| Extra keys | ✅ | Media / system keys |
| Grave ESC | ❌ | |
| WPM | ❌ | |
| LTO | ✅ | Link-time optimisation — smaller firmware |
| Split transport mirror | ✅ | Layer state, LED state, mods, OLED synced |
| OLED | ✅ | Custom animation sync via `SPLIT_TRANSACTION_IDS_USER ANIM_SYNC` |
| Tap Dance | ❌ | |
| Combos | ❌ | |
| Key overrides | ❌ | |
| Mouse keys | ❌ | |
| RGB Matrix | ❌ | |
| Vial RGB | ❌ | |

Tapping term is **250 ms** with `TAPPING_FORCE_HOLD` enabled.

---

## Vial GUI

Download the [Vial app](https://get.vial.today/) and plug in the keyboard — it will be detected automatically. You can remap any key, create macros, and adjust tap-term settings without reflashing.

The unlock combo is **top-left + second-from-left** on the top row (row 0, cols 0–1).

---

## Project Layout

```
keyboards/lily58/
└── khor/                      — Khor's Lily58 PRO V2 board definition (lily58/khor in vial-qmk)
    ├── keyboard.json           — hardware: RP2040, matrix, OLED, WS2812, split
    ├── config.h                — debounce, I2C pins, bootmagic, split sync flags
    ├── rules.mk                — SERIAL_DRIVER = vendor
    └── keymaps/
        └── atom/              — personal keymap
            ├── keymap.c        — layer definitions, OLED task, boot countdown, process_record
            ├── config.h        — Vial UID, tapping term, split pins, OLED timeout
            ├── rules.mk        — feature flags, extra source files
            ├── vial.json       — Vial descriptor (name, USB IDs, matrix size)
            ├── icons.c/h       — OLED bitmap data and render functions
            ├── oled_sync.c/h   — split RPC dispatch: routes key-press events to correct OLED
            ├── oled_glitch.c/h — XORshift32 glitch effect renderer
            └── user_transport.c/h — thin wrapper around QMK's split transaction API
```

> `setup.sh` copies `keyboards/lily58/khor/` into vial-qmk as-is. The firmware is built with `make lily58/khor:atom`.
