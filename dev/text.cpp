#include "dev/text.hpp"

#include "dev/loader.hpp"
#include "dev/log.hpp"
#include "dev/shader.hpp"
#include "tcm/gl.h"

#include <string>

#include <cassert>
#include <cstring>
#include <vector>

namespace tcm {

namespace {

const uint8_t MAGIC_1[] = {0x36, 0x04};
const uint8_t MAGIC_2[] = {0x72, 0xb5, 0x4a, 0x86};

enum {
    Mode512 = 0x01,
    ModeHasTab = 0x02,
};

enum {
    FlagHasTab = 0x01,
};

const uint32_t MinSize = 6;
const uint32_t MaxSize = 48;
const uint32_t MaxCount = 1024;
const uint32_t Columns = 16;

uint32_t Read(const char *ptr) {
    uint32_t x;
    std::memcpy(&x, ptr, 4);
    return x;
}

uint32_t RoundUpPow2(uint32_t x) {
    x -= 1;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

struct Metrics {
    uint32_t cwidth;
    uint32_t cheight;
    uint32_t iwidth;
    uint32_t iheight;
};

struct Font {
    std::vector<uint8_t> pixels;
    Metrics metrics;
};

void LoadFont(const std::string &path, Font *font) {
    std::vector<char> data;
    int r;
    r = ReadFile(path, &data);
    if (r != 0) {
        DieErrno(r, "Could not read %s", path.c_str());
    }
    if (data.size() < 4) {
        Die("Unknown font format: %s", path.c_str());
    }
    uint32_t count, headersize, cheight, cwidth;
    // bool has_table;
    if (std::memcmp(data.data(), MAGIC_1, 2) == 0) {
        uint8_t mode = data[2];
        uint8_t charsize = data[3];
        count = (mode & Mode512) != 0 ? 512 : 256;
        // has_table = (mode & ModeHasTab) != 0;
        headersize = 4;
        cheight = charsize;
        cwidth = 8;
    } else if (std::memcmp(data.data(), MAGIC_2, 4) == 0) {
        if (data.size() < 4 * 8) {
            Die("Invalid PSF2: %s", path.c_str());
        }
        const char *ptr = data.data();
        uint32_t version = Read(ptr + 4);
        if (version != 0) {
            Die("Unknown PSF2 version: %s", path.c_str());
        }
        headersize = Read(ptr + 8);
        // uint32_t flags = Read(ptr + 12);
        // has_table = (flags & FlagHasTab) != 0;
        count = Read(ptr + 16);
        cheight = Read(ptr + 24);
        cwidth = Read(ptr + 32);
        if (count > MaxCount) {
            Die("Too many glyphs in font: %s", path.c_str());
        }
    } else {
        Die("Unknown font format: %s", path.c_str());
    }
    if (cwidth > MaxSize || cheight > MaxSize) {
        Die("Font size %ux%u too large: %s", cwidth, cheight, path.c_str());
    }
    if (cwidth < MinSize || cheight < MinSize) {
        Die("Font size %ux%u too small: %s", cwidth, cheight, path.c_str());
    }
    uint32_t gsize = (cwidth + 7) / 8 * cheight * count;
    if (headersize > data.size() || gsize > data.size() - headersize) {
        Die("Invalid font %s", path.c_str());
    }
    uint32_t iwidth = RoundUpPow2(cwidth * Columns);
    uint32_t iheight = RoundUpPow2(cheight * ((count + Columns - 1) / Columns));
    std::vector<uint8_t> pixels(iwidth * iheight);
    uint8_t *pdata = pixels.data();
    const char *idata = data.data() + headersize;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t x0 = (i % Columns) * cwidth;
        uint32_t y0 = (i / Columns) * cheight;
        for (uint32_t y = 0; y < cheight; y++) {
            for (uint32_t xb = 0; xb < cwidth; xb += 8) {
                uint32_t d = static_cast<uint8_t>(*idata++);
                uint32_t nbits = cwidth - xb;
                if (nbits > 8) {
                    nbits = 8;
                }
                for (uint32_t bit = 0; bit < nbits; bit++) {
                    pdata[(y0 + y) * iwidth + x0 + xb + bit] =
                        (d & (0x80 >> bit)) != 0 ? 0xff : 0x00;
                }
            }
        }
    }
    *font = Font{std::move(pixels), Metrics{cwidth, cheight, iwidth, iheight}};
}

const char FONT_PATH[] = "font/ter-i16n.psf";

Shader *shader_vert;
Shader *shader_frag;
Program *program;

GLuint prog;
GLuint texture;
GLuint arr;
GLuint buf[2]; // quad, text
int icsize[2];
float csize[2];
float tsize[2];

} // namespace

void TextInit() {
    shader_vert = new Shader(DevShaderDir + "text.vert", GL_VERTEX_SHADER);
    shader_frag = new Shader(DevShaderDir + "text.frag", GL_FRAGMENT_SHADER);
    program = new Program(&prog, "text", {shader_vert, shader_frag});

    Font font;
    LoadFont(FONT_PATH, &font);
    const Metrics &m = font.metrics;
    icsize[0] = m.cwidth;
    icsize[1] = m.cheight;
    csize[0] = m.cwidth;
    csize[1] = m.cheight;
    tsize[0] = static_cast<float>(m.cwidth) / static_cast<float>(m.iwidth);
    tsize[1] = static_cast<float>(m.cheight) / static_cast<float>(m.iheight);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m.iwidth, m.iheight, 0, GL_RED,
                 GL_UNSIGNED_BYTE, font.pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &arr);
    glBindVertexArray(arr);
    glGenBuffers(2, buf);

    glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(int16_t) * 8,
                 (const int16_t[4][2]){
                     {0, 0},
                     {1, 0},
                     {0, 1},
                     {1, 1},
                 },
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, 8, 0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    glVertexAttribIPointer(2, 4, GL_UNSIGNED_BYTE, 8, (void *)(uintptr_t)4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

namespace {

std::vector<StatusItem *> status_items;
bool status_items_changed;

struct Vertex {
    int16_t x, y;
    uint8_t u, v, fg, bg;
};

struct Pos {
    int x;
    int y;
};

std::vector<Vertex> vertexes;

const Pos pos0 = {8, 8};

Pos PutText(Pos pos, const std::string &text) {
    Pos cur = pos;
    size_t i = 0;
    while (true) {
        size_t limit = i;
        int rem = (640 - 2 * pos0.x - cur.x) / icsize[0];
        if (rem > 0) {
            limit += rem;
        }
        if (limit > text.size()) {
            limit = text.size();
        }
        size_t end = i;
        while (end < limit && text[end] != '\n') {
            end++;
        }
        if (end < text.size() && text[end] != '\n') {
            size_t j = end;
            while (j > i && text[j - 1] != ' ') {
                j--;
            }
            if (j > i) {
                end = j;
            }
        }
        for (; i != end; i++) {
            uint8_t c = text[i];
            Vertex v;
            v.x = cur.x;
            v.y = cur.y;
            v.u = c % Columns;
            v.v = c / Columns;
            v.fg = 0;
            v.bg = 0;
            vertexes.push_back(v);
            cur.x += icsize[0];
        }
        if (i == text.size()) {
            break;
        }
        cur.x = pos0.x;
        cur.y += icsize[1];
        if (text[i] == '\n') {
            i++;
        } else {
            cur.x += icsize[0] * 2;
        }
    }
    return cur;
}

void UpdateText() {
    status_items_changed = false;
    vertexes.clear();
    Pos pos = pos0;
    for (const StatusItem *it : status_items) {
        const std::string &text = it->value();
        if (!text.empty()) {
            pos = PutText(pos, it->name());
            pos = PutText(pos, ": ");
            pos = PutText(pos, text);
            if (pos.x != pos0.x) {
                pos.x = pos0.x;
                pos.y += icsize[1];
            }
            pos.y += 4;
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(Vertex),
                 vertexes.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace

StatusItem::StatusItem(std::string name) : name_{std::move(name)} {
    status_items.push_back(this);
}

StatusItem::~StatusItem() {
    auto it = std::find(std::begin(status_items), std::end(status_items), this);
    if (it != std::end(status_items)) {
        status_items.erase(it);
    }
}

void StatusItem::Clear() {
    if (!text_.empty()) {
        status_items_changed = true;
        text_.clear();
    }
}

void StatusItem::Set(std::string s) {
    if (text_ != s) {
        status_items_changed = true;
        text_ = std::move(s);
    }
}

void TextDraw() {
    if (prog == 0) {
        return;
    }
    if (status_items_changed) {
        UpdateText();
    }
    if (vertexes.empty()) {
        return;
    }
    glUseProgram(prog);
    glBindVertexArray(arr);
    glUniform2f(glGetUniformLocation(prog, "scale"), 2.0 / 640.0, -2.0 / 360.0);
    glUniform2fv(glGetUniformLocation(prog, "csize"), 1, csize);
    glUniform2fv(glGetUniformLocation(prog, "tsize"), 1, tsize);
    glUniform1i(glGetUniformLocation(prog, "texture"), 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, vertexes.size());

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

} // namespace tcm
