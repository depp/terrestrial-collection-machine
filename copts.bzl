# This is how we set compiler options globally - we define them here, import
# this variable in every build file that has C code, and then set copts on every
# target manually. This is clumsy, but it's also very simple. There will
# probably be an easier way to do this in the future.
#
# See Bazel discussion: https://github.com/bazelbuild/bazel/issues/5198
COPTS = [
    "-std=c17",
    # "-D_DEFAULT_SOURCE",
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
