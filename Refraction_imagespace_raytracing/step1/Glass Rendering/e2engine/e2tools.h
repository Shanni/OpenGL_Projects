#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <windows.h>
#include <winbase.h>

#include <gl3/glgen.h>
#include <il/il.h>
#include <e2const.h>

#pragma once

using namespace std;

namespace et
{

inline double QueryTime()
{
 __int64 COUNTER;
 __int64 FREQ;
 QueryPerformanceCounter((LARGE_INTEGER*)&COUNTER);
 QueryPerformanceFrequency((LARGE_INTEGER*)&FREQ);
 return double(COUNTER) / double(FREQ);
} 
 
template <class STL_container> class each
{
 public:
  typedef STL_container container_type;
  typedef typename container_type::value_type value_type;
  each(container_type& i) : m_i(i.begin()), m_end(i.end()) {}
  operator bool () const { return m_i != m_end; }
  each& operator ++ () { ++m_i; return *this; }
  value_type& operator * () const { return  *m_i; }
  value_type* operator ->() const { return &*m_i; }
 private:
  typename container_type::iterator m_i;
  typename container_type::iterator m_end;
};

inline void trim(string &str)
{
 if (str == "") 
  return;

 while ((str[0] == 32)||(str[0] == 13)||(str[0] == 10)||(str[0] == 9)) str.erase(0, 1);
 
 int nLen = (str.length() - 1);
 while ((str[nLen] == 32)||(str[nLen] == 13)||(str[nLen] == 10)||(str[nLen] == 9)) 
 {
  str.erase(nLen, 1);
  nLen = (str.length() - 1);
 } 
}

inline void lowercase(string &str)
{
 for (unsigned int i = 0; i < str.length(); i++)
  str[i] = char(tolower(str[i]));
}

inline bool fileExists(std::string name)
{
 WIN32_FIND_DATAA F = {};
 HANDLE H = FindFirstFileA(name.c_str(), &F);
 FindClose(H);
 return (H != INVALID_HANDLE_VALUE);
}

inline string GetFilePath(string name)
{
 size_t P;
 return ( (P = name.find_last_of('\\')) == string::npos) ? "" : name.substr(0, ++P);
}

inline string getFileName(const string& fullPath)
{
 size_t p = fullPath.find_last_of('\\');
 return (p  == string::npos) ? fullPath : fullPath.substr(p + 1);
}

inline string RemoveUpDir(string &name)
{
 string R = name;
 unsigned int DotPos;
 if ( (DotPos = name.find("..")) != string::npos)
 {
  R  = GetFilePath( name.substr(0, DotPos - 1) );
  R += name.substr(DotPos + 3);
 }
 return R;
}


inline string GetFileExt(string name)
{
 size_t DP;
 return ((DP = name.find_last_of('.')) == string::npos) ? "" : name.substr(++DP);
}

inline int strToInt(const std::string& value)
{
 return atoi(value.c_str());
}

inline string intToStr(int value)
{ 
 char buffer[16] = {0};
 _itoa_s(value, buffer, 16, 10);
 return buffer;
}

inline string floatToStr(float value, std::streamsize precission = 5)
{ 
 stringstream s;
 s << setprecision(precission) << value;
 return s.str();
}

inline string floatToTimeStr(float value)
{
 float fract = value - int(value);

 int as_seconds = int(value) + int(fract > 0.5);

 string seconds_str = intToStr(as_seconds % 60);

 if (seconds_str.size() == 1)
  seconds_str = "0" + seconds_str;

 return intToStr(as_seconds / 60) + ":" + seconds_str;
}

inline string glTexTargetToString(int target)
{
 switch (target)
 {
  case GL_TEXTURE_1D : return "GL_TEXTURE_1D";
  case GL_TEXTURE_2D : return "GL_TEXTURE_2D";
  case GL_TEXTURE_3D : return "GL_TEXTURE_3D";
  case GL_TEXTURE_CUBE_MAP_POSITIVE_X  : return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_X  : return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Y  : return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y  : return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Z  : return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  : return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
  case GL_TEXTURE_2D_ARRAY             : return "GL_TEXTURE_2D_ARRAY";
  case GL_TEXTURE_CUBE_MAP_ARRAY       : return "GL_TEXTURE_CUBE_MAP_ARRAY";
  case GL_TEXTURE_RECTANGLE            : return "GL_TEXTURE_RECTANGLE";
  case GL_TEXTURE_2D_MULTISAMPLE       : return "TEXTURE_2D_MULTISAMPLE";
  case GL_TEXTURE_2D_MULTISAMPLE_ARRAY : return "TEXTURE_2D_MULTISAMPLE_ARRAY";
  case GL_TEXTURE_BUFFER               : return "TEXTURE_BUFFER_ARB";
 }
 string res = "UNKNOWN TEXTURE TARGET";
 res += intToStr(target);
 return res;
}

inline string glInternalFormatToString(int format)
{
 switch (format)
 {
/* 
  case GL_ALPHA   : return "GL_ALPHA";
  case GL_ALPHA4  : return "GL_ALPHA4";
  case GL_ALPHA8  : return "GL_ALPHA8";
  case GL_ALPHA12 : return "GL_ALPHA12";
  case GL_ALPHA16 : return "GL_ALPHA16";
/*  
  case GL_COMPRESSED_ALPHA           : return "GL_COMPRESSED_ALPHA";
  case GL_COMPRESSED_LUMINANCE       : return "GL_COMPRESSED_LUMINANCE";
  case GL_COMPRESSED_LUMINANCE_ALPHA : return "GL_COMPRESSED_LUMINANCE_ALPHA";
  case GL_COMPRESSED_INTENSITY       : return "GL_COMPRESSED_INTENSITY";
  case GL_COMPRESSED_RGB             : return "GL_COMPRESSED_RGB";
  case GL_COMPRESSED_RGBA            : return "GL_COMPRESSED_RGBA";
  case GL_LUMINANCE   : return "GL_LUMINANCE";
  case GL_LUMINANCE4  : return "GL_LUMINANCE4";
  case GL_LUMINANCE8  : return "GL_LUMINANCE8";
  case GL_LUMINANCE12 : return "GL_LUMINANCE12";
  case GL_LUMINANCE16 : return "GL_LUMINANCE16";
  case GL_LUMINANCE_ALPHA     : return "GL_LUMINANCE_ALPHA";
  case GL_LUMINANCE4_ALPHA4   : return "GL_LUMINANCE4_ALPHA4";
  case GL_LUMINANCE6_ALPHA2   : return "GL_LUMINANCE6_ALPHA2";
  case GL_LUMINANCE8_ALPHA8   : return "GL_LUMINANCE8_ALPHA8";
  case GL_LUMINANCE12_ALPHA4  : return "GL_LUMINANCE12_ALPHA4";
  case GL_LUMINANCE12_ALPHA12 : return "GL_LUMINANCE12_ALPHA12";
  case GL_LUMINANCE16_ALPHA16 : return "GL_LUMINANCE16_ALPHA16";
  case GL_INTENSITY   : return "GL_INTENSITY";
  case GL_INTENSITY4  : return "GL_INTENSITY4";
  case GL_INTENSITY8  : return "GL_INTENSITY8";
  case GL_INTENSITY12 : return "GL_INTENSITY12";
  case GL_INTENSITY16 : return "GL_INTENSITY16";
*/  
  case GL_DEPTH_COMPONENT    : return "GL_DEPTH_COMPONENT";
  case GL_DEPTH_COMPONENT16  : return "GL_DEPTH_COMPONENT16";
  case GL_DEPTH_COMPONENT24  : return "GL_DEPTH_COMPONENT24";
  case GL_DEPTH_COMPONENT32  : return "GL_DEPTH_COMPONENT32";
  
  case GL_R3_G3_B2 : return "GL_R3_G3_B2";
  case GL_RGB      : return "GL_RGB";
  case GL_BGR      : return "GL_BGR";
  case GL_RGB4     : return "GL_RGB4";
  case GL_RGB5     : return "GL_RGB5";
  case GL_RGB8     : return "GL_RGB8";
  case GL_RGB10    : return "GL_RGB10";
  case GL_RGB12    : return "GL_RGB12";
  case GL_RGB16    : return "GL_RGB16";
  case GL_RGBA     : return "GL_RGBA";
  case GL_BGRA     : return "GL_BGRA";
  case GL_RGBA2    : return "GL_RGBA2";
  case GL_RGBA4    : return "GL_RGBA4";
  case GL_RGB5_A1  : return "GL_RGB5_A1";
  case GL_RGBA8    : return "GL_RGBA8";
  case GL_RGB10_A2 : return "GL_RGB10_A2";
  case GL_RGBA12   : return "GL_RGBA12";
  case GL_RGBA16   : return "GL_RGBA16";
/*  
  case GL_SLUMINANCE         : return "GL_SLUMINANCE";
  case GL_SLUMINANCE8        : return "GL_SLUMINANCE8";
  case GL_SLUMINANCE_ALPHA   : return "GL_SLUMINANCE_ALPHA";
  case GL_SLUMINANCE8_ALPHA8 : return "GL_SLUMINANCE8_ALPHA8";
*/  
  case GL_DEPTH_COMPONENT32F : return "GL_DEPTH_COMPONENT32F";
  case GL_SRGB               : return "GL_SRGB";
  case GL_SRGB8              : return "GL_SRGB8";
  case GL_SRGB_ALPHA         : return "GL_SRGB_ALPHA ";
  case GL_SRGB8_ALPHA8       : return "GL_SRGB8_ALPHA8";
  case GL_RGB16F             : return "GL_RGB16F";
  case GL_RGBA16F            : return "GL_RGBA16F";
  case GL_RGBA32F            : return "GL_RGBA32F_ARB ";
  case GL_RGB32F             : return "GL_RGB32F_ARB";
  case GL_R11F_G11F_B10F     : return "GL_R11F_G11F_B10F_EXT";
/*  
  case GL_ALPHA16F           : return "GL_ALPHA16F_ARB";
  case GL_INTENSITY16F       : return "GL_INTENSITY16F_ARB";
  case GL_LUMINANCE16F       : return "GL_LUMINANCE16F_ARB";
  case GL_LUMINANCE_ALPHA16F : return "GL_LUMINANCE_ALPHA16F_ARB";
  case GL_ALPHA32F           : return "GL_ALPHA32F_ARB";
  case GL_INTENSITY32F       : return "GL_INTENSITY32F_ARB";
  case GL_LUMINANCE32F       : return "GL_LUMINANCE32F_ARB";
  case GL_LUMINANCE_ALPHA32F : return "GL_LUMINANCE_ALPHA32F_ARB";
*/  
 }

 string res = "UNKNOWN FORMAT #" ;
 res += intToStr(format);
 return res;
}

inline string glTypeToString(int type)
{
 switch (type)
 {
  case GL_UNSIGNED_BYTE : return "GL_UNSIGNED_BYTE";
  case GL_BYTE          : return "GL_BYTE";
  case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
  case GL_SHORT         : return "GL_SHORT";
  case GL_UNSIGNED_INT  : return "GL_UNSIGNED_INT";
  case GL_INT           : return "GL_INT";
  case GL_FLOAT         : return "GL_FLOAT";
  case GL_HALF_FLOAT    : return "GL_HALF_FLOAT";
 }
 string res = "UNKNOWN TYPE #" ;
 res += intToStr(type);
 return res;
}

inline string glErrorToString(int error)
{
 switch (error)
 {
  case GL_NO_ERROR          : return "GL_NO_ERROR"; 
  case GL_INVALID_ENUM      : return "GL_INVALID_ENUM"; 
  case GL_INVALID_VALUE     : return "GL_INVALID_VALUE"; 
  case GL_INVALID_OPERATION : return "GL_INVALID_OPERATION"; 
  case GL_OUT_OF_MEMORY     : return "GL_OUT_OF_MEMORY"; 
  case GL_ERROR_INVALID_VERSION_ARB : return "GL_ERROR_INVALID_VERSION_ARB";
  case GL_ERROR_INVALID_PROFILE_ARB : return "GL_ERROR_INVALID_PROFILE_ARB";
//  case GL_STACK_OVERFLOW    : return "GL_STACK_OVERFLOW"; 
//  case GL_STACK_UNDERFLOW   : return "GL_STACK_UNDERFLOW"; 
 }

 string res = "UNKNOWN OPENGL ERROR #" ;
 res += intToStr(error);
 return res;
}

inline string ilErrorToString(int error)
{
 switch (error)
 {
  case IL_NO_ERROR             : return "IL_NO_ERROR";
  case IL_INVALID_ENUM         : return "IL_INVALID_ENUM";
  case IL_OUT_OF_MEMORY        : return "IL_OUT_OF_MEMORY";
  case IL_FORMAT_NOT_SUPPORTED : return "IL_FORMAT_NOT_SUPPORTED";
  case IL_INTERNAL_ERROR       : return "IL_INTERNAL_ERROR";
  case IL_INVALID_VALUE        : return "IL_INVALID_VALUE";
  case IL_ILLEGAL_OPERATION    : return "IL_ILLEGAL_OPERATION";
  case IL_ILLEGAL_FILE_VALUE   : return "IL_ILLEGAL_FILE_VALUE";
  case IL_INVALID_FILE_HEADER  : return "IL_INVALID_FILE_HEADER";
  case IL_INVALID_PARAM        : return "IL_INVALID_PARAM";
  case IL_COULD_NOT_OPEN_FILE  : return "IL_COULD_NOT_OPEN_FILE";
  case IL_INVALID_EXTENSION    : return "IL_INVALID_EXTENSION";
  case IL_FILE_ALREADY_EXISTS  : return "IL_FILE_ALREADY_EXISTS";
  case IL_OUT_FORMAT_SAME      : return "IL_OUT_FORMAT_SAME";
  case IL_STACK_OVERFLOW       : return "IL_STACK_OVERFLOW";
  case IL_STACK_UNDERFLOW      : return "IL_STACK_UNDERFLOW";
  case IL_INVALID_CONVERSION   : return "IL_INVALID_CONVERSION";
  case IL_LIB_GIF_ERROR        : return "IL_LIB_GIF_ERROR";
  case IL_LIB_JPEG_ERROR       : return "IL_LIB_JPEG_ERROR";
  case IL_LIB_PNG_ERROR        : return "IL_LIB_PNG_ERROR";
  case IL_LIB_TIFF_ERROR       : return "IL_LIB_TIFF_ERROR";
  case IL_LIB_MNG_ERROR        : return "IL_LIB_MNG_ERROR";
  case IL_UNKNOWN_ERROR        : return "IL_UNKNOWN_ERROR";
 }
 return "UNKNOWN IL ERROR";
}      

inline string FramebufferStatusToString(int status)
{
 switch (status)
 {
  case GL_FRAMEBUFFER_COMPLETE                      : return "GL_FRAMEBUFFER_COMPLETE"; 
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT         : return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; 
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT : return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; 
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER        : return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; 
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER        : return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; 
//  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS         : return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS"; 
//  case GL_FRAMEBUFFER_INCOMPLETE_UNSUPPORTED        : return "GL_FRAMEBUFFER_INCOMPLETE_UNSUPPORTED"; 
 }
 return "Unknown FBO status " + intToStr(status);
}

inline unsigned int fileSize(ifstream& file)
{
 file.seekg(0, std::ios::end);
 ifstream::pos_type end = file.tellg();
 file.seekg(0, std::ios::beg);
 return (unsigned int)(end - file.tellg());
}

__forceinline void sincos(float a, float &s, float &c) 
{ 
  _asm
  { 
    fld a
    fsincos
    fstp dword ptr [c]
    fstp dword ptr [s]
    fwait
  }
}

inline unsigned int roundToHighestPowerOfTow(unsigned int x)
{
 x = x - 1;
 x |= (x >> 1);
 x |= (x >> 2);
 x |= (x >> 4);
 x |= (x >> 8);
 x |= (x >> 16);
 return x + 1;
}

}