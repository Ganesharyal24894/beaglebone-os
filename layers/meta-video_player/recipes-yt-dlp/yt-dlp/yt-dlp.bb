SUMMARY = "yt-dlp - YouTube video URL extractor"
DESCRIPTION = "yt-dlp script to extract direct video URLs"
HOMEPAGE = "https://github.com/yt-dlp/yt-dlp"

LICENSE = "Unlicense"
LIC_FILES_CHKSUM = "file://COPYING.UNLICENSE;md5=d88e9e08385d2a17052dac348bde4bc1"

S = "${UNPACKDIR}"

SRC_URI = "file://yt-dlp \
           file://COPYING.UNLICENSE"

RDEPENDS:${PN} = "python3"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/yt-dlp ${D}${bindir}/yt-dlp
}
