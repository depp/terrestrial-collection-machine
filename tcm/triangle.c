// triangle.h - Draw triangle on screen.
#include "tcm/triangle.h"

#include "tcm/gl.h"
#include "tcm/shaders.h"

static GLuint arr;
static GLuint buf;

void triangle_init(void) {
    glGenVertexArrays(1, &arr);
    glBindVertexArray(arr);
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6,
                 (const float[][2]){
                     {-1.0f, -1.0f},
                     {+1.0f, -1.0f},
                     {+0.0f, +1.0f},
                 },
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void triangle_draw(void) {
    if (shader_triangle == 0) {
        return;
    }
    glUseProgram(shader_triangle);
    glBindVertexArray(arr);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
