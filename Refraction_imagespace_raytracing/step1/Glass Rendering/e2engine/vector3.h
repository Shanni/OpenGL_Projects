#pragma once

#include <e2const.h>
#include <vector2.h>

namespace et 
{

template <typename format>
struct vector3
{
 format x, y, z;
 inline vector3(){x=y=z=0;}
 inline vector3(format val){x=y=z=val;}
 inline vector3(vector2<format>_xy, format _z){x = _xy.x; y = _xy.y; z = _z;}
 inline vector3(format _x, format _y) {x=_x; y=_y; z=0;}
 inline vector3(format _x, format _y, format _z) {x=_x; y=_y; z=_z;}
// []
 inline format* raw() { return &x; }
 
 inline format& operator[](int i){return (&x)[i];}
 inline void operator = (format &value) {x=value; y=value; z=value;};

 inline vector3 operator -() {return vector3(-x, -y, -z);};
 inline const vector3 operator -() const {return vector3(-x, -y, -z);};
/////////////////////////////// +, -, *, /

// vector +, -, *, / vector
 inline vector3 operator +(vector3& value){return vector3(x + value.x, y + value.y, z + value.z);};
 inline vector3 operator -(vector3& value){return vector3(x - value.x, y - value.y, z - value.z);};
 inline vector3 operator *(vector3& value){return vector3(x * value.x, y * value.y, z * value.z);};
 inline vector3 operator /(vector3& value){return vector3(x / value.x, y / value.y, z / value.z);};
 inline vector3 operator +(const vector3& value){return vector3(x + value.x, y + value.y, z + value.z);};
 inline vector3 operator -(const vector3& value){return vector3(x - value.x, y - value.y, z - value.z);};
 inline vector3 operator *(const vector3& value){return vector3(x * value.x, y * value.y, z * value.z);};
 inline vector3 operator /(const vector3& value){return vector3(x / value.x, y / value.y, z / value.z);};
// vector +, -, *, / scalar
 inline vector3 operator +(format &value){return vector3(x + value, y + value, z + value);};
 inline vector3 operator -(format &value){return vector3(x - value, y - value, z - value);};
 inline vector3 operator *(format &value){return vector3(x * value, y * value, z * value);};
 inline vector3 operator /(format &value){return vector3(x / value, y / value, z / value);};
 inline vector3 operator +(const format &value){return vector3(x + value, y + value, z + value);};
 inline vector3 operator -(const format &value){return vector3(x - value, y - value, z - value);};
 inline vector3 operator *(const format &value){return vector3(x * value, y * value, z * value);};
 inline vector3 operator /(const format &value){return vector3(x / value, y / value, z / value);};
 inline const vector3 operator +(const format &value) const {return vector3(x + value, y + value, z + value);};
 inline const vector3 operator -(const format &value) const {return vector3(x - value, y - value, z - value);};
 inline const vector3 operator *(const format &value) const {return vector3(x * value, y * value, z * value);};
 inline const vector3 operator /(const format &value) const {return vector3(x / value, y / value, z / value);};

///////////////////////// +=, -=, *=, /=

// vector {+-*/}= vector
 inline vector3& operator +=(const vector3 &value){x+=value.x;y+=value.y;z+=value.z; return *this;};
 inline vector3& operator -=(const vector3 &value){x-=value.x;y-=value.y;z-=value.z; return *this;};
 inline vector3& operator *=(const vector3 &value){x*=value.x;y*=value.y;z*=value.z; return *this;};
 inline vector3& operator /=(const vector3 &value){x/=value.x;y/=value.y;z/=value.z; return *this;};
// vector {+-*/}= scalar
 inline vector3& operator +=(format value){x+=value;y+=value;z+=value; return *this;};
 inline vector3& operator -=(format value){x-=value;y-=value;z-=value; return *this;};
 inline vector3& operator *=(format value){x*=value;y*=value;z*=value; return *this;};
 inline vector3& operator /=(format value){x/=value;y/=value;z/=value; return *this;};

 inline format dot(vector3 &vec) { return x*vec.x + y*vec.y + z*vec.z;}
 inline format dot(const vector3 &vec) { return x*vec.x + y*vec.y + z*vec.z;}
 inline format dotSelf() { return x*x + y*y + z*z; }
 inline format length() { return sqrt( dotSelf() ); }

 vector3 cross(const vector3 &vec) 
  { 
   return vector3( y * vec.z - z * vec.y, 
                   z * vec.x - x * vec.z,
                   x * vec.y - y * vec.x );
  }

 vector3 normalize()
  {
   float len = dot(*this);
   return (len > 0) ? (*this / sqrt(len)) : vector3(0); 
  }
   
 static renderAttributeVector getRA()
 { 
  renderAttributeVector R;
  R.push_back(render_attrib(RENDER_ATTRIB_POSITION, 3));
  return R;
 }
   
};

template <typename format>
 inline vector3<format> operator *(format value, const vector3<format>& vec) 
   {return vector3<format>(vec.x * value, vec.y * value, vec.z * value);}

template <typename format>
 inline vector3<format> operator +(const vector3<format>& v1, vector3<format>& v2) 
   {return vector3<format>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);}

template <typename format>
 inline vector3<format> operator -(const vector3<format>& v1, vector3<format>& v2) 
   {return vector3<format>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);}

template <typename format>
 inline vector3<format> operator -(const vector3<format>& v1, const vector3<format>& v2) 
   {return vector3<format>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);}

}