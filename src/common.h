#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef bool b32;

typedef float f32;
typedef double f64;

typedef unsigned char ubyte;

template<typename T>
using ref = std::shared_ptr<T>;

static f32 pixel2screen = 3.2f/455.0f;