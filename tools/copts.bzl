# This is how we set compiler options globally - we define them here, import
# this variable in every build file that has C code, and then set copts on every
# target manually. This is clumsy, but it's also very simple. There will
# probably be an easier way to do this in the future.
#
# See Bazel discussion: https://github.com/bazelbuild/bazel/issues/5198

COPTS_BASE = [
    # Ubuntu 18 LTS uses GCC 7.4, but c17 is not supported until GCC 8.
    "-std=c11",
    # "-D_DEFAULT_SOURCE",
]

_CWARN = [
    "-Wall",
    "-Wextra",
    "-Wpointer-arith",
    "-Wwrite-strings",
    "-Wmissing-prototypes",
    "-Wdouble-promotion",
    "-Werror=implicit-function-declaration",
    "-Winit-self",
    "-Wstrict-prototypes",
]

COPTS = (
    COPTS_BASE +
    select({
        "//tools:warnings_off": [],
        "//tools:warnings_on": _CWARN,
        "//tools:warnings_error": _CWARN + ["-Werror"],
    })
)

CXXOPTS_BASE = [
    "-std=c++17",
]

_CXXWARN = [
    "-Wall",
    "-Wextra",
]

CXXOPTS = (
    CXXOPTS_BASE +
    select({
        "//tools:warnings_off": [],
        "//tools:warnings_on": _CXXWARN,
        "//tools:warnings_error": _CXXWARN + ["-Werror"],
    })
)
