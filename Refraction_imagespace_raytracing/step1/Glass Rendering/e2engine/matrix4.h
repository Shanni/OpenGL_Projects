#pragma once

#include <e2const.h>
#include <vector3.h>
#include <vector4.h>

namespace et
{

template<typename format>
struct matrix4
{
 vector4<format> mat[4];
 
 matrix4()
 {
   mat[0] = vector4<format>(0.0, 0.0, 0.0, 0.0);
   mat[1] = vector4<format>(0.0, 0.0, 0.0, 0.0);
   mat[2] = vector4<format>(0.0, 0.0, 0.0, 0.0);
   mat[3] = vector4<format>(0.0, 0.0, 0.0, 0.0);
 }
 
 matrix4(format s)
 {
  mat[0] = vector4<format>(  s, 0.0, 0.0, 0.0);
  mat[1] = vector4<format>(0.0,   s, 0.0, 0.0);
  mat[2] = vector4<format>(0.0, 0.0,   s, 0.0);
  mat[3] = vector4<format>(0.0, 0.0, 0.0,   s);
 }
 
 matrix4(vector4<format>& c0, vector4<format>& c1, vector4<format>& c2, vector4<format>& c3)
  {
    mat[0] = c0;
    mat[1] = c1;
    mat[2] = c2;
    mat[3] = c3;
  }

 matrix4(const vector4<format>& c0, const vector4<format>& c1, const vector4<format>& c2, const vector4<format>& c3)
  {
    mat[0] = c0;
    mat[1] = c1;
    mat[2] = c2;
    mat[3] = c3;
  }

 inline format* raw()
  { return &mat[0].x; } 
 inline const format* const_raw() const
  { return &mat[0].x; }  
  
 inline format& operator ()(int i)
  { return *(&mat[0].x + i);}
  
 inline vector4<format>& operator [](int i)
  { return mat[i];}

 inline const vector4<format>& operator [](int i) const
  { return mat[i];}
  
 inline vector4<format>& row(int r)
  { return mat[i];}
  
 inline vector4<format> column(int c) 
  { return vector4<format>( mat[0][c], mat[1][c], mat[2][c], mat[3][c]); }
 
 inline matrix4 operator * (format& s)
  { return matrix4<format>(mat[0] * s, mat[1] * s, mat[2] * s, mat[3] * s); }

 inline matrix4 operator / (format& s)
  { return matrix4<format>(mat[0] / s, mat[1] / s, mat[2] / s, mat[3] / s); }
 
 inline matrix4 operator+(matrix4 &m)
 {
  return matrix4(
   mat[0] + m.mat[0],
   mat[1] + m.mat[1],
   mat[2] + m.mat[2],
   mat[3] + m.mat[3]);
 }
 
 inline matrix4 operator - (matrix4 &m)
 {
  return matrix4(
   mat[0] - m.mat[0],
   mat[1] - m.mat[1],
   mat[2] - m.mat[2],
   mat[3] - m.mat[3]);
 }
 
 inline matrix4& operator += (matrix4 &m)
  {
   mat[0] += m.mat[0];
   mat[1] += m.mat[1];
   mat[2] += m.mat[2];
   mat[3] += m.mat[3];
   return *this;
  }
  
 inline matrix4& operator -= (matrix4 &m)
  {
    mat[0] -= m.mat[0];
    mat[1] -= m.mat[1];
    mat[2] -= m.mat[2];
    mat[3] -= m.mat[3];
    return *this;
  }
  
 inline matrix4& operator *= (format &m)
  { 
    mat[0]*=m;
    mat[1]*=m;
    mat[2]*=m;
    mat[3]*=m;
    return *this;
  }
  
 inline matrix4& operator /= (format &m)
  {
   mat[0] /= m;
   mat[1] /= m;
   mat[2] /= m;
   mat[3] /= m; 
   return *this;
  }

 inline vector3<format> operator * (vector3<format> &v) 
 {
  vec3 r;
  r.x = mat[0][0] * v.x + mat[1][0] * v.y + mat[2][0] * v.z + mat[3][0];
  r.y = mat[0][1] * v.x + mat[1][1] * v.y + mat[2][1] * v.z + mat[3][1];
  r.z = mat[0][2] * v.x + mat[1][2] * v.y + mat[2][2] * v.z + mat[3][2];
  return r;
 }
  
