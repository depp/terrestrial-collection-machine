// dragon.h - Draw dragon curve.
#include "tcm/dragon.h"

#include "tcm/gl.h"
#include "tcm/shaders.h"

static GLuint arr;
static GLuint buf;

void dragon_init(void) {
    glGenVertexArrays(1, &arr);
    glBindVertexArray(arr);
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 7,
                 (const float[7][2]){
                     {-1.0f, +2.0f},
                     {-1.0f, +1.0f},
                     {-1.0f, +0.0f},
                     {+0.0f, -1.0f},
                     {+1.0f, +0.0f},
                     {+1.0f, +1.0f},
                     {+1.0f, +2.0f},
                 },
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void dragon_draw(void) {
    if (shader_line == 0) {
        return;
    }
    glUseProgram(shader_line);
    glBindVertexArray(arr);
    glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 7);
}
