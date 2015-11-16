#include <e2const.h>
#include <vector3.h>
#include <vector4.h>
#include <matrix4.h>

namespace et
{

template <typename format>
struct quaternion
{
 format          scalar;
 vector3<format> vector;
 
 quaternion()
  {
   scalar = 0.0; 
   vector = vector3<format>(0.0);
  }
  
 quaternion(format w, format x, format y, format z) 
  {
   scalar = w;
   vector = vector3<format>(x, y, z);
  }
  
 quaternion(format s, vector3<format> v)
  {
   scalar = s;
   vector = v;
  }
  
 quaternion(vector3<format> v)
  {
   scalar=0.0;
   vector=v;
  }
 
 quaternion(format angle, int axis)
  {
   format fHalfAngle = angle / 2.0;
   format fSin = sin(fHalfAngle);
   
   scalar = cos(fHalfAngle);
   vector.x = fSin * (axis == 1);
   vector.y = fSin * (axis == 2);
   vector.z = fSin * (axis == 3);
  }
 
 inline format operator[](int i) {return (&scalar)[i];};
 inline quaternion operator * (const format& value) {return quaternion(scalar*value, vector*value);}
 inline quaternion operator / (const format& value) {return quaternion(scalar/value, vector/value);}

 inline quaternion operator !() {return quaternion(scalar, -vector);};
 
 inline quaternion operator + (const quaternion &q) {return quaternion(scalar+q.scalar, vector+q.vector);}
 inline quaternion operator - (const quaternion &q) {return quaternion(scalar-q.scalar, vector-q.vector);}
 inline quaternion operator * (const quaternion &q)
  {
   return quaternion(
    scalar * q.scalar - vector.dot(q.vector),                        // SCALAR 
    vector.cross(q.vector) + scalar * q.vector + q.scalar * vector); // VECTOR 
  };
  
 inline format length()
  { return sqrt( sqr(scalar) + sqr(vector.x) + sqr(vector.y) + sqr(vector.z)); }
  
 inline quaternion normalize() {return *this / length();}
 
 //inline vector3<format> operator * (const vector3<format> &v)
 //{return ((*this) * Quaternion(v)).vector;}
  
 inline vector3<format> transform(vector3<format> &v)
  {
   quaternion T = *this;
   return (T * Quaternion(v) * !T).vector;
  }
  
 inline vector3<format> invtransform(vector3<format> &v)
  {
   quaternion T = *this;
   return (!T * quaternion(v) * T).vector;
  }
  
 inline quaternion<format>& operator +=(const quaternion &q)
  { 
   scalar = +q.scalar; 
   vector += q.vector; 
   return *this;
  }
  
 inline quaternion<format>& operator -=(const quaternion &q) 
  {
    scalar = -q.scalar; 
    vector -= q.vector; 
    return *this;
   }
 
 inline matrix4<format> toMatrix()
 {
  format wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
  
  x2 = vector.x + vector.x;
  y2 = vector.y + vector.y;
  z2 = vector.z + vector.z;
  xx = vector.x * x2;   xy = vector.x * y2;   xz = vector.x * z2;
  yy = vector.y * y2;   yz = vector.y * z2;   zz = vector.z * z2;
  wx = scalar   * x2;   wy = scalar   * y2;   wz = scalar   * z2;
  return matrix4<format>(
   vector4<format>(1.0f - (yy+zz),         xy-wz ,         xz+wy , 0.0),
   vector4<format>(        xy+wz , 1.0f - (xx+zz),         yz-wx , 0.0),
   vector4<format>(        xz-wy ,         yz+wx , 1.0f - (xx+yy), 0.0),
   vector4<format>(          0.0 ,           0.0 ,           0.0 , 1.0));
 }
};


}