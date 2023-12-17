#pragma once
#include <limits>
#include <vector>
#include <random>
#include <thread>
#include <memory>

#include "Noise.hpp"

struct Vec2 {
    int x, y;           // integer coordinates
    float x_f, y_f;     // pixel center coordinates
    Vec2() {}
    Vec2(int x, int y) : x(x), y(y), x_f(x + 0.5f), y_f(y + 0.5f) {}
    Vec2(float x_f, float y_f) : x_f(x_f), y_f(y_f), x(x_f - 0.5f), y(y_f - 0.5f) {}
};

static int randInt(int max, int min = 0) {
    static std::mt19937 engine(std::random_device{}());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(engine);
}

static float distanceSquared(Vec2& a, Vec2& b) {
    float dx = a.x_f - b.x_f;
    float dy = a.y_f - b.y_f;
    return dx * dx + dy * dy;
}

static float linearFactorSquared(float t, float radius) {
    return 1.0f - sqrt(t) / (radius * 1.2f);
}

static void shadePixel(unsigned char* data, int index, int r = 0, int g = 0, int b = 0) {
    data[index + 0] = r;
    data[index + 1] = g;
    data[index + 2] = b;
    data[index + 3] = 255;
}

inline TexBufferInfo createWorleyNoiseTexture(unsigned int blocksize, unsigned int count) {
    int width = blocksize * count;
    std::shared_ptr<unsigned char> data(new unsigned char[width * width * 4]);
    count += 2;

    std::vector<Vec2> points;

    for (int j = 0; j < count; j++) {
        for (int i = 0; i < count; i++) {
            points.emplace_back(int(randInt(blocksize) + i * blocksize), int(randInt(blocksize) + j * blocksize));
        }
    }

    for (int j = 1; j < count - 1; j++) {
        for (int i = 1; i < count - 1; i++) {
            Vec2& center_point = points[j * count + i];
            std::vector<Vec2*> points_around;
            for (int y = -1; y <= 1; y++) {
                for (int x = -1; x <= 1; x++) {
                    if (x != 0 || y != 0) {
                        points_around.push_back(&points[(j + y) * count + (i + x)]);
                    }
                }
            }
            
            for (int y = 0; y < blocksize; y++) {
                for (int x = 0; x < blocksize; x++) {
                    Vec2 pixel(int(x + blocksize * i), int(y + blocksize * j));
                    float dist_1 = distanceSquared(pixel, center_point);

                    // whatever a big number
                    float dist_2 = std::numeric_limits<float>::max();
                    for (int k = 0; k < points_around.size(); k++) {
                        float dist = distanceSquared(pixel, *points_around[k]);
                        if (dist < dist_2) {
                            dist_2 = dist;
                        }
                    }

                    int pixel_index = (((j-1) * blocksize + y) * width + (i-1) * blocksize + x) * 4;
                    float t = linearFactorSquared(dist_1 < dist_2 ? dist_1 : dist_2, blocksize);
                    shadePixel(data.get(), pixel_index, 255 * t, 255 * t, 255 * t);
                }
            }
        }
    }
    return TexBufferInfo{ width, 4, data, width * 4 };
}