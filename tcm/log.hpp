#pragma once

namespace tcm {

// Return the name of the given error code returned from glError.
const char *GLErrorName(int glerror);

// Print an error message and exit the program.
void Die(const char *fmt, ...) __attribute__((noreturn, format(printf, 1, 2)));

// Print an error message with an errno error code and exit the program.
void DieErrno(int ecode, const char *fmt, ...)
    __attribute__((noreturn, format(printf, 2, 3)));

// Print an error message.
void Error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

// Print an error message with an errno error code.
void ErrorErrno(int ecode, const char *fmt, ...)
    __attribute((format(printf, 2, 3)));

// Print an error message with an OpenGL error code from glGetError.
void ErrorGL(int glerror, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

} // namespace tcm
