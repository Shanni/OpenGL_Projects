#include <e2core.h>
#include <e2helpers.h>
#include <e2const.h>

using namespace et;

/// basic helper /////////////////////////////////////////////////////
void Ce2BasicHelper::lookAt(vec3 from, vec3 to, vec3 up)
{
 _position = from;
 _view     = to;
 _mat_mv   = IDENTITY_MATRIX;
 
 vec3 dir = normalize(to - from);
 
 vec3 s = dir.cross(up).normalize();
 vec3 u = s.cross(dir).normalize();
 
 vec3 e(-s.dot(from), -u.dot(from), dir.dot(from));
 
 _mat_mv[0] = vec4(s.x, u.x, -dir.x, 0.0);
 _mat_mv[1] = vec4(s.y, u.y, -dir.y, 0.0);
 _mat_mv[2] = vec4(s.z, u.z, -dir.z, 0.0);
 _mat_mv[3] = vec4(e.x, e.y,    e.z, 1.0);
 
 _afterupdate();
}

void Ce2BasicHelper::perspective(float fov, float aspect, float zNear, float zFar)
{
 _aspect   = aspect;
 _fov      = fov;
 _znear    = zNear;
 _zfar     = zFar;
 _mat_proj = Ce2BasicHelper::perspectiveProjection(fov, aspect, zNear, zFar);
 _afterupdate();
}

void Ce2BasicHelper::orthographic(float left, float right, float top, float bottom, float zNear, float zFar)
{
 _mat_proj = IDENTITY_MATRIX;
 _mat_proj[0][0] =  2.0f / (right - left);
 _mat_proj[1][1] =  2.0f / (top   - bottom);
 _mat_proj[2][2] = -2.0f / (zFar - zNear);
 
 _mat_proj[3][0] = -(right + left  ) / (right - left);
 _mat_proj[3][1] = -(top   + bottom) / (top   - bottom);
 _mat_proj[3][2] = -(zFar  + zNear ) / (zFar  - zNear);
 
 _afterupdate(); 
}

void Ce2BasicHelper::setPosition(const vec3 &p)
{
 _position = p;
 vec3 e( sideVector().dot(p),  upVector().dot(p), -direction().dot(p));
 _mat_mv[3] = vec4(-e.x, -e.y, -e.z, 1.0);
 _afterupdate();
}

void Ce2BasicHelper::_afterupdate()
{
 _mat_mvp = _mat_mv * _mat_proj;
 _mat_inv_mvp = _mat_mvp.inverse();
 _mat_lpr = _mat_mvp * MATRIX_PROJECTION;
}

mat4 Ce2BasicHelper::perspectiveProjection(float fov, float aspect, float zNear, float zFar)
{
 mat4 result = IDENTITY_MATRIX;

 float fHalfFOV = 0.5f * fov;
 float cotan = cos(fHalfFOV) / sin(fHalfFOV);
 float dz = zFar - zNear;

 result[0][0] = cotan / aspect;
 result[1][1] = cotan;
 result[2][2] = -(zFar + zNear) / dz;
 result[3][3] =  0.0f;
 result[2][3] = -1.0f;
 result[3][2] = -2.0f * zNear * zFar / dz;

 return result;
}

CubemapMatrixArray Ce2BasicHelper::cubemapMatrix(const mat4& projectionMatrix, const vec3& pointOfView)
{
 CubemapMatrixArray result;
 
 mat4 translation = translationMatrix(-pointOfView);

 const vec4& rX = projectionMatrix[0];
 const vec4& rY = projectionMatrix[1];
 const vec4& rZ = projectionMatrix[2];
 const vec4& rW = projectionMatrix[3];
 result[0] = translation * mat4( -rZ, -rY, -rX, rW );
 result[1] = translation * mat4(  rZ, -rY,  rX, rW );
 result[2] = translation * mat4(  rX, -rZ,  rY, rW );
 result[3] = translation * mat4(  rX,  rZ, -rY, rW );
 result[4] = translation * mat4(  rX, -rY, -rZ, rW );
 result[5] = translation * mat4( -rX, -rY,  rZ, rW );

 return result;
}

