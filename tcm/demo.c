// demo.c - Main demo entry point.
#include "tcm/demo.h"

#include "tcm/dragon.h"
#include "tcm/triangle.h"

void demo_init(void) {
    triangle_init();
    dragon_init();
}

void demo_draw(double time) {
    triangle_draw();
    dragon_draw(time);
}
