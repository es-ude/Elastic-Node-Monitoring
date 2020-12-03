load(
    "@AvrToolchain//:helpers.bzl",
#    "cpu_frequency_flag",
#    "create_unity_library",
    "default_embedded_binary",
    "generate_hex",
#    "mcu_avr_gcc_flag",
)
load("@AvrToolchain//platforms/cpu_frequency:cpu_frequency.bzl", "cpu_frequency_flag")

load("@EmbeddedSystemsBuildScripts//Unity:unity.bzl",
    "generate_test_runner",
    "generate_a_unity_test_for_every_file",
    "unity_test")



commonCopts = cpu_frequency_flag()

LUFA_COPTS = [
    "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config",
    "-isystem external/LUFA/",
    # "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config/",
]

#default_embedded_binary(
#	name = "main",
#	srcs = [
#		"src/main.c"],
#	copts = elasticNodeCopts,
#	linkopts = mcu_avr_gcc_flag() + ["-Wl,-u,vfprintf -lprintf_flt -lm"],
#	deps = [
#		"//lib:DebugLib",
#		"//lib:FpgaLibs",
#		"//lib:LedLibs",
#		"//lib:FlashLib",
#		"//lib:XorLib",
#		"//lib:ControlLib",
#        "//lib:SelectmapLib",
#		"//lib:CurrentSenseLib",
#
#		"@CommunicationModule",
#	] + select({
#        "@AvrToolchain//config:ElasticNode4": [],
#        "@AvrToolchain//config:ElasticNode3": [
#			"//lib:SoftSerialLib",
#			"//lib:SoftSpiLib"
#			]
#    }),
#    uploader = "//:stk_upload_script"
#)

#default_embedded_binary(
#    name = "demo",
#    srcs = [
#        "src/demoICAC.c",
#    ],
#    copts = commonCopts + LUFA_COPTS + ["-O2"],
#    linkopts = commonCopts + LUFA_COPTS + ["-Wl,-u,vfprintf -lprintf_flt -lm"],
#    uploader = "//:stk_upload_script",
#    deps = [
#        "//lib:CurrentSenseLib",
#        "//lib:DebugLib",
#    ],
#)
#
#default_embedded_binary(
#    name = "experiment",
#    srcs = [
#        "src/experiment.c",
#    ],
#    copts = commonCopts + LUFA_COPTS + ["-O2"],
#    linkopts = commonCopts + LUFA_COPTS + ["-Wl,-u,vfprintf -lprintf_flt -lm"],
#    uploader = "//:stk_upload_script",
#    deps = [
#        "//lib:PAC1720DriverLib",
#        ":AdapterLib",
#        "//lib:DebugLib",
#    ],
#)

default_embedded_binary(
	name = "CurrentSenseApp",
	srcs = [
		"src/currentSenseApp.c",
    ],

	copts = commonCopts + LUFA_COPTS,

	linkopts = commonCopts + LUFA_COPTS + ["-Wl,-u,vfprintf -lprintf_flt -lm"],
    uploader = "//:stk_upload_script",

	deps = [
        "AdapterLib",
        "//lib:I2cLib",
        "//lib:PAC1720DriverLib",
        "//lib:DelayLib",
        "//lib:DebugLib",
        "//lib:SubSystem"

	],
)

cc_library(
    name = "AdapterLib",
	srcs = ["src/adapter_PAC1720/adapter_PAC1720.c"],
    hdrs = ["src/adapter_PAC1720/adapter_PAC1720.h"],

    copts = commonCopts,

    deps = [
        "//lib:PAC1720DriverLib",
    ],

	visibility = [
		"//test:__pkg__",
		"//test:__subpackages__",
		"@CMock//CMock:__pkg__",
	],
)

# something like
# build --define PROGRAMMER_PORT=/dev/tty.usbmodem2301
# in your .bazelrc
# Uploading stuff
load("//:vars.bzl", "defineProgrammer", "programmer")
programmer()

STK_UPLOAD_SCRIPT_TEMPLATE = """
{sudo}avrdude -v -V -p $$1 -c stk500v2 -P $(PROGPORT) -U flash:w:$$2 -e;
"""

genrule(
    name = "stk_upload_script",
    outs = ["stk_upload_script.sh"],
    cmd = "echo '" +
          #    select({
          #        "@AvrToolchain//host_config:dfu_needs_sudo": UPLOAD_SCRIPT_TEMPLATE.format(
          #            export = "",
          #            sudo = "sudo ",
          #        ),
          #        "@AvrToolchain//host_config:dfu_needs_askpass": UPLOAD_SCRIPT_TEMPLATE.format(
          #            export = "export SUDO_ASKPASS=$(ASKPASS)",
          #            sudo = "sudo ",
          #        ),
          #        "//conditions:default":
          STK_UPLOAD_SCRIPT_TEMPLATE.format(
              sudo = "",
          ) +
          #    })
          "' > $@",
    toolchains = [":programmerPort"],
)

default_embedded_binary(
	name = "FindSensorsApp",
	srcs = [
		"src/findSensorsApp.c",
    ],

	copts = commonCopts + LUFA_COPTS,

	linkopts = commonCopts + LUFA_COPTS + ["-Wl,-u,vfprintf -lprintf_flt -lm"],
    uploader = "//:stk_upload_script",

	deps = [
        ":AdapterLib",
        "//lib:PAC1720DriverLib",
        "//lib:DebugLib",
        "//lib:I2cLib",
        "//lib:DelayLib"
	],
)
