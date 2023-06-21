#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vecmath.h>
#include "stb_image.h"

//实现纹理贴图和凹凸贴图

class Texture {
    public:
        Texture() {
            texture_image = nullptr;
            width = height = channel = 0;
        }

        Texture(const char* texture_file) {
            if (strcmp(texture_file, "") == 0){
                texture_image = nullptr;
            return;
            }
            texture_image = stbi_load(texture_file, &width, &height, &channel, 0);
            
            if (!texture_image) {
                fprintf(stderr, "cannot load texture file with name %s \n", texture_file);
            } else {
                printf("load texture file %s with size %d %d %d \n", texture_file, width, height, channel);
            }
        }

        Vector3f ColorAt(float u, float v) {
            if (texture_image == nullptr) return Vector3f::ZERO;

            int x = (int)(u * width) % width;
            int y = (int)(v * height) % height;

            int i = (y * width + x) * channel;

            //注意，texture_image是一个rgb（可能是rgba）图像，下标i表示红度，i+1表示绿度，i+2表示蓝度，所以需要除以255
            return Vector3f((float)(texture_image[i]), (float)(texture_image[i+1]), (float)(texture_image[i+2])) / 255.f;
        }

        bool hasTexture() {
            if (texture_image == nullptr) return false;
            return true;
        }


    protected:
        
        unsigned char* texture_image;
        int width, height;
        int channel; 
};

class BumpTexture: public Texture {
    public:
        BumpTexture(): Texture() {}
        BumpTexture(const char* texture_file): Texture(texture_file) {
        }
        
        //详见肖光烜学长的report，凹凸贴图“修改的不是物体表面的颜色而是法向量”
        Vector2f GradAt(float u, float v, float &value) {
            
            
            value = getValue(u, v);
            float du = 1.0f / width, dv = 1.0f / height;
            Vector2f grad(10*(getValue(u + du, v) - getValue(u - du, v)), 10*(getValue(u, v + dv) - getValue(u, v - dv)));
            return grad;
        }

        bool hasBump() {
            if (texture_image == nullptr) return false;
            return true;
        }

    protected:
        float getValue(float u, float v) {

            if (texture_image == nullptr) return 0.0;
            int x = (((int)(u * width + width)) % width + width) % width;
            int y = (((int)(v * height + height)) % height + height) % height;
            return float(texture_image[y * channel * width + x * channel] / 255.f);

        }
};
#endif