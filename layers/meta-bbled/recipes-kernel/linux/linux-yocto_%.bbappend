FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://gpio-sysfs.cfg"

# Enable UART1 by modifying the device tree source directly
do_patch:append() {
    # Add uart1_pins after uart0_pins block
    sed -i '/uart0_pins: uart0-pins {/,/};/{
        /};/a\
\
\tuart1_pins: uart1-pins {\
\t\tpinctrl-single,pins = <\
\t\t\tAM33XX_PADCONF(AM335X_PIN_UART1_RXD, PIN_INPUT_PULLUP, MUX_MODE0)\
\t\t\tAM33XX_PADCONF(AM335X_PIN_UART1_TXD, PIN_OUTPUT_PULLDOWN, MUX_MODE0)\
\t\t>;\
\t};
    }' ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi

    # Add uart1 node after uart0 node
    sed -i '/^&uart0 {/,/^};/{
        /^};/a\
\
\&uart1 {\
\tstatus = "okay";\
\tpinctrl-names = "default";\
\tpinctrl-0 = <\&uart1_pins>;\
};
    }' ${S}/arch/arm/boot/dts/ti/omap/am335x-bone-common.dtsi
}
