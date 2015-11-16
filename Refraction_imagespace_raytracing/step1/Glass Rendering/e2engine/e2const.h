#include <vector>

#pragma once

#define GL_ERROR_INVALID_VERSION_ARB 0x2095
#define GL_ERROR_INVALID_PROFILE_ARB 0x2096
#define GL_FRAMEBUFFER_INCOMPLETE_UNSUPPORTED

#define GL_GEOMETRY_VERTICES_OUT_ 0x8DDA
#define GL_GEOMETRY_INPUT_TYPE_   0x8DDB
#define GL_GEOMETRY_OUTPUT_TYPE_  0x8DDC     

// WINDOW STYLES
#define RW_FIXED            0
#define RW_FITWORKAREA      1
#define RW_FITSCREEN        2
#define RW_FIXED_NO_CAPTION 3

//// ERRORS
#define E2_OK           0
#define E2_COMMONERROR  1

// DEFAULT TIMER ID
#define E2_TIMER_ID   666

// RENDER ATTRIBUTES
#define RENDER_ATTRIB_POSITION  0
#define RENDER_ATTRIB_NORMAL    1
#define RENDER_ATTRIB_TEXCOORD0 2
#define RENDER_ATTRIB_TANGENT   3

// UNIFORM TYPES
#define UNIFORM_FLOAT   0
#define UNIFORM_VEC2    1
#define UNIFORM_VEC3    2
#define UNIFORM_VEC4    3
#define UNIFORM_MAT3    4
#define UNIFORM_MAT4    5
#define UNIFORM_SAMPLER 6

// MOUSE BUTTONS
#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_RIGHT 1
#define MOUSE_BUTTON_MIDDLE 2

#ifndef PI
 #define PI         3.1415926535897932384626433832795f
#endif

#define DOUBLE_PI   6.283185307179586476925286766559f
#define HALF_PI     1.5707963267948966192313216916398f
#define QUAD_PI     0.78539816339744830961566084581988f
#define TO_DEGREES 57.295779513082320876798154814105f
#define TO_RADIANS  0.01745329251994329576923690768489f;

// SOME DEGREES
const float DEG_15 = 0.26179938779914943653855361527329f;
const float DEG_30 = 0.52359877559829887307710723054658f;
const float DEG_45 = 0.78539816339744830961566084581988f;
const float DEG_60 = 1.0471975511965977461542144610932f;
const float DEG_90 = 1.5707963267948966192313216916398f;

const float SQRT_2 = 1.4142135623730950488016887242097f;

static const char* DEFAULT_VERTEXSHADER   = "void main(){gl_Position = vec4(0.0, 0.0, 0.0, 1.0);}";
static const char* DEFAULT_FRAGMENTSHADER = "#version 150\nprecision highp float;\n out vec4 FragColor; void main(){FragColor = vec4(1.0, 0.0, 0.0, 1.0);}";

// KEYS

#define VK_0 '0'
#define VK_1 '1'
#define VK_2 '2'
#define VK_3 '3'
#define VK_4 '4'
#define VK_5 '5'
#define VK_6 '6'
#define VK_7 '7'
#define VK_8 '8'
#define VK_9 '9'

#define VK_A 'A'
#define VK_B 'B'
#define VK_C 'C'
#define VK_D 'D'
#define VK_E 'E'
#define VK_F 'F'
#define VK_G 'G'
#define VK_H 'H'
#define VK_I 'I'
#define VK_J 'J'
#define VK_K 'K'
#define VK_L 'L'
#define VK_M 'M'
#define VK_N 'N'
#define VK_O 'O'
#define VK_P 'P'
#define VK_Q 'Q'
#define VK_R 'R'
#define VK_S 'S'
#define VK_T 'T'
#define VK_U 'U'
#define VK_V 'V'
#define VK_W 'W'
#define VK_X 'X'
#define VK_Y 'Y'
#define VK_Z 'Z'

struct RENDERPARAMS
{
 bool bForwardContext;
 
 RENDERPARAMS()
 {
  bForwardContext = true;
 }
};

struct ENGINEPARAMS
{
 int eWindowStyle;
 int vPosX, vPosY;
 int vSizeX, vSizeY;
 RENDERPARAMS eRender;
};

struct render_attrib
{
 int id;
 int numFloats;
 render_attrib()
  { id = 0;  numFloats = 0; }
 render_attrib(int _id, int _numFloats)
  { id = _id; numFloats = _numFloats; }
};

typedef std::vector<render_attrib> renderAttributeVector;
