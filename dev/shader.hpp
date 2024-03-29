// shader.hpp - Load OpenGL shaders.
#pragma once

#include "dev/callback.hpp"
#include "dev/loader.hpp"
#include "dev/text.hpp"
#include "tcm/gl.h"

#include <initializer_list>
#include <string>

namespace tcm {

extern const std::string ShaderDir;
extern const std::string DevShaderDir;

// An OpenGL shader.
class Shader {
public:
    Shader(std::string path, GLenum type);
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
    ~Shader();

    // Get the OpenGL shader.
    GLuint get() const { return shader_; }

    // Return true if the shader is loaded and compiled.
    bool ok() const { return ok_; }

    // Call a function when the shader changes.
    void OnChanged(Callback cb) { onchanged_.Add(std::move(cb)); }

private:
    // Load the shader from a file.
    void Load(const DataBuffer &buf);

    // Mark the shader as having failed to load or compile.
    void SetFailed();

    StatusItem status_;
    const std::string path_;
    const GLenum type_;
    GLuint shader_;
    bool ok_;
    CallbackList onchanged_;
};

// An OpenGL shader program.
class Program {
public:
    Program(GLuint *program, std::string name,
            std::initializer_list<Shader *> shaders);
    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;
    ~Program();

    // Return true if the program is loaded and linked.
    bool ok() const { return ok_; }

private:
    static const int kMaxShaders = 3;

    // Respond to an input shader changing.
    void ShaderChanged();

    // Update the shader program after the shaders have potentially changed.
    void Update();

    // Mark the program as having failed to link or having failed shaders.
    void SetFailed();

    StatusItem status_;
    GLuint *program_ptr_;
    const std::string name_;
    Shader *shaders_[kMaxShaders];
    GLuint program_;
    bool ok_;
    bool shader_changed_;
};

} // namespace tcm
