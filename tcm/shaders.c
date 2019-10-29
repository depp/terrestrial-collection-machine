// shaders.c - Shader programs used in the demo.
#include "tcm/shaders.h"

// Note... we have to explicitly set these to 0. If we leave them uninitialized,
// they will be common symbols, and this will result in a link error because
// some linkers don't consider common symbols to be definitions. This is only a
// problem because this file gets put in a static library.

GLuint shader_triangle = 0;

GLuint shader_line = 0;
