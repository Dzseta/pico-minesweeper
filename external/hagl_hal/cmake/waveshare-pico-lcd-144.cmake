#
# Waveshare Pico-LCD-1.44 (128x128 ST7735S)
# https://www.waveshare.com/wiki/Pico-LCD-1.44
# https://botland.store/search?s=5904422347284
#
target_compile_definitions(firmware PRIVATE
    MIPI_DISPLAY_PIN_CS=9
    MIPI_DISPLAY_PIN_DC=8
    MIPI_DISPLAY_PIN_RST=12
    MIPI_DISPLAY_PIN_BL=13
    MIPI_DISPLAY_PIN_CLK=10
    MIPI_DISPLAY_PIN_MOSI=11
    MIPI_DISPLAY_PIN_MISO=-1
    MIPI_DISPLAY_PIN_POWER=-1
    MIPI_DISPLAY_PIN_TE=-1

    MIPI_DISPLAY_SPI_PORT=spi1
    MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ=62500000

    MIPI_DISPLAY_PIXEL_FORMAT=MIPI_DCS_PIXEL_FORMAT_16BIT
    MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_BGR
    MIPI_DISPLAY_WIDTH=128
    MIPI_DISPLAY_HEIGHT=128
    MIPI_DISPLAY_OFFSET_X=2
    MIPI_DISPLAY_OFFSET_Y=1
    # MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_BGR|MIPI_DCS_ADDRESS_MODE_SWAP_XY|MIPI_DCS_ADDRESS_MODE_MIRROR_X
    # MIPI_DISPLAY_WIDTH=128
    # MIPI_DISPLAY_HEIGHT=128
    # MIPI_DISPLAY_OFFSET_X=1
    # MIPI_DISPLAY_OFFSET_Y=2
    MIPI_DISPLAY_INVERT=0
)