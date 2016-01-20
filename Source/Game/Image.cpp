#include "Image.h"

#include <stb_image.h>
#include <vector>
#include <fstream>

namespace tankwars {
    Image::Image(const std::string& path) {
        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file.is_open()) {
            throw std::runtime_error("Attempt to open non-existent image file");
        }

        file.seekg(0, std::ios::end);
        auto size = static_cast<std::size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        file.read(buffer.data(), size);

        image = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buffer.data()), size,
            &width, &height, &numChannels, STBI_default);
    }

    Image::Image(const void* data, size_t size) {
        image = stbi_load_from_memory(static_cast<const stbi_uc*>(data), size,
            &width, &height, &numChannels, STBI_default);
    }

    Image::~Image() {
        if (image) {
            stbi_image_free(image);
        }
    }

    int Image::getWidth() const {
        return width;
    }

    int Image::getHeight() const {
        return height;
    }

    int Image::getNumChannels() const {
        return numChannels;
    }

    unsigned char* Image::getImage() const {
        return image;
    }
}