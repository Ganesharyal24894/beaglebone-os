SUMMARY = "ILI9488 FBTFT Display Driver"
DESCRIPTION = "Framebuffer driver for ILI9488 TFT LCD with 18-bit color support"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = "https://raw.githubusercontent.com/birdtechstep/fbtft-4.14/master/fb_ili9488.c;name=driver \
           file://Makefile \
           file://0001-fix-gpio-api.patch"

SRC_URI[driver.sha256sum] = "147ce2f34ec4917d591d65d06706edce2b2a94ee3d0ce945989c049b74da52ad"

S = "${UNPACKDIR}"

EXTRA_OEMAKE:append = " KERNEL_SRC=${STAGING_KERNEL_DIR}"

# Auto-load the module at boot
KERNEL_MODULE_AUTOLOAD += "fb_ili9488"

# Make sure fbtft core is enabled
RDEPENDS:${PN} = "kernel-module-fbtft"
