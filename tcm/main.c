// OpenGL is deprecated on macOS. We don't care. This silences the warnings.
#define GL_SILENCE_DEPRECATION 1

#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void die(const char *msg) __attribute__((noreturn));

static void die(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (!glfwInit()) {
        die("Could not initialize GLFW");
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
        640, 480, "Terrestrial Collection Machine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        die("Could not create window");
    }

    glfwMakeContextCurrent(window);
    fprintf(stderr, "GL_VERSION: %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));

    while (!glfwWindowShouldClose(window)) {
        double time = glfwGetTime();
        float color = fmod(time, 1.0);
        glClearColor(color, color, color, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
