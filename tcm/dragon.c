// dragon.h - Draw dragon curve.
#include "tcm/dragon.h"

#include "tcm/gl.h"
#include "tcm/shaders.h"

#include <math.h>

GLuint arr;

void dragon_init(void) {
    glGenVertexArrays(1, &arr);
}

void dragon_draw(double time) {
    if (shader_line == 0) {
        return;
    }
    glUseProgram(shader_line);
    glBindVertexArray(arr);
    glUniform1f(glGetUniformLocation(shader_line, "a"), 0.5 * sin(time));
    const int N = 8;
    glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, (1 << N) + 3);
}
