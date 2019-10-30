// Development version of TCM. Loads shaders dynamically, and reloads them from
// disk as they change. Must be run from a checked out copy of the source code.

// To avoid conflict when we define these twice.
#define GLFW_INCLUDE_NONE

#include "dev/callback.hpp"
#include "dev/loader.hpp"
#include "dev/log.hpp"
#include "dev/shader.hpp"
#include "dev/text.hpp"
#include "tcm/demo.h"
#include "tcm/gl.h"
#include "tcm/shaders.h"

#include <GLFW/glfw3.h>

#include <cmath>

namespace tcm {

namespace {

void GLInit() {
#if !defined __APPLE__
    glewInit();
#endif
}

int Main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    ChdirWorkspaceRoot();

    if (!glfwInit()) {
        Die("Could not initialize GLFW");
    }

    // All of these are necessary.
    //
    // - On Apple devices, the context will be 2.1 if no hints are provided. If
    //   some of the hints are provided, context creation fails. Otherwise, the
    //   implementation will give the highest available OpenGL version, probably
    //   3.3 or 4.1 depending on hardware.
    //
    // - On Mesa, 3.0 is the maximum without FORWARD_COMPAT, and 3.1 is the
    //   maximum with FORWARD_COMPAT but without CORE_PROFILE.
    //
    // - With AMD or Nvidia drivers on Linux or Windows, you will always get the
    //   highest version supported (e.g. 4.6) even without any hints.
    //
    // Note that some implementations will give you exactly the version you ask
    // for and no more, but some will give you the highest version available.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(
        640, 360, "Terrestrial Collection Machine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        Die("Could not create window");
    }

    glfwMakeContextCurrent(window);
    fprintf(stderr, "GL_VERSION: %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));
    GLInit();
    TextInit();

    Shader triangle_vert(ShaderDir + "triangle.vert", GL_VERTEX_SHADER);
    Shader triangle_frag(ShaderDir + "triangle.frag", GL_FRAGMENT_SHADER);
    Program triangle_prog(&shader_triangle, "triangle",
                          {&triangle_vert, &triangle_frag});
    Shader line_vert(ShaderDir + "line.vert", GL_VERTEX_SHADER);
    Shader line_geom(ShaderDir + "line.geom", GL_GEOMETRY_SHADER);
    Shader line_frag(ShaderDir + "line.frag", GL_FRAGMENT_SHADER);
    Program line_prog(&shader_line, "line",
                      {&line_vert, &line_geom, &line_frag});
    demo_init();

    while (!glfwWindowShouldClose(window)) {
        InvokeCallbacks();

        double time = glfwGetTime();
        float color = std::fmod(time, 1.0);
        glClearColor(color, color, color, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        demo_draw(time);
        TextDraw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

} // namespace

} // namespace tcm

int main(int argc, char **argv) {
    return tcm::Main(argc, argv);
}
