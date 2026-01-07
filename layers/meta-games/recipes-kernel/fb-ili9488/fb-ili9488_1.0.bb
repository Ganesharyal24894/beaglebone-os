SUMMARY = "ILI9488 FBTFT Display Driver"
DESCRIPTION = "Framebuffer driver for ILI9488 TFT LCD with 18-bit color support"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = "file://fb_ili9488.c \
           file://Makefile"

S = "${UNPACKDIR}"

EXTRA_OEMAKE:append = " KERNEL_SRC=${STAGING_KERNEL_DIR}"

# Auto-load the module at boot
KERNEL_MODULE_AUTOLOAD += "fb_ili9488"

# Make sure fbtft core is enabled
RDEPENDS:${PN} = "kernel-module-fbtft"
