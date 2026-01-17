# BeagleBone Hardware BSP - Kernel Configuration
# This bbappend adds all hardware support: GPIO, UART, SPI display, LEDs

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Kernel configuration fragments
SRC_URI += "file://gpio-sysfs.cfg"
SRC_URI += "file://display-ili9488.cfg"

# Kernel patch to add ILI9488 driver to FBTFT
SRC_URI += "file://0001-fbtft-add-fb_ili9488-driver.patch"

# Device tree include for ILI9488 display
SRC_URI += "file://ili9488-display.dtsi"

# Add device tree and enable UART1 during patching
do_patch:append() {
    # Copy display device tree include
    cp ${WORKDIR}/sources/ili9488-display.dtsi ${S}/arch/arm/boot/dts/ti/omap/

    # Include display dtsi in common beaglebone device tree
    if ! grep -q "ili9488-display.dtsi" ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi; then
        echo '' >> ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi
        echo '/* ILI9488 SPI Display Support */' >> ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi
        echo '#include "ili9488-display.dtsi"' >> ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi
    fi

    # Enable UART1 (disabled by default in am335x-bone-common.dtsi)
    if grep -q 'uart1:.*status = "disabled"' ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi 2>/dev/null || \
       grep -q '&uart1 {' ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi 2>/dev/null; then
        sed -i 's/\(&uart1 {\)/\1\n\tstatus = "okay";/' ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi 2>/dev/null || true
    fi
}
