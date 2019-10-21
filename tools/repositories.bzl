load(":pkg_config.bzl", "pkg_config_repository")

def add_repositories():
    pkg_config_repository(
        name = "glfw3",
        spec = "glfw3",
        includes = ["GLFW"],
    )
