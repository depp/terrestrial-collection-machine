load(":pkg_config.bzl", "pkg_config_repository")

def add_repositories():
    pkg_config_repository(
        name = "glfw3",
        spec = "glfw3",
        includes = [
            "GLFW/*.h",
        ],
    )
    pkg_config_repository(
        name = "glew",
        spec = "glew",
        includes = [
            "GL/glew.h",
        ],
    )
    pkg_config_repository(
        name = "libpng",
        spec = "libpng",
        includes = [
            "png.h",
            "pngconf.h",
            "pnglibconf.h",
        ],
    )
