#pragma once

#include <string>
#include <GL/gl3w.h>

namespace tankwars {
    // The textures must have the same dimensions and have to be in 24bit rgb
    class SkyBox {
    public:
        SkyBox(
            const std::string& leftPath,
            const std::string& frontPath,
            const std::string& rightPath,
            const std::string& backPath,
            const std::string& topPath,
            const std::string& bottomPath,
            bool generateMipmaps = true,
            bool srgb = true);
        SkyBox(SkyBox&&) = default;
        ~SkyBox();

        void render() const;

    private:
        GLuint vbo;
        GLuint ibo;
        GLuint vao;
        GLuint cubemap;
    };
}