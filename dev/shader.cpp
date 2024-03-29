#include "dev/shader.hpp"

#include "dev/log.hpp"

#include <algorithm>

namespace tcm {

const std::string ShaderDir{"tcm/shader/"};
const std::string DevShaderDir{"dev/shader/"};

Shader::Shader(std::string path, GLenum type)
    : status_{path},
      path_{std::move(path)},
      type_{type},
      shader_{0},
      ok_{false} {
    WatchFile(path_, [this](const DataBuffer &buf) { Load(buf); });
}

Shader::~Shader() {}

void Shader::Load(const DataBuffer &buf) {
    if (!buf) {
        status_.Set("No file.");
        SetFailed();
        return;
    }
    const std::vector<char> &data = *buf;
    if (shader_ == 0) {
        shader_ = glCreateShader(type_);
        if (shader_ == 0) {
            std::string text("glCreateShader: ");
            text.append(GLErrorName(glGetError()));
            status_.Set(std::move(text));
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
    std::string text;
    if (!status) {
        text.append("Compilation failed.\n");
    }
    GLint loglen;
    // loglen includes nul terminator.
    glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &loglen);
    if (loglen > 1) {
        std::vector<char> log(loglen);
        glGetShaderInfoLog(shader_, loglen, nullptr, log.data());
        if (text.empty()) {
            text.append("Compilation succeeded.\n");
        }
        text.append(log.data(), loglen - 1);
    }
    status_.Set(std::move(text));
    if (!status) {
        SetFailed();
        return;
    }
    ok_ = true;
    onchanged_.Call();
}

void Shader::SetFailed() {
    if (ok_) {
        ok_ = false;
        onchanged_.Call();
    }
}

Program::Program(GLuint *program, std::string name,
                 std::initializer_list<Shader *> shaders)
    : status_{name},
      program_ptr_{program},
      name_{std::move(name)},
      program_{0},
      ok_{false},
      shader_changed_{false} {
    if (shaders.size() > kMaxShaders) {
        Die("%s: Too many shaders: got %zu, maximum is %d", name_.c_str(),
            shaders.size(), kMaxShaders);
    }
    for (Shader *const shader : shaders) {
        shader->OnChanged([this]() { ShaderChanged(); });
    }
    auto it =
        std::copy(std::begin(shaders), std::end(shaders), std::begin(shaders_));
    std::fill(it, std::end(shaders_), nullptr);
}

Program::~Program() {}

void Program::ShaderChanged() {
    if (!shader_changed_) {
        shader_changed_ = true;
        Schedule([this]() { Update(); });
    }
}

void Program::Update() {
    if (!shader_changed_) {
        return;
    }
    shader_changed_ = false;
    GLuint shaders[kMaxShaders];
    for (int i = 0; i < kMaxShaders; i++) {
        Shader *sp = shaders_[i];
        if (sp != nullptr) {
            if (!sp->ok()) {
                status_.Clear();
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
            std::string text("glCreateProgram: ");
            text.append(GLErrorName(glGetError()));
            status_.Set(std::move(text));
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
    std::string text;
    if (!status) {
        text.append("Linking failed.\n");
    }
    GLint loglen;
    // loglen includes nul terminator.
    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &loglen);
    if (loglen > 1) {
        std::vector<char> log(loglen);
        glGetProgramInfoLog(program_, loglen, nullptr, log.data());
        if (text.empty()) {
            text.append("Linking succeeded.\n");
        }
        text.append(log.data(), loglen - 1);
    }
    status_.Set(std::move(text));
    if (!status) {
        SetFailed();
        return;
    }
    *program_ptr_ = program_;
    ok_ = true;
}

void Program::SetFailed() {
    *program_ptr_ = 0;
    ok_ = false;
}

} // namespace tcm
