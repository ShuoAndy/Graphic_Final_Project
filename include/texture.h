#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vecmath.h>

class Texture {
    public:
        Texture() {
            texture_image = nullptr;
            width = height = channel = 0;
        }

        Texture(const char* texture_file);

        Vector3f ColorAt(float u, float v) {

            if (texture_image == nullptr) return Vector3f::ZERO;

            u = Clamp(u);
            v = 1.0 - Clamp(v);

            int x = static_cast<int>(u * width + width) % width;
            int y = static_cast<int>(v * height + height) % height;

            auto target_pixel = texture_image + y * channel * width + x * channel;
            return Vector3f(target_pixel[0], target_pixel[1], target_pixel[2]) / 255.f;

        }

        bool hasTexture() {
            if (texture_image == nullptr) return false;
            return true;
        }


    protected:
        float Clamp(float a) {
            a -= int(a);
            if (a < 0) a += 1;
            return a;
        }
        unsigned char* texture_image;
        int width, height;
        int channel; 
};

class BumpTexture: public Texture {
    public:
        BumpTexture(): Texture() {}
        BumpTexture(const char* texture_file): Texture(texture_file) {
        }
        
        Vector2f GradAt(float u, float v, float &value) {
            value = getValue(u, v);
            float du = 1.0f / width, dv = 1.0f / height;
            Vector2f grad(width * (getValue(u + du, v) - getValue(u - du, v)), height * (getValue(u, v + dv) - getValue(u, v - dv)));
            return grad;
        }

        bool hasBump() {
            if (texture_image == nullptr) return false;
            return true;
        }

    protected:
        float getValue(float u, float v) {

            if (texture_image == nullptr) return 0.0;
            int x = ((static_cast<int>(u * width + width)) % width + width) % width;
            int y = ((static_cast<int>(v * height + height)) % height + height) % height;
            return float(texture_image[y * channel * width + x * channel] / 255.f);

        }
};
#endif