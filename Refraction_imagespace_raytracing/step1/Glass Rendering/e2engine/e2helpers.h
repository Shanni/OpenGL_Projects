#include <e2geometry.h>
#include <e2classes.h>

#pragma once

namespace et
{

typedef Ce2StaticArray<mat4, 6> CubemapMatrixArray;

class Ce2BasicHelper
{
 protected:
  vec3 _position;
  vec3 _view;
  mat4 _mat_mv;
  mat4 _mat_proj;
  mat4 _mat_mvp;
  mat4 _mat_lpr;
  mat4 _mat_inv_mvp;
  float _fov;
  float _aspect;
  float _znear;
  float _zfar;
  virtual void perspective (float fov, float aspect, float zNear, float zFar);
  virtual void orthographic(float left, float right, float top, float bottom, float zNear, float zFar);
  virtual void lookAt(vec3 from, vec3 to, vec3 up);
  virtual void setPosition(const vec3 &p);
  virtual void _afterupdate();
 public:
  inline mat4& modelViewMatrix()  {return _mat_mv;}
  inline mat4& projectionMatrix() {return _mat_proj;}
  inline mat4& mvpMatrix()        {return _mat_mvp;}
  inline mat4& inverseMVPMatrix() {return _mat_inv_mvp;}
  
  // map window coordinates to world
  vec3 project(int x, int y, float depth);
  
  static CubemapMatrixArray cubemapMatrix(const mat4& projectionMatrix, const vec3& pointOfView);
  static mat4 perspectiveProjection(float fov, float aspect, float zNear, float zFar);
  
  inline mat4& projectionToTexutreMatrix() {return _mat_lpr;}
  
  inline vec3& position() {return _position;}
  inline vec3 sideVector(){return vec3( _mat_mv[0][0],  _mat_mv[1][0],  _mat_mv[2][0]);}
  inline vec3 upVector()  {return vec3( _mat_mv[0][1],  _mat_mv[1][1],  _mat_mv[2][1]);}
  inline vec3 direction() {return vec3(-_mat_mv[0][2], -_mat_mv[1][2], -_mat_mv[2][2]);}
  inline float& nearClipplane() {return _znear;}
  inline float& farClipplane() {return _zfar;}
};

class Ce2Camera : public Ce2BasicHelper
{
 public:
  bool lockUpVector;

 public:
  Ce2Camera();

  virtual void perspective(float fov, float aspect, float zNear, float zFar)
   { Ce2BasicHelper::perspective(fov, aspect, zNear, zFar); }
  virtual void lookAt(vec3 from, vec3 to, vec3 up)
   { Ce2BasicHelper::lookAt(from, to, up); }
  virtual void orthographic(float left, float right, float top, float bottom, float zNear, float zFar)
   { Ce2BasicHelper::orthographic(left, right, top, bottom, zNear, zFar); } 
   
  vec3 GetViewAngles();
  
  mat4& lightProjectionMatrix() {return _mat_lpr;}
  
  void LookAt(float x, float y, float z, float vx = 0.0, float vy = 0.0, float vz = 0.0, float ux = 0.0, float uy = 1.0, float uz = 0.0);
  void Update(){_afterupdate();}
  
  void ScaleModelView(float sx, float sy, float sz)
   {
    mat4 s   = scaleMatrix(sx, sy, sz);
    _mat_mv *= s;
    _afterupdate();
   }
  
  void MoveForward (float speed = 1.0);
  void MoveBackward(float speed = 1.0);
  void StrafeLeft  (float speed = 1.0);
  void StrafeRight (float speed = 1.0);
  
  void rotateView    (float dtheta, float dphi);
  void rotateUpVector(float dAngle);
};

class Ce2Light : public Ce2Camera
{
};

}