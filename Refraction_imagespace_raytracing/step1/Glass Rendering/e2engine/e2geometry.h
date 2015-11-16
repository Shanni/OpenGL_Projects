#include <algorithm>
#include <math.h>
#include <vector>
#include <e2const.h>

#include <vector2.h>
#include <vector3.h>
#include <vector4.h>
#include <matrix3.h>
#include <matrix4.h>
#include <quaternion.h>

#pragma once

using namespace std;
namespace et
{

typedef unsigned int Index;

template<typename format>
 inline format sqr(format value) { return value*value; }

template<typename format>
 inline format clamp(format value, format min, format max)
  { return (value < min) ? min : (value > max) ? max : value; }
  
inline Index getIndex(int u, int v, int u_sz, int v_sz)
 { return clamp(u, 0, u_sz - 1) + clamp(v, 0, v_sz - 1) * u_sz; }

struct Complex
{
 float Re;
 float Im;
};

typedef vector2<float>         vec2;
typedef vector2<double>        vec2d;
typedef vector2<long double>   vec2ld;
typedef vector2<int>           vec2i;
typedef vector2<unsigned char> vec2ub;
typedef vector2<unsigned long> vec2ul;
typedef vector2<Complex>       vec2x;

typedef vector3<float>         vec3;
typedef vector3<double>        vec3d;
typedef vector3<long double>   vec3ld;
typedef vector3<int>           vec3i;
typedef vector3<unsigned char> vec3ub;
typedef vector3<Complex>       vec3x;

typedef vector4<float>         vec4;
typedef vector4<double>        vec4d;
typedef vector4<long double>   vec4ld;
typedef vector4<int>           vec4i;
typedef vector4<unsigned char> vec4ub;
typedef vector4<Complex>       vec4x;

typedef matrix3<float> mat3;
typedef matrix3<double> mat3d;

typedef matrix4<float> mat4;
typedef matrix4<double> mat4d;
typedef matrix4<long double> mat4ld;

typedef quaternion<float>         Quaternion;
typedef quaternion<double>        Quaterniond;
typedef quaternion<long double>   Quaternionld;
typedef quaternion<int>           Quaternioni;
typedef quaternion<unsigned char> Quaternionub;

/// constants ////////////////////////////////////////////////
static const vec3 NULL_VEC3(0.0, 0.0, 0.0);

static const vec3 ORTH_I(1.0, 0.0, 0.0);
static const vec3 ORTH_J(0.0, 1.0, 0.0);
static const vec3 ORTH_K(0.0, 0.0, 1.0);

static mat4 IDENTITY_MATRIX(1.0f);
static mat4 MATRIX_PROJECTION( vec4(0.5, 0.0, 0.0, 0.0),
                               vec4(0.0, 0.5, 0.0, 0.0),
                               vec4(0.0, 0.0, 0.5, 0.0),
                               vec4(0.5, 0.5, 0.5, 1.0) );


template<typename format>
inline vector3<format> abs(const vector3<format>& value) 
 { return vector3<format>(::abs(value.x), ::abs(value.y), ::abs(value.z)); }

template<typename format>
inline vector4<format> abs(const vector4<format>& value) 
 { return vector4<format>(::abs(value.x), ::abs(value.y), ::abs(value.z) ::abs(value.z)); }

template<typename format>
inline vector3<format> maxv(const vector3<format>& v1, const vector3<format>& v2)
 {
  return vector3<format>(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z));
 }

template<typename format>
inline vector3<format> minv(const vector3<format>& v1, const vector3<format>& v2)
 {
  return vector3<format>(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z)); }

inline float sign(float s) {return (s == 0) ? 0.0f : s / fabs(s);}
inline float sign_nz(float s) {return (s < 0) ? -1.0f : 1.0f;}

inline vec3 randVector(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f)
{
 return vec3( -sx + 2.0f * sx * rand() / RAND_MAX, 
              -sy + 2.0f * sy * rand() / RAND_MAX, 
              -sz + 2.0f * sz * rand() / RAND_MAX );
}

inline pair<float, int> min3(float a, float b, float c)
{
 int k = 0; 
 float m = a;
 if (b < m) {k = 1; m = b;}
 if (c < m) {k = 2; m = c;}
 return pair<float, int>(m, k);
}

inline vec3ub vec3f_to_3ub(const vec3 &fv)
{
 return vec3ub( unsigned char(clamp<float>(0.5f + 0.5f * fv.x, 0.0, 1.0) * 255), 
                unsigned char(clamp<float>(0.5f + 0.5f * fv.y, 0.0, 1.0) * 255), 
                unsigned char(clamp<float>(0.5f + 0.5f * fv.z, 0.0, 1.0) * 255) );
}

struct VERT_V2_T2
{
 vec2 vPosition;
 vec2 vTexCoord;
 VERT_V2_T2(){vPosition=vec2(0.0);vTexCoord=vec2(0.0);}
 VERT_V2_T2(float x, float y, float u, float v){vPosition.x = x;vPosition.y = y;vTexCoord.x=u;vTexCoord.y=v;}

 static renderAttributeVector getRA()
 {
  renderAttributeVector v;
  v.push_back(render_attrib(RENDER_ATTRIB_POSITION,  2));
  v.push_back(render_attrib(RENDER_ATTRIB_TEXCOORD0, 2));
  return v;
 }
};

