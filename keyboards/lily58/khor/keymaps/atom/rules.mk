VIA_ENABLE = yes
VIAL_ENABLE = yes
WPM_ENABLE = no
MOUSEKEY_ENABLE = yes
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
EXTRAKEY_ENABLE = yes
TAP_DANCE_ENABLE = yes
COMBO_ENABLE = no
GRAVE_ESC_ENABLE = yes
KEY_OVERRIDE_ENABLE = no
VIALRGB_ENABLE = no
NKRO_ENABLE = yes
LTO_ENABLE = yes
SPLIT_KEYBOARD = yes
SLEEP_LED_ENABLE  = no
NO_USB_STARTUP_CHECK = yes

# If you want to change the display of OLED, you need to change here
SRC +=  oled_glitch.c \
        icons.c \
        oled_sync.c \
        user_transport.c \
        oled_clock.c
