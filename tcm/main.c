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

    GLFWwindow *window = glfwCreateWindow(
        640, 480, "Terrestrial Collection Machine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        die("Could not create window");
    }

    glfwMakeContextCurrent(window);
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
