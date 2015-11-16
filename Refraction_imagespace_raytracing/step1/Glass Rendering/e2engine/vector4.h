#pragma once

#include <e2const.h>
#include <vector3.h>

namespace et 
{

template <typename format>
struct vector4
{
 format x, y, z, w;
 inline format* raw() { return &x; }
 inline const format* const_raw() const {return &x;}
 
 inline vector4(){}
 inline vector4(format s) {x = y = z = w = s;}
 inline vector4(vector2<format>v1, vector2<format>v2) {x=v1.x;y=v1.y;z=v2.x;w=v2.y;}
 inline vector4(vector3<format> v3, format _w) {x = v3.x; y=v3.y; z=v3.z; w=_w;}
 inline vector4(vector3<format> v3) {x = v3.x; y=v3.y; z=v3.z; w=0;}
 inline vector4(format _x, format _y, format _z, format _w){x=_x;y=_y;z=_z;w=_w;}
 inline format& operator[](int i){return *(&x+i);}
 
 inline vector4 operator -() { return vector4<format>(-x, -y, -z, -w); }
 inline const vector4 operator -() const { return vector4<format>(-x, -y, -z, -w); }
 
 inline vector4 operator + (vector4& value) {return vector4(x + value.x, y + value.y, z + value.z, w + value.w);};
 inline vector4 operator - (vector4& value) {return vector4(x - value.x, y - value.y, z - value.z, w - value.w);};
 inline vector4 operator * (vector4& value) {return vector4(x * value.x, y * value.y, z * value.z, w * value.w);};
 inline vector4 operator / (vector4& value) {return vector4(x / value.x, y / value.y, z / value.z, w / value.w);};
 inline vector4 operator + (const vector4& value) {return vector4(x + value.x, y + value.y, z + value.z, w + value.w);};
 inline vector4 operator - (const vector4& value) {return vector4(x - value.x, y - value.y, z - value.z, w - value.w);};
 inline vector4 operator * (const vector4& value) {return vector4(x * value.x, y * value.y, z * value.z, w * value.w);};
 inline vector4 operator / (const vector4& value) {return vector4(x / value.x, y / value.y, z / value.z, w / value.w);};

 inline vector4 operator + (format& value) {return vector4(x + value, y + value, z + value, w + value);};
 inline vector4 operator - (format& value) {return vector4(x - value, y - value, z - value, w - value);};
 inline vector4 operator * (format& value) {return vector4(x * value, y * value, z * value, w * value);};
 inline vector4 operator / (format& value) {return vector4(x / value, y / value, z / value, w / value);};
 inline vector4 operator + (const format& value) {return vector4(x + value, y + value, z + value, w + value);};
 inline vector4 operator - (const format& value) {return vector4(x - value, y - value, z - value, w - value);};
 inline vector4 operator * (const format& value) {return vector4(x * value, y * value, z * value, w * value);};
 inline vector4 operator / (const format& value) {return vector4(x / value, y / value, z / value, w / value);};

 inline vector4& operator += (const vector4 &value) {x+=value.x; y+=value.y; z+=value.z; w+=value.w; return *this;};
 inline vector4& operator -= (const vector4 &value) {x-=value.x; y-=value.y; z-=value.z; w-=value.w; return *this;};
 inline vector4& operator *= (const vector4 &value) {x*=value.x; y*=value.y; z*=value.z; w*=value.w; return *this;};
 inline vector4& operator /= (const vector4 &value) {x/=value.x; y/=value.y; z/=value.z; w/=value.w; return *this;};
/* 
 inline vector4& operator += (format value) {x+=value; y+=value; z+=value; w+=value; return *this;};
 inline vector4& operator -= (format value) {x-=value; y-=value; z-=value; w-=value; return *this;};
 inline vector4& operator *= (format value) {x*=value; y*=value; z*=value; w*=value; return *this;};
 inline vector4& operator /= (format value) {x/=value; y/=value; z/=value; w/=value; return *this;};
*/ 
 inline vector3<format>& xyz(){return (vector3<format>&)x;}
 
 inline format dot(vector4<format>& vector) 
  {
   return x * vector.x + 
          y * vector.y + 
          z * vector.z + 
          w * vector.w;
  }
 
 static renderAttributeVector getRA()
 { 
  renderAttributeVector R;
  R.push_back(render_attrib(RENDER_ATTRIB_POSITION, 4));
  return R;
 }
 
};

template <typename format>
 inline vector4<format> operator * (format value, const vector4<format>& vec) 
   {return vector4<format>(vec.x * value, vec.y * value, vec.z * value, vec.w * value);}

}