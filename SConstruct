#!/usr/bin/env python
import os
import sys

from methods import print_error


libname = "OpusGdextension"
projectdir = "project"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"api_version": 4.4, "env": env, "customs": customs})

# Add WIN32 define on Windows (required by config.h)
if env["platform"] == "windows":
    env.Append(CPPDEFINES=["WIN32"])

env.Append(CPPDEFINES=[
    "HAVE_CONFIG_H",
])

env.Append(CPPPATH=[
    "src/",
    "#thirdparty/libogg/",
    "#thirdparty/libogg/ogg/",
    "#thirdparty/libopus/",
    "#thirdparty/libopus/opus/",
    "#thirdparty/libopus/src/",
    "#thirdparty/libopus/celt/",
    "#thirdparty/libopus/silk/",
    "#thirdparty/libopus/silk/fixed/",
    "#thirdparty/libopus/silk/float/",
    "#thirdparty/libopusfile/",
    "#thirdparty/libopusfile/opus/",
    "#thirdparty/libopusfile/src/",

    "#thirdparty/libopus/celt/x86/",
    "#thirdparty/libopus/silk/x86/",
    "#thirdparty/libopus/silk/fixed/x86/",
    "#thirdparty/libopus/silk/float/x86/",

    "#thirdparty/libopus/celt/arm/",
    "#thirdparty/libopus/silk/arm/",
    "#thirdparty/libopus/silk/fixed/arm/",
])

sources = Glob("src/*.cpp") \
    + Glob("#thirdparty/libogg/*.c") \
    + Glob("#thirdparty/libogg/ogg/*.c") \
    + Glob("#thirdparty/libopus/src/*.c") \
    + Glob("#thirdparty/libopus/celt/*.c") \
    + Glob("#thirdparty/libopus/silk/*.c") \
    + Glob("#thirdparty/libopus/silk/float/*.c") \
    + Glob("#thirdparty/libopusfile/src/*.c")

# Remove /fp:strict flag
if env["CCFLAGS"]:
    if "/fp:strict" in env["CCFLAGS"]:
        env["CCFLAGS"].remove("/fp:strict")
        # Add /fp:precise flag
        env.Append(CCFLAGS=["/fp:precise"])

# Add instruction sets for architecture-specific optimizations (see https://github.com/godotengine/godot/blob/master/SConstruct#L798)
# These are added automatically by Godot, but since this is a GDExtension, we need to add them manually
if env["arch"] == "x86_64":
    if env.get("is_msvc", False):
        env.Append(CCFLAGS=["/d2archSSE42"])
    else:
        env.Append(CCFLAGS=["-msse4.2", "-mpopcnt"])
elif env["arch"] == "x86_32":
    if env.get("is_msvc", False):
        env.Append(CCFLAGS=["/arch:SSE2"])
    else:
        env.Append(CCFLAGS=["-msse2", "-mfpmath=sse", "-mstackrealign"])

# Add architecture-specific optimizations (see https://github.com/godotengine/godot-cpp/blob/master/tools/godotcpp.py#L182)
if env["platform"] == "windows" and env["arch"] in ["arm32", "arm64"]:
    # Skip optimizations on Windows ARM64, because `arm_neon.h` is not provided on setup-mingw (https://github.com/egor-tensin/setup-mingw)
    pass
elif env["arch"] == "x86_32":
    env.Append(CPPDEFINES=["OPUS_X32_OPT"])
    sources += [
        #"#thirdparty/libopus/celt/x86/celt_lpc_sse4_1.c", # SSE4.1 is disabled for x32
        #"#thirdparty/libopus/celt/x86/pitch_avx.c", # AVX2 is disabled for x32
        "#thirdparty/libopus/celt/x86/pitch_sse.c",
        "#thirdparty/libopus/celt/x86/pitch_sse2.c",
        #"#thirdparty/libopus/celt/x86/pitch_sse4_1.c", # SSE4.1 is disabled for x32
        "#thirdparty/libopus/celt/x86/vq_sse2.c",
        "#thirdparty/libopus/celt/x86/x86_celt_map.c",
        "#thirdparty/libopus/celt/x86/x86cpu.c",

        #"#thirdparty/libopus/silk/x86/NSQ_del_dec_avx2.c", # AVX2 is disabled for x32
        #"#thirdparty/libopus/silk/x86/NSQ_del_dec_sse4_1.c", # SSE4.1 is disabled for x32
        #"#thirdparty/libopus/silk/x86/NSQ_sse4_1.c", # SSE4.1 is disabled for x32
        #"#thirdparty/libopus/silk/x86/VAD_sse4_1.c", # SSE4.1 is disabled for x32
        #"#thirdparty/libopus/silk/x86/VQ_WMat_EC_sse4_1.c", # SSE4.1 is disabled for x32
        "#thirdparty/libopus/silk/x86/x86_silk_map.c",

        #"#thirdparty/libopus/silk/float/x86/inner_product_FLP_avx2.c", # AVX2 is disabled for x32
    ]
elif env["arch"] == "x86_64":
    env.Append(CPPDEFINES=["OPUS_X64_OPT"])
    sources += [
        "#thirdparty/libopus/celt/x86/celt_lpc_sse4_1.c",
        #"#thirdparty/libopus/celt/x86/pitch_avx.c", # AVX2 is disabled for x64
        "#thirdparty/libopus/celt/x86/pitch_sse.c",
        "#thirdparty/libopus/celt/x86/pitch_sse2.c",
        "#thirdparty/libopus/celt/x86/pitch_sse4_1.c",
        "#thirdparty/libopus/celt/x86/vq_sse2.c",
        "#thirdparty/libopus/celt/x86/x86_celt_map.c",
        "#thirdparty/libopus/celt/x86/x86cpu.c",

        #"#thirdparty/libopus/silk/x86/NSQ_del_dec_avx2.c", # AVX2 is disabled for x64
        "#thirdparty/libopus/silk/x86/NSQ_del_dec_sse4_1.c",
        "#thirdparty/libopus/silk/x86/NSQ_sse4_1.c",
        "#thirdparty/libopus/silk/x86/VAD_sse4_1.c",
        "#thirdparty/libopus/silk/x86/VQ_WMat_EC_sse4_1.c",
        "#thirdparty/libopus/silk/x86/x86_silk_map.c",

        #"#thirdparty/libopus/silk/float/x86/inner_product_FLP_avx2.c", # AVX2 is disabled for x64
    ]
elif env["arch"] == "arm32":
    env.Append(CPPDEFINES=["OPUS_ARM32_OPT"])
    #sources += []
elif env["arch"] == "arm64":
    env.Append(CPPDEFINES=["OPUS_ARM64_OPT"])
    sources += [
        "#thirdparty/libopus/celt/arm/arm_celt_map.c",
        "#thirdparty/libopus/celt/arm/armcpu.c",
        "#thirdparty/libopus/celt/arm/celt_neon_intr.c",
        "#thirdparty/libopus/celt/arm/pitch_neon_intr.c",

        "#thirdparty/libopus/silk/arm/arm_silk_map.c",
        "#thirdparty/libopus/silk/arm/biquad_alt_neon_intr.c",
        "#thirdparty/libopus/silk/arm/LPC_inv_pred_gain_neon_intr.c",
        "#thirdparty/libopus/silk/arm/NSQ_del_dec_neon_intr.c",
        "#thirdparty/libopus/silk/arm/NSQ_neon.c",
    ]

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("src/doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/addons/OpusGdextension/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
