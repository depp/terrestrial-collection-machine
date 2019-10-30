// demo.c - Main demo entry point.
#include "tcm/demo.h"

#include "tcm/dragon.h"
#include "tcm/gl.h"

void demo_init(void) {
    dragon_init();
}

void demo_draw(double time) {
    glClear(GL_COLOR_BUFFER_BIT);
    dragon_draw(time);
}
