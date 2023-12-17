#pragma once

#include <memory>

struct TexBufferInfo {
    int width;
    int channels;
    std::shared_ptr<unsigned char> data;
    int stride;
};