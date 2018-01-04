#ifndef RENDER_TEXTURE_H_INCLUDED
#define RENDER_TEXTURE_H_INCLUDED

#include <string>

namespace render {

class Texture {
public:
    // creation
    Texture() : m_id { 0 } {}
    Texture(std::string file) { load(std::move(file)); }
    void load(std::string file);

    // deletion
    ~Texture() { release(); }
    void release();

    // only moving
    Texture(Texture&& other) : m_id { other.m_id } { other.m_id = 0; }
    Texture& operator=(Texture&& other) { std::swap(m_id, other.m_id); return *this; }

    // get the GL id directly
    unsigned get() const { return m_id; }
    operator unsigned() const { return m_id; }

    // use it
    void use(int texUnit = -1) const;

private:
    unsigned m_id;
};

}

#endif
