#pragma once

#include <e2const.h>

namespace et 
{

template <typename format> 
struct vector2
{
 format x, y;
 vector2(){x = y = 0;};
 vector2(format value){x = y = value;}
 vector2(format _x, format _y){x=_x;y=_y;}

 inline bool operator == (const vector2& value) const {return (value.x == x) && (value.y == y);}
 inline bool operator == (vector2& value) const {return (value.x == x) && (value.y == y);}
 
 inline vector2 operator +(vector2& value) {return vector2(x + value.x, y + value.y);};
 inline vector2 operator -(vector2& value) {return vector2(x - value.x, y - value.y);};
 inline vector2 operator *(vector2& value) {return vector2(x * value.x, y * value.y);};
 inline vector2 operator /(vector2& value) {return vector2(x / value.x, y / value.y);};
 inline vector2 operator +(const vector2 &value) {return vector2(x + value.x, y + value.y);};
 inline vector2 operator -(const vector2 &value) {return vector2(x - value.x, y - value.y);};
 inline vector2 operator *(const vector2 &value) {return vector2(x * value.x, y * value.y);};
 inline vector2 operator /(const vector2 &value) {return vector2(x / value.x, y / value.y);};

 inline vector2 operator +(format &value) {return vector2(x + value, y + value);};
 inline vector2 operator -(format &value) {return vector2(x - value, y - value);};
 inline vector2 operator *(format &value) {return vector2(x * value, y * value);};
 inline vector2 operator /(format &value) {return vector2(x / value, y / value);};
 inline vector2 operator +(const format &value) {return vector2(x + value, y + value);};
 inline vector2 operator -(const format &value) {return vector2(x - value, y - value);};
 inline vector2 operator *(const format &value) {return vector2(x * value, y * value);};
 inline vector2 operator /(const format &value) {return vector2(x / value, y / value);};

 inline vector2& operator +=(const vector2 &value) {x+=value.x; y+=value.y; return *this;};
 inline vector2& operator -=(const vector2 &value) {x-=value.x; y-=value.y; return *this;};
 inline vector2& operator *=(const vector2 &value) {x*=value.x; y*=value.y; return *this;};
 inline vector2& operator /=(const vector2 &value) {x/=value.x; y/=value.y; return *this;};

 inline vector2& operator +=(format value) {x+=value; y+=value; return *this;};
 inline vector2& operator -=(format value) {x-=value; y-=value; return *this;};
 inline vector2& operator *=(format value) {x*=value; y*=value; return *this;};
 inline vector2& operator /=(format value) {x/=value; y/=value; return *this;};
 
 inline format length() { return sqrt(x*x + y*y); }
 inline format dot(vector2<format>& v) { return x*v.x + y*v.y; }
 
 inline format square() {return x*y;}

 static renderAttributeVector getRA()
 { 
  renderAttributeVector R;
  R.push_back( render_attrib(RENDER_ATTRIB_POSITION, 2) );
  return R;
 }

};

template <typename format>
 inline vector2<format> operator *(const vector2<format>& vec, format value)
   {return vector2<format>(vec.x * value, vec.y * value);}
template <typename format>
 inline vector2<format> operator *(format value, const vector2<format>& vec) 
   {return vector2<format>(vec.x * value, vec.y * value);}

}