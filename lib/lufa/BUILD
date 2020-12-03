package(default_visibility = ["//visibility:public"])
load("@AvrToolchain//platforms/cpu_frequency:cpu_frequency.bzl", "cpu_frequency_flag")

LUFA_COPTS = [
    "-Iexternal/LUFA/Projects/USBtoSerial/Config",
    "-DARCH=ARCH_AVR8",
    "-DUSE_LUFA_CONFIG_HEADER",
    "-DF_USB=8000000UL",
    "-isystem external/LUFA",
    "-Iexternal/LUFA/LUFA/Drivers/",
]

cc_library(
    name = "LufaLib",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),

    copts = LUFA_COPTS + cpu_frequency_flag(),
    deps = ["@LUFA//:LufaSerial"]
)
