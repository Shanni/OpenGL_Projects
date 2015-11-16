#pragma once

#include <e2const.h>
#include <vector3.h>

namespace et
{

template<typename format>
struct matrix3
{
 vector3<format>mat[3];
 matrix3()
 {
  mat[0] = mat[1] = mat[2] = vector3<format>(0);
 }
 matrix3(vector3<format>c0, vector3<format>c1, vector3<format>c2)
 {
  mat[0] = c0; 
  mat[1] = c1;
  mat[2] = c2;
 }
 matrix3(format a00, format a01, format a02,
         format a10, format a11, format a12,
         format a20, format a21, format a22)
 {
  mat[0] = vec3(a00, a01, a02);
  mat[1] = vec3(a10, a11, a12);
  mat[2] = vec3(a20, a21, a22);
 }        
 
 inline vector3<format>& operator[](int i){return mat[i];}
 inline format& operator()(int i){return *(&mat[0].x + i);}
 
 inline format determinant()
 {
  format a10 = mat[1].x;
  format a11 = mat[1].y;
  format a12 = mat[1].z;
  format a20 = mat[2].x;
  format a21 = mat[2].y;
  format a22 = mat[2].z;
  return mat[0].x * (a11*a22-a12*a21) + 
         mat[0].y * (a20*a12-a10*a22) + 
         mat[0].z * (a10*a21-a11*a20);
 }
};

}