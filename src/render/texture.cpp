#include "texture.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

namespace render {

void Texture::load(std::string file) {
    release();

    int width;
    int height;
    int channels;
    unsigned char* data = stbi_load(file.c_str(), &width, &height, &channels, 0);
    if (!data) {
        throw std::runtime_error("Could not load texture from \"" + file + "\"");
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int mode = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0,
                 mode, GL_UNSIGNED_BYTE, data);

    if (GLAD_GL_EXT_texture_filter_anisotropic) {
        float size;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &size);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, size);

        std::cout << "Anisotropic filtering: " << size << "\n";
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture::release() {
    if (m_id) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

void Texture::use(int texUnit /* = -1 */) const {
    if (texUnit >= 0)
        glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

}
