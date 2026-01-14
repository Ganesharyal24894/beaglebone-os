SUMMARY = "YouTube Player with LVGL UI"
DESCRIPTION = "Plays YouTube videos on framebuffer using LVGL and GStreamer"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

# Use gitsm:// to fetch repo with submodules (LVGL)
SRC_URI = "gitsm://github.com/Ganesharyal24894/linux-mediaplayer.git;protocol=https;branch=master"
SRCREV = "ac6d8472a1305ab1c8462977dd2c4158e71d2e3b"

# Dependencies your app needs at build-time
DEPENDS += "gstreamer1.0 gstreamer1.0-plugins-base \
            gstreamer1.0-plugins-good alsa-lib"

# Runtime dependencies
RDEPENDS:${PN} = "yt-dlp gstreamer1.0-plugins-good gstreamer1.0-plugins-base"

inherit cmake pkgconfig

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/youtubeplayer ${D}${bindir}/youtubeplayer
}
