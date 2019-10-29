// dragon.h - Draw dragon curve.
#include "tcm/dragon.h"

#include "tcm/gl.h"
#include "tcm/shaders.h"

GLuint arr;

void dragon_init(void) {
    glGenVertexArrays(1, &arr);
}

void dragon_draw(void) {
    if (shader_line == 0) {
        return;
    }
    glUseProgram(shader_line);
    glBindVertexArray(arr);
    glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 18);
}