vec3 Ce2BasicHelper::project(int x, int y, float depth)
{
 float fx = float(x) / core.windowSize.x;
 float fy = float(core.windowSize.y - y) / core.windowSize.y;

 vec4 proj = 2.0f * vec4(fx, fy, depth, 1.0) - vec4(1.0);
 
 proj = _mat_inv_mvp * proj;
 proj /= proj.w;
 
 return proj.xyz();
}

// camera ////////////////////////////////////////////////////////////
Ce2Camera::Ce2Camera()
{
 lockUpVector = false;
 _mat_mv   = IDENTITY_MATRIX;
 _mat_proj = IDENTITY_MATRIX;
 _position = vec3(0.0);
 _view     = vec3(0.0);
}

void Ce2Camera::LookAt(float x, float y, float z, float vx, float vy, float vz, float ux, float uy, float uz)
{
 lookAt(
  vec3( x,  y,  z),
  vec3(vx, vy, vz),
  vec3(ux, uy, uz) );
}

void Ce2Camera::MoveForward(float speed)
{
 _position -= vec3(_mat_mv[0][2] * speed, _mat_mv[1][2] * speed, _mat_mv[2][2] * speed);
 
 _mat_mv   *= translationMatrix(0.0, 0.0, speed);
 _afterupdate();
}

void Ce2Camera::MoveBackward(float speed)
{
 _position += vec3(_mat_mv[0][2] * speed, _mat_mv[1][2] * speed, _mat_mv[2][2] * speed);
 
 _mat_mv *= translationMatrix(0.0, 0.0, -speed);
 _afterupdate();
}

void Ce2Camera::StrafeLeft(float speed)
{
 _position -= vec3(_mat_mv[0][0] * speed, _mat_mv[1][0] * speed, _mat_mv[2][0] * speed);
 
 _mat_mv *= translationMatrix(speed, 0.0, 0.0);
 _afterupdate();
}

void Ce2Camera::StrafeRight(float speed)
{
 _position += vec3(_mat_mv[0][0] * speed, _mat_mv[1][0] * speed, _mat_mv[2][0] * speed);
 
 _mat_mv *= translationMatrix(-speed, 0.0, 0.0);
 _afterupdate();
}

vec3 Ce2Camera::GetViewAngles()
{
 return vec3(asin(-_mat_mv[1][2]), atan2(-_mat_mv[2][2], -_mat_mv[0][2]), (_view - _position).length());
}

void Ce2Camera::rotateUpVector(float angle)
{
 if (lockUpVector || (angle == 0.0)) return;
  
 _mat_mv *= quaternionFromAxisAngle(angle, vec3(0.0, 0.0, 1.0)).toMatrix();
 _afterupdate();
}

void Ce2Camera::rotateView(float dtheta, float dphi)
{
 if ((dtheta == 0.0) && (dphi == 0.0)) return;
 
 Quaternion Q = quaternionFromAxisAngle( dphi, vec3(0.0, 1.0, 0.0) ) * 
                quaternionFromAxisAngle( dtheta, vec3(1.0, 0.0, 0.0) );
                
 _mat_mv *= Q.toMatrix();

 if (lockUpVector)
 {
  vec3& d = direction();
  vec3 s = normalize(vec3(-d.z, 0.0, d.x));
  vec3 u = normalize(s.cross(d));
  vec3 e(-s.dot(position()), -u.dot(position()), d.dot(position()));
  _mat_mv[0] = vec4(s.x, u.x, -d.x, 0.0);
  _mat_mv[1] = vec4(s.y, u.y, -d.y, 0.0);
  _mat_mv[2] = vec4(s.z, u.z, -d.z, 0.0);
  _mat_mv[3] = vec4(e.x, e.y,  e.z, 1.0);
 }


 _afterupdate();
}