 inline vector4<format> operator * (vector4<format> &v) 
 {
  vec4 r;
  r.x = mat[0][0] * v.x + mat[1][0] * v.y + mat[2][0] * v.z + mat[3][0] * v.w;
  r.y = mat[0][1] * v.x + mat[1][1] * v.y + mat[2][1] * v.z + mat[3][1] * v.w;
  r.z = mat[0][2] * v.x + mat[1][2] * v.y + mat[2][2] * v.z + mat[3][2] * v.w;
  r.w = mat[0][3] * v.x + mat[1][3] * v.y + mat[2][3] * v.z + mat[3][3] * v.w;
  return r;
 }

 inline matrix4& operator *= (matrix4& m)
 {
  vector4<format> r0, r1, r2, r3;
 
  r0.x = mat[0].x*m.mat[0].x + mat[0].y*m.mat[1].x + mat[0].z*m.mat[2].x + mat[0].w*m.mat[3].x;
  r0.y = mat[0].x*m.mat[0].y + mat[0].y*m.mat[1].y + mat[0].z*m.mat[2].y + mat[0].w*m.mat[3].y;
  r0.z = mat[0].x*m.mat[0].z + mat[0].y*m.mat[1].z + mat[0].z*m.mat[2].z + mat[0].w*m.mat[3].z;
  r0.w = mat[0].x*m.mat[0].w + mat[0].y*m.mat[1].w + mat[0].z*m.mat[2].w + mat[0].w*m.mat[3].w;
  r1.x = mat[1].x*m.mat[0].x + mat[1].y*m.mat[1].x + mat[1].z*m.mat[2].x + mat[1].w*m.mat[3].x;
  r1.y = mat[1].x*m.mat[0].y + mat[1].y*m.mat[1].y + mat[1].z*m.mat[2].y + mat[1].w*m.mat[3].y;
  r1.z = mat[1].x*m.mat[0].z + mat[1].y*m.mat[1].z + mat[1].z*m.mat[2].z + mat[1].w*m.mat[3].z;
  r1.w = mat[1].x*m.mat[0].w + mat[1].y*m.mat[1].w + mat[1].z*m.mat[2].w + mat[1].w*m.mat[3].w;
  r2.x = mat[2].x*m.mat[0].x + mat[2].y*m.mat[1].x + mat[2].z*m.mat[2].x + mat[2].w*m.mat[3].x;
  r2.y = mat[2].x*m.mat[0].y + mat[2].y*m.mat[1].y + mat[2].z*m.mat[2].y + mat[2].w*m.mat[3].y;
  r2.z = mat[2].x*m.mat[0].z + mat[2].y*m.mat[1].z + mat[2].z*m.mat[2].z + mat[2].w*m.mat[3].z;
  r2.w = mat[2].x*m.mat[0].w + mat[2].y*m.mat[1].w + mat[2].z*m.mat[2].w + mat[2].w*m.mat[3].w;
  r3.x = mat[3].x*m.mat[0].x + mat[3].y*m.mat[1].x + mat[3].z*m.mat[2].x + mat[3].w*m.mat[3].x;
  r3.y = mat[3].x*m.mat[0].y + mat[3].y*m.mat[1].y + mat[3].z*m.mat[2].y + mat[3].w*m.mat[3].y;
  r3.z = mat[3].x*m.mat[0].z + mat[3].y*m.mat[1].z + mat[3].z*m.mat[2].z + mat[3].w*m.mat[3].z;
  r3.w = mat[3].x*m.mat[0].w + mat[3].y*m.mat[1].w + mat[3].z*m.mat[2].w + mat[3].w*m.mat[3].w;

  mat[0] = r0;
  mat[1] = r1;
  mat[2] = r2;
  mat[3] = r3;
  return *this;
 }

inline matrix4 operator ^ (matrix4& m)
 {
 }

