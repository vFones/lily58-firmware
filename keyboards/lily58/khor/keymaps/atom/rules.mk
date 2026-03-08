VIA_ENABLE = yes
VIAL_ENABLE = yes
WPM_ENABLE = yes
MOUSEKEY_ENABLE = no
CONSOLE_ENABLE = yes
COMMAND_ENABLE = no
EXTRAKEY_ENABLE = yes
TAP_DANCE_ENABLE = no
COMBO_ENABLE = no
GRAVE_ESC_ENABLE = yes
KEY_OVERRIDE_ENABLE = no
VIALRGB_ENABLE = no
NKRO_ENABLE = yes
LTO_ENABLE = yes
SPLIT_KEYBOARD = yes

# If you want to change the display of OLED, you need to change here
SRC +=  oled_glitch.c \
        icons.c \
        oled_sync.c \
        user_transport.c
