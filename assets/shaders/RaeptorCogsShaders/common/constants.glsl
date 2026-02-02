// Constants
#if defined(__cplusplus)
    #pragma once
    #define RGSL_CONST constexpr
    #define RGSL_DOUBLE double
#else
    #define RGSL_CONST const
    #define RGSL_DOUBLE float
#endif

RGSL_CONST RGSL_DOUBLE PI = 3.14159265358979323846;
RGSL_CONST RGSL_DOUBLE EPSILON = 1e-6;

RGSL_CONST int IDATATEX_WIDTH = 2048;

#if defined(__cplusplus)
enum class RendererMode : int {
    RENDERER_MODE_DEFAULT = 0,
    RENDERER_MODE_2D_SPRITE = 1,
    RENDERER_MODE_2D_TEXT = 2
};

enum class BlendMode : int {
    BLEND_MODE_NONE = 0,
    BLEND_MODE_ALPHA = 1,
    BLEND_MODE_ADD = 2,
    BLEND_MODE_MULTIPLY = 3
};

enum class MaskMode : int {
    MASK_MODE_EQUAL = 0,
    MASK_MODE_LESS_THAN = 1,
    MASK_MODE_LESS_EQUAL = 2,
    MASK_MODE_GREATER_THAN = 3,
    MASK_MODE_GREATER_EQUAL = 4,
    MASK_MODE_NOT_EQUAL = 5
};
#else
#define RENDERER_MODE_DEFAULT 0
#define RENDERER_MODE_2D_SPRITE 1
#define RENDERER_MODE_2D_TEXT 2

#define BLEND_MODE_NONE 0
#define BLEND_MODE_ALPHA 1
#define BLEND_MODE_ADD 2
#define BLEND_MODE_MULTIPLY 3

#define MASK_MODE_EQUAL 0
#define MASK_MODE_LESS_THAN 1
#define MASK_MODE_LESS_EQUAL 2
#define MASK_MODE_GREATER_THAN 3
#define MASK_MODE_GREATER_EQUAL 4
#define MASK_MODE_NOT_EQUAL 5
#endif