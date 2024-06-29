RP2040_SDK_PATH = /c/Users/niall/qmk_firmware/lib/pico-sdk

DFU_SUFFIX_ARGS = -p FFFF -v FFFF

MIDI_ENABLE = yes
RGB_MATRIX_ENABLE = yes


RP2040_ENABLE = yes
PICO_ENABLE = yes
WS2812_DRIVER = vendor

SRC += platforms/chibios/drivers/vendor/RP/RP2040/ws2812_vendor.c