 inline matrix4 operator * (matrix4& m)
 {
  vector4<format> r0, r1, r2, r3;

  r0.x = mat[0].x*m.mat[0].x + mat[0].y*m.mat[1].x + mat[0].z*m.mat[2].x + mat[0].w*m.mat[3].x;
  r0.y = mat[0].x*m.mat[0].y + mat[0].y*m.mat[1].y + mat[0].z*m.mat[2].y + mat[0].w*m.mat[3].y;
  r0.z = mat[0].x*m.mat[0].z + mat[0].y*m.mat[1].z + mat[0].z*m.mat[2].z + mat[0].w*m.mat[3].z;
  r0.w = mat[0].x*m.mat[0].w + mat[0].y*m.mat[1].w + mat[0].z*m.mat[2].w + mat[0].w*m.mat[3].w;
  
  r1.x = mat[1].x*m.mat[0].x + mat[1].y*m.mat[1].x + mat[1].z*m.mat[2].x + mat[1].w*m.mat[3].x;
  r1.y = mat[1].x*m.mat[0].y + mat[1].y*m.mat[1].y + mat[1].z*m.mat[2].y + mat[1].w*m.mat[3].y;
  r1.z = mat[1].x*m.mat[0].z + mat[1].y*m.mat[1].z + mat[1].z*m.mat[2].z + mat[1].w*m.mat[3].z;
  r1.w = mat[1].x*m.mat[0].w + mat[1].y*m.mat[1].w + mat[1].z*m.mat[2].w + mat[1].w*m.mat[3].w;
  
  r2.x = mat[2].x*m.mat[0].x + mat[2].y*m.mat[1].x + mat[2].z*m.mat[2].x + mat[2].w*m.mat[3].x;
  r2.y = mat[2].x*m.mat[0].y + mat[2].y*m.mat[1].y + mat[2].z*m.mat[2].y + mat[2].w*m.mat[3].y;
  r2.z = mat[2].x*m.mat[0].z + mat[2].y*m.mat[1].z + mat[2].z*m.mat[2].z + mat[2].w*m.mat[3].z;
  r2.w = mat[2].x*m.mat[0].w + mat[2].y*m.mat[1].w + mat[2].z*m.mat[2].w + mat[2].w*m.mat[3].w;
  
  r3.x = mat[3].x*m.mat[0].x + mat[3].y*m.mat[1].x + mat[3].z*m.mat[2].x + mat[3].w*m.mat[3].x;
  r3.y = mat[3].x*m.mat[0].y + mat[3].y*m.mat[1].y + mat[3].z*m.mat[2].y + mat[3].w*m.mat[3].y;
  r3.z = mat[3].x*m.mat[0].z + mat[3].y*m.mat[1].z + mat[3].z*m.mat[2].z + mat[3].w*m.mat[3].z;
  r3.w = mat[3].x*m.mat[0].w + mat[3].y*m.mat[1].w + mat[3].z*m.mat[2].w + mat[3].w*m.mat[3].w;

  return matrix4<format>(r0, r1, r2, r3);  
 }
 
 inline format determinant()
 {
  format& a10 = mat[1].x;
  format& a11 = mat[1].y;
  format& a12 = mat[1].z;
  format& a13 = mat[1].w;
  format& a20 = mat[2].x; 
  format& a21 = mat[2].y; 
  format& a22 = mat[2].z; 
  format& a23 = mat[2].w; 
  format& a30 = mat[3].x;
  format& a31 = mat[3].y;
  format& a32 = mat[3].z;
  format& a33 = mat[3].w;
  return mat[0].x * (a11 * (a22*a33-a23*a32) +
                     a12 * (a31*a23-a21*a33) +
                     a13 * (a21*a32-a22*a31))+
         mat[0].y * (a10 * (a23*a32-a22*a33) +
                     a20 * (a12*a33-a13*a32) +
                     a30 * (a13*a22-a12*a23))+
         mat[0].z * (a10 * (a21*a33-a31*a23) +
                     a11 * (a30*a23-a20*a33) +
                     a13 * (a20*a31-a21*a30))+ 
         mat[0].w * (a10 * (a22*a31-a21*a32) +
                     a11 * (a20*a32-a30*a22) +
                     a12 * (a21*a30-a20*a31));
 }
 
 inline matrix3<format> subMatrix(int r, int c)
 {
  matrix3<format> m;
  
  int i = 0; 
  for (int y = 0; y < 4; ++y)
   if (y != r)
   {
    int j = 0;
    if (0 != c) m[i][j++] = mat[y].x;
    if (1 != c) m[i][j++] = mat[y].y;
    if (2 != c) m[i][j++] = mat[y].z;
    if (3 != c) m[i][j++] = mat[y].w;
    ++i;
   } 

  return m;
 }
 
