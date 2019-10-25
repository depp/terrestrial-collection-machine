#include "tcm/shader.hpp"

#include "tcm/log.hpp"

#include <algorithm>

namespace tcm {

namespace {

std::string ShaderPath(const char *name) {
    std::string path("tcm/shader/");
    path.append(name);
    return path;
}

} // namespace

Shader::Shader(const char *name, GLenum type)
    : path_{ShaderPath(name)},
      type_{type},
      shader_{0},
      ok_{false},
      changed_{false} {
    WatchFile(path_, [this](const DataBuffer &buf) { Load(buf); });
}

Shader::~Shader() {}

void Shader::ClearChanged() {
    changed_ = false;
}

void Shader::Load(const DataBuffer &buf) {
    if (!buf) {
        SetFailed();
        return;
    }
    const std::vector<char> &data = *buf;
    if (shader_ == 0) {
        shader_ = glCreateShader(type_);
        if (shader_ == 0) {
            ErrorGL(glGetError(), "%s: glCreateShader", path_.c_str());
            SetFailed();
            return;
        }
    }
    const char *srctext[1] = {data.data()};
    const GLint srclen[1] = {static_cast<GLint>(data.size())};
    glShaderSource(shader_, 1, srctext, srclen);
    glCompileShader(shader_);
    GLint status;
    glGetShaderiv(shader_, GL_COMPILE_STATUS, &status);
    if (!status) {
        Error("%s: compilation failed", path_.c_str());
    }
    GLint loglen;
    // loglen includes nul terminator.
    glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &loglen);
    if (loglen > 1) {
        std::vector<char> log(loglen);
        glGetShaderInfoLog(shader_, loglen, nullptr, log.data());
        log[loglen - 1] = '\0';
        fputs("-- Shader compilation log --\n", stderr);
        fputs(log.data(), stderr);
    }
    if (!status) {
        SetFailed();
        return;
    }
    ok_ = true;
    changed_ = true;
}

void Shader::SetFailed() {
    if (ok_) {
        ok_ = false;
        changed_ = true;
    }
}

Program::Program(const char *name, std::initializer_list<Shader *> shaders)
    : name_{name}, program_{0}, ok_{false} {
    if (shaders.size() > kMaxShaders) {
        Die("%s: Too many shaders: got %zu, maximum is %d", name_.c_str(),
            shaders.size(), kMaxShaders);
    }
    auto it =
        std::copy(std::begin(shaders), std::end(shaders), std::begin(shaders_));
    std::fill(it, std::end(shaders_), nullptr);
}

Program::~Program() {}

void Program::Update() {
    GLuint shaders[kMaxShaders];
    for (int i = 0; i < kMaxShaders; i++) {
        Shader *sp = shaders_[i];
        if (sp != nullptr) {
            if (!sp->ok()) {
                SetFailed();
                return;
            }
            shaders[i] = sp->get();
        } else {
            shaders[i] = 0;
        }
    }
    if (program_ == 0) {
        program_ = glCreateProgram();
        if (program_ == 0) {
            ErrorGL(glGetError(), "%s: glCreateShader", name_.c_str());
            SetFailed();
            return;
        }
        for (int i = 0; i < kMaxShaders; i++) {
            if (shaders[i] != 0) {
                glAttachShader(program_, shaders[i]);
            }
        }
    }
    glLinkProgram(program_);
    GLint status;
    glGetProgramiv(program_, GL_LINK_STATUS, &status);
    if (!status) {
        Error("%s: linking failed", name_.c_str());
    }
    GLint loglen;
    // loglen includes nul terminator.
    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &loglen);
    if (loglen > 1) {
        std::vector<char> log(loglen);
        glGetProgramInfoLog(program_, loglen, nullptr, log.data());
        log[loglen - 1] = '\0';
        fputs("-- Shader linking log --\n", stderr);
        fputs(log.data(), stderr);
    }
    if (!status) {
        SetFailed();
        return;
    }
    ok_ = true;
}

void Program::SetFailed() {
    ok_ = false;
}

} // namespace tcm