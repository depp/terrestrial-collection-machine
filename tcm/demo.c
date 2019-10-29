// demo.c - Main demo entry point.
#include "tcm/demo.h"

#include "tcm/triangle.h"
#include "tcm/dragon.h"

void demo_init(void) {
    triangle_init();
    dragon_init();
}

void demo_draw(void) {
    triangle_draw();
    dragon_draw();
}