 inline matrix4<format> adjugateMatrix()
 {
  matrix4<format> m;
  
  m[0].x =  subMatrix(0, 0).determinant();
  m[0].y = -subMatrix(1, 0).determinant();
  m[0].z =  subMatrix(2, 0).determinant();
  m[0].w = -subMatrix(3, 0).determinant();
  m[1][0] = -subMatrix(0, 1).determinant();
  m[1][1] =  subMatrix(1, 1).determinant();
  m[1][2] = -subMatrix(2, 1).determinant();
  m[1][3] =  subMatrix(3, 1).determinant();
  m[2][0] =  subMatrix(0, 2).determinant();
  m[2][1] = -subMatrix(1, 2).determinant();
  m[2][2] =  subMatrix(2, 2).determinant();
  m[2][3] = -subMatrix(3, 2).determinant();
  m[3][0] = -subMatrix(0, 3).determinant();
  m[3][1] =  subMatrix(1, 3).determinant();
  m[3][2] = -subMatrix(2, 3).determinant();
  m[3][3] =  subMatrix(3, 3).determinant();
  
  return m;
 }
 
 inline vector3<format> rotationMultiply(vector3<format>& v)
 {
  vec3 r;
  r.x = mat[0][0] * v.x + mat[1][0] * v.y + mat[2][0] * v.z;
  r.y = mat[0][1] * v.x + mat[1][1] * v.y + mat[2][1] * v.z;
  r.z = mat[0][2] * v.x + mat[1][2] * v.y + mat[2][2] * v.z;
  return r;
 }
 
 inline matrix4<format> inverse()
 {
  format det = determinant();
  return (det*det > 0) ? adjugateMatrix() / det : matrix4<format>(0);
 }
 
 inline vector4<format>extractPosition()
 {
  return -inverse().mat[3];
 }
 
};

template <typename format>
inline matrix4<format> _transMatrix(format x, format y, format z)
{
 matrix4<format> M;
 M[0][0] = M[1][1] = M[2][2] = M[3][3] = format(1);
 M[3][0] = x;
 M[3][1] = y;
 M[3][2] = z;
 return M;
}

template <typename format>
inline matrix4<format> _transScaleMatrix(format tx, format ty, format tz, format sx, format sy, format sz)
{
 matrix4<format> M;
 M[0][0] = sx;
 M[1][1] = sy;
 M[2][2] = sz;
 M[3][3] = format(1);
 
 M[3][0] = tx;
 M[3][1] = ty;
 M[3][2] = tz;
 return M;
}

template <typename format>
inline matrix4<format> _scaleMatrix(format x, format y, format z)
{
 matrix4<format> M;
 M[0][0] = x;
 M[1][1] = y;
 M[2][2] = z;
 M[3][3] = format(1);
 return M;
}

template <typename format>
inline matrix4<format> _rotationYXZMatrix(format x, format y, format z)
{
 matrix4<format> m;

 float sx = sin(x);
 float cx = cos(x);
 float sy = sin(y);
 float cy = cos(y);
 float sz = sin(z);
 float cz = cos(z);

 m[0][0] =  cz*cy - sz*sx*sy; m[0][1] = -cx*sz; m[0][2] = cz*sy + sz*sx*cy;
 m[1][0] =  sz*cy + cz*sx*sy; m[1][1] =  cx*cz; m[1][2] = sz*sy - cz*sx*cy;
 m[2][0] = -cx*sy;            m[2][1] =  sx;    m[2][2] = cx*cy;           
 m[3][3] = 1;

 return m;
}

template <typename format>
inline matrix4<format> _transformYXZMatrix(format tx, format ty, format tz, format rx, format ry, format rz)
{
 matrix4<format> m;

 float sx = sin(rx);
 float cx = cos(rx);
 float sy = sin(ry);
 float cy = cos(ry);
 float sz = sin(rz);
 float cz = cos(rz);

 m[0][0] =  cz*cy - sz*sx*sy; m[0][1] = -cx*sz; m[0][2] = cz*sy + sz*sx*cy;
 m[1][0] =  sz*cy + cz*sx*sy; m[1][1] =  cx*cz; m[1][2] = sz*sy - cz*sx*cy;
 m[2][0] = -cx*sy;            m[2][1] =  sx;    m[2][2] = cx*cy;           
 m[3][0] = tx;                m[3][1] =  ty;    m[3][2] = tz; 
 
 m[3][3] = 1;

 return m;
}


}