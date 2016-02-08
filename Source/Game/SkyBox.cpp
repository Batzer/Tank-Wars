#include "SkyBox.h"

#include <cstdint>

#include <glm/glm.hpp>

#include "GLTools.h"
#include "Image.h"

namespace {
    const glm::vec3 cubeVertices[] = {
        { -1, -1, 1 },
        {  1, -1, 1 },
        {  1,  1, 1 },
        { -1,  1, 1 },
        { -1, -1, -1 },
        {  1, -1, -1 },
        {  1,  1, -1 },
        { -1,  1, -1 },
    };

    const std::uint16_t cubeIndices[] = {
        0, 1, 2, 0, 2, 3, // Front
        4, 0, 3, 4, 3, 7, // Left
        1, 5, 6, 1, 6, 2, // Right
        5, 4, 7, 5, 7, 6, // Back
        3, 2, 6, 3, 6, 7, // Top
        4, 5, 1, 4, 1, 0  // Bottom
    };
}

namespace tankwars {
    SkyBox::SkyBox(
        const std::string& leftPath,
        const std::string& frontPath,
        const std::string& rightPath,
        const std::string& backPath,
        const std::string& topPath,
        const std::string& bottomPath,
        bool generateMipmaps,
        bool srgb)
    {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 96, cubeVertices, GL_STATIC_DRAW);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 72, cubeIndices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
        glBindVertexArray(0); // Unbind so no unwanted information is recorded

        Image leftFace(leftPath);
        Image frontFace(frontPath);
        Image rightFace(rightPath);
        Image backFace(backPath);
        Image topFace(topPath);
        Image bottomFace(bottomPath);

        auto width = leftFace.getWidth();
        auto height = leftFace.getHeight();

        glGenTextures(1, &cubemap);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

        auto format = srgb ? GL_SRGB : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, leftFace.getImage());
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rightFace.getImage());
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bottomFace.getImage());
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, topFace.getImage());
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frontFace.getImage());
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, backFace.getImage());

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        if (generateMipmaps) {
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
    }

    SkyBox::~SkyBox() {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);
        glDeleteTextures(1, &cubemap);
    }

    void SkyBox::render() const {
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
    }
}