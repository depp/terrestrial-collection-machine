// OpenGL is deprecated on macOS. We don't care. This silences the warnings.
#define GL_SILENCE_DEPRECATION 1

// To avoid conflict when we define these twice.
#define GLFW_INCLUDE_NONE

#include <OpenGL/gl3.h>

#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void die(const char *msg) __attribute__((noreturn));

static void die(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

static const char VERT_SHADER[] =
    "#version 330\n"
    "layout(location = 0) in vec2 in_pos;\n"
    "out VertexData {\n"
    "    vec3 color;\n"
    "} o;\n"
    "void main() {\n"
    "    o.color = vec3(in_pos.xy, 0.0);\n"
    "    gl_Position = vec4(in_pos, 0.0, 1.0);\n"
    "}\n";

static const char FRAG_SHADER[] =
    "#version 330\n"
    "in VertexData {\n"
    "    vec3 color;\n"
    "} i;\n"
    "out vec4 out_color;\n"
    "void main() {\n"
    "    out_color = vec4(i.color, 0.0);\n"
    "}\n";

// Load a single shader, and return the shader object.
static GLuint load_shader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        die("Could not create shader");
    }
    glShaderSource(shader, 1, (const char *[]){source},
                   (const GLint[]){strlen(source)});
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

// Load the shader program and return the program object.
static GLuint load_shaders(void) {
    GLuint vert = load_shader(GL_VERTEX_SHADER, VERT_SHADER);
    GLuint frag = load_shader(GL_FRAGMENT_SHADER, FRAG_SHADER);
    GLuint prog = glCreateProgram();
    if (prog == 0) {
        die("Could not create program");
    }
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
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
        640, 480, "Terrestrial Collection Machine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        die("Could not create window");
    }

    glfwMakeContextCurrent(window);
    fprintf(stderr, "GL_VERSION: %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));

    GLuint prog = load_shaders();
    GLuint arr;
    GLuint buf;

    glGenVertexArrays(1, &arr);
    glBindVertexArray(arr);
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6,
                 (const float[][2]){
                     {-0.6f, -0.8f},
                     {0.6f, -0.8f},
                     {0.0f, 0.8f},
                 },
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        double time = glfwGetTime();
        float color = fmod(time, 1.0);
        glClearColor(color, color, color, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
