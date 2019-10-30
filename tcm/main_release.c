// To avoid conflict when we define these twice.
#define GLFW_INCLUDE_NONE

#include "tcm/demo.h"
#include "tcm/gl.h"
#include "tcm/packed_shaders.h"
#include "tcm/shaders.h"

#include <OpenGL/gl3.h>

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

static void die(const char *msg) __attribute__((noreturn));

static void die(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

// Load a single shader, and return the shader object.
static GLuint load_shader(GLenum type, const char *source, size_t sourcelen) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        die("Could not create shader");
    }
    glShaderSource(shader, 1, (const char *[]){source},
                   (const GLint[]){sourcelen});
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        fputs("Error: Could not compile shader.\n", stderr);
    }
    GLint loglen;
    // loglen includes nul terminator.
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
    if (loglen > 1) {
        char *log = malloc(loglen);
        if (log == NULL) {
            die("No memory");
        }
        glGetShaderInfoLog(shader, loglen, NULL, log);
        log[loglen - 1] = '\0';
        fputs("-- Shader compilation log --\n", stderr);
        fputs(log, stderr);
        free(log);
    }
    if (!status) {
        die("Shader compilation failed.");
    }
    return shader;
}

// Link an OpenGL shader program.
static GLuint link_program(const GLuint *restrict shaders) {
    GLuint prog = glCreateProgram();
    if (prog == 0) {
        die("Could not create program");
    }
    for (int i = 0; shaders[i] != 0; i++) {
        glAttachShader(prog, shaders[i]);
    }
    glLinkProgram(prog);
    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (!status) {
        fputs("Error: Could not link program.\n", stderr);
    }
    GLint loglen;
    // loglen includes nul terminator.
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &loglen);
    if (loglen > 1) {
        char *log = malloc(loglen);
        if (log == NULL) {
            die("No memory");
        }
        glGetProgramInfoLog(prog, loglen, NULL, log);
        log[loglen - 1] = '\0';
        fputs("-- Shader linking log --\n", stderr);
        fputs(log, stderr);
        free(log);
    }
    if (!status) {
        die("Shader linking failed.");
    }
    return prog;
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
        640, 360, "Terrestrial Collection Machine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        die("Could not create window");
    }

    glfwMakeContextCurrent(window);
    fprintf(stderr, "GL_VERSION: %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));

    shader_triangle = link_program((const GLuint[]){
        load_shader(GL_VERTEX_SHADER, TRIANGLE_VERT, sizeof(TRIANGLE_VERT)),
        load_shader(GL_FRAGMENT_SHADER, TRIANGLE_FRAG, sizeof(TRIANGLE_FRAG)),
        0,
    });
    shader_line = link_program((const GLuint[]){
        load_shader(GL_VERTEX_SHADER, LINE_VERT, sizeof(LINE_VERT)),
        load_shader(GL_GEOMETRY_SHADER, LINE_GEOM, sizeof(LINE_GEOM)),
        load_shader(GL_FRAGMENT_SHADER, LINE_FRAG, sizeof(LINE_FRAG)),
        0,
    });
    demo_init();

    while (!glfwWindowShouldClose(window)) {
        double time = glfwGetTime();

        demo_draw(time);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
