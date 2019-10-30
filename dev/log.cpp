#include "dev/log.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace tcm {

namespace {

struct GLErrorCode {
    int value;
    const char *name;
};

const struct GLErrorCode GLERROR_CODES[] = {
    {0, "NO_ERROR"},
    {0x0500, "INVALID_ENUM"},
    {0x0501, "INVALID_VALUE"},
    {0x0502, "INVALID_OPERATION"},
    {0x0503, "STACK_OVERFLOW"},
    {0x0504, "STACK_UNDERFLOW"},
    {0x0505, "OUT_OF_MEMORY"},
    {0x0506, "INVALID_FRAMEBUFFER_OPERATION"},
};

} // namespace

const char *GLErrorName(int glerror) {
    for (const auto &code : GLERROR_CODES) {
        if (code.value == glerror) {
            return code.name;
        }
    }
    return nullptr;
}

static void VError(const char *fmt, va_list ap) {
    fputs("Error: ", stderr);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
}

static void VErrorErrno(int ecode, const char *fmt, va_list ap) {
    fputs("Error: ", stderr);
    vfprintf(stderr, fmt, ap);
    fputs(": ", stderr);
    fputs(strerror(ecode), stderr);
    fputc('\n', stderr);
}

static void VErrorGL(int glerror, const char *fmt, va_list ap) {
    fputs("Error: ", stderr);
    vfprintf(stderr, fmt, ap);
    fputs(": ", stderr);
    const char *name = GLErrorName(glerror);
    if (name == nullptr) {
        fprintf(stderr, "unknown error (0x%04x)", glerror);
    } else {
        fputs(name, stderr);
    }
    fputc('\n', stderr);
}

void Die(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    VError(fmt, ap);
    va_end(ap);
    exit(1);
}

void DieErrno(int ecode, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    VErrorErrno(ecode, fmt, ap);
    va_end(ap);
    exit(1);
}

void Error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    VError(fmt, ap);
    va_end(ap);
}

void ErrorErrno(int ecode, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    VErrorErrno(ecode, fmt, ap);
    va_end(ap);
}

void ErrorGL(int glerror, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    VErrorGL(glerror, fmt, ap);
    va_end(ap);
}

} // namespace tcm
