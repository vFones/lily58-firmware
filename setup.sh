#!/usr/bin/env bash
set -euo pipefail

VIAL_QMK_REPO="https://github.com/vial-kb/vial-qmk.git"
VIAL_QMK_DIR="vial-qmk"
BOARD_SRC="keyboards/lily58/khor"

# ── helpers ──────────────────────────────────────────────────────────────────
green()  { printf '\033[0;32m%s\033[0m\n' "$*"; }
yellow() { printf '\033[0;33m%s\033[0m\n' "$*"; }
red()    { printf '\033[0;31m%s\033[0m\n' "$*"; }
die()    { red "ERROR: $*"; exit 1; }

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# ── dependency checks ─────────────────────────────────────────────────────────
green "==> Checking dependencies..."

command -v git   >/dev/null 2>&1 || die "'git' is not installed. Please install git first."
command -v make  >/dev/null 2>&1 || die "'make' is not installed. Please install build-essential."
command -v python3 >/dev/null 2>&1 || die "'python3' is not installed. Please install python3."

if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    yellow "WARNING: 'arm-none-eabi-gcc' not found."
    yellow "  On Debian/Ubuntu: sudo apt install gcc-arm-none-eabi"
    yellow "  The build will fail without it."
fi

# ── clone vial-qmk ───────────────────────────────────────────────────────────
green "==> Setting up vial-qmk..."

if [ -d "$SCRIPT_DIR/$VIAL_QMK_DIR" ]; then
    yellow "  vial-qmk already exists at $SCRIPT_DIR/$VIAL_QMK_DIR — skipping clone."
else
    git clone "$VIAL_QMK_REPO" "$SCRIPT_DIR/$VIAL_QMK_DIR"
fi

# ── init submodules ───────────────────────────────────────────────────────────
green "==> Initialising QMK submodules (this may take a while)..."
make -C "$SCRIPT_DIR/$VIAL_QMK_DIR" git-submodule

# ── copy board + keymap ───────────────────────────────────────────────────────
green "==> Copying board definition and keymap into vial-qmk..."

DEST="$SCRIPT_DIR/$VIAL_QMK_DIR/$BOARD_SRC"
mkdir -p "$DEST"
cp -r "$SCRIPT_DIR/$BOARD_SRC/." "$DEST/"

green "  Installed at: $DEST"

# ── done ──────────────────────────────────────────────────────────────────────
echo ""
green "Setup complete!"
echo ""
echo "  To build the firmware, run:"
echo ""
echo "    make -C $SCRIPT_DIR/$VIAL_QMK_DIR lily58/khor:atom"
echo ""
echo "  The compiled firmware will be at:"
echo ""
echo "    $SCRIPT_DIR/$VIAL_QMK_DIR/lily58_khor_atom.uf2"
echo ""
echo "  To flash: hold the BOOT button on the RP2040, plug in USB,"
echo "  then drag-and-drop the .uf2 file onto the RPI-RP2 drive."
echo "  Repeat for the right half."
echo ""
