FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Add display kernel config fragment
SRC_URI += "file://display-ili9488.cfg"

# Add ILI9488 driver to kernel tree (makes it in-tree, can be built-in)
SRC_URI += "file://0001-fbtft-add-fb_ili9488-driver.patch"

# Add display device tree include file
SRC_URI += "file://ili9488-display.dtsi"

# Include the display dtsi in the bone-common dtsi
do_patch:append() {
    # Copy the dtsi file to the correct location
    cp ${WORKDIR}/sources/ili9488-display.dtsi ${S}/arch/arm/boot/dts/ti/omap/

    # Add include for display overlay at the end of am335x-bone-common.dtsi
    if ! grep -q "ili9488-display.dtsi" ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi; then
        echo '' >> ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi
        echo '/* ILI9488 SPI Display Support */' >> ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi
        echo '#include "ili9488-display.dtsi"' >> ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi
    fi
}
