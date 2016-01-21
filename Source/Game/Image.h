#pragma once

#include <cstddef>
#include <string>

namespace tankwars {
    class Image {
    public:
        Image(const std::string& path);
        Image(const void* data, size_t size);
        ~Image();

        int getWidth() const;
        int getHeight() const;
        int getNumChannels() const;
        unsigned char* getImage() const;

    private:
        int width;
        int height;
        int numChannels;
        unsigned char* image;
    };

}