struct VERT_V3_T2
{
 vec3 vPosition;
 vec2 vTexCoord;

 static renderAttributeVector getRA()
 {
  renderAttributeVector v;
  v.push_back(render_attrib(RENDER_ATTRIB_POSITION,  3));
  v.push_back(render_attrib(RENDER_ATTRIB_TEXCOORD0, 2));
  return v;
 }
};

struct VERT_V4_T2
{
 vec4 vPosition;
 vec2 vTexCoord;

 static renderAttributeVector getRA()
 {
  renderAttributeVector v;
  v.push_back(render_attrib(RENDER_ATTRIB_POSITION,  4));
  v.push_back(render_attrib(RENDER_ATTRIB_TEXCOORD0, 2));
  return v;
 }
};


struct VERT_V3_N3_T2
{
 vec3 vPosition;
 vec3 vNormal;
 vec2 vTexCoord;
 static renderAttributeVector getRA()
 {
  renderAttributeVector v;
  v.push_back(render_attrib(RENDER_ATTRIB_POSITION, 3));
  v.push_back(render_attrib(RENDER_ATTRIB_NORMAL, 3));
  v.push_back(render_attrib(RENDER_ATTRIB_TEXCOORD0, 2));
  return v;
 }
};

struct VERT_V3_N3
{
 vec3 vPosition;
 vec3 vNormal;
 static renderAttributeVector getRA()
 {
  renderAttributeVector v;
  v.push_back(render_attrib(RENDER_ATTRIB_POSITION, 3));
  v.push_back(render_attrib(RENDER_ATTRIB_NORMAL, 3));
  return v;
 }
};

inline mat4 translationMatrix(float x, float y, float z) { return _transMatrix<float>(x, y, z); }
inline mat4 scaleMatrix(float x, float y, float z) { return _scaleMatrix<float>(x, y, z); }
inline mat4 rotationYXZMatrix(float x, float y, float z) { return _rotationYXZMatrix<float>(x, y, z); }

inline mat4 translationMatrix(const vec3& v) { return _transMatrix<float>(v.x, v.y, v.z); }
inline mat4 translationScaleMatrix(vec3 t, vec3 s) { return _transScaleMatrix<float>(t.x, t.y, t.z, s.x, s.y, s.z); }
inline mat4 scaleMatrix(vec3 v) { return _scaleMatrix<float>(v.x, v.y, v.z); }
inline mat4 rotationYXZMatrix(vec3 v) { return _rotationYXZMatrix<float>(v.x, v.y, v.z); }

inline mat4 transformYXZMatrix(vec3 translate, vec3 rotate) 
 { return _transformYXZMatrix<float>(translate.x, translate.y, translate.z, rotate.x, rotate.y, rotate.z); }

template<typename format>
inline vector3<format> planeNormal(vector3<format> p0, vector3<format> p1, vector3<format> p2)
 { return (p2 - p0).cross(p1 - p0).normalize(); }

template<typename format>
inline vector2<format> minv(vector2<format> a, vector2<format> b)
 {return vector2<format>( min(a.x, b.x), min(a.y, b.y) );}
template<typename format>
inline vector2<format> maxv(vector2<format> a, vector2<format> b)
 {return vector2<format>( max(a.x, b.x), max(a.y, b.y) );}

template<typename format>
inline vector4<format> mix(vector4<format> v1, vector4<format> v2, format t)
{
 format nt = 1 - t;
 return vector4<format>( v1.x * nt + v2.x * t, 
                         v1.y * nt + v2.y * t, 
                         v1.z * nt + v2.z * t, 
                         v1.w * nt + v2.w * t);
}

template<typename format>
inline vector3<format> mix(vector3<format> v1, vector3<format> v2, format t)
{ 
 format nt = 1 - t;
 return vector3<format>(v1.x * nt + v2.x * t, 
                        v1.y * nt + v2.y * t, 
                        v1.z * nt + v2.z * t);
}

template<typename format>
inline vector2<format> mix(vector2<format> v1, vector2<format> v2, format t)
 { return vector2<format>(v1.x * (format(1.0) - t) + v2.x * t, 
                          v1.y * (format(1.0) - t) + v2.y * t);}

template<typename format>
inline format mix(format v1, format v2, format t)
 { return v1 * (format(1.0) - t) + v2 * t;}

template<typename format>
inline vector3<format>fromSpherical(format theta, format phi)
{
 format fCosTheta = cos(theta);
 return vec3( fCosTheta * cos(phi), 
              sin(theta), 
              fCosTheta * sin(phi) );
}

template<typename format>
inline vector3<format>fromSphericalRotated(format theta, format phi)
{
 format fSinTheta = sin(theta);
 return vec3( fSinTheta * cos(phi), 
              cos(theta), 
              fSinTheta * sin(phi) );
}

template <typename format>
 inline vector3<format> normalize(vector3<format> v)
  { return v.normalize(); }
  
template <typename format>
quaternion<format> quaternionFromAxisAngle(format angle, vector3<format>axis)
{
 format half_angle = format(0.5) * angle;
 return quaternion<format>(cos(half_angle), axis * sin(half_angle));
}

template <typename format>  
 inline vector3<format> reflect(vector3<format>& v, vector3<format>& n)
  { return v - 2 * n.dot(v) * n; }

}