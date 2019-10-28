// gl.h - OpenGL definitions.
#pragma once

#if defined __APPLE__

// OpenGL is deprecated on macOS. We don't care. This silences the warnings.
#define GL_SILENCE_DEPRECATION 1

#include <OpenGL/gl3.h>

#else

#include <GL/glew.h>

#endif
