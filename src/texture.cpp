#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdio>
#include <iostream>

Texture::Texture(const char* texture_file) {
    if (strcmp(texture_file, "") == 0){
        texture_image = nullptr;
        return;
    }
    texture_image = stbi_load(texture_file, &width, &height, &channel, 0);
    if (!texture_image) {
        fprintf(stderr, "cannot load texture file with name %s \n", texture_file);
    }
}