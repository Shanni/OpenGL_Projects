#include <e2core.h>
#include <e2manager.h>
#include <e2tools.h>
#include <e2const.h>

using namespace et;

Ce2ResourceManager * Ce2ResourceManager::Instance;

Ce2ResourceManager::Ce2ResourceManager()
{
 Instance = this;

 const string& appPath = core.appPath;
 
 addSearchPath(appPath);
 addSearchPath(appPath + "shaders\\" );
 addSearchPath(appPath + "models\\" );

 addSearchPath(appPath);
 addSearchPath(appPath + "..\\data\\" );
 addSearchPath(appPath + "..\\data\\shaders\\" );
 addSearchPath(appPath + "..\\data\\textures\\" );
 addSearchPath(appPath + "..\\data\\models\\" );

 addSearchPath("E:\\Delphi components\\MyEngine2\\");
 addSearchPath("E:\\Delphi components\\MyEngine2\\shaders\\");
 addSearchPath("E:\\Projects\\_include\\e2engine\\");
 addSearchPath("E:\\Projects\\_include\\e2engine\\shaders\\");

 ilInit();
 InitDefaultTextures();
}

void Ce2ResourceManager::checkError(string info)
{
 Ce2Render::instance->checkError(info);
}

void Ce2ResourceManager::checkILError(string info)
{
 ILenum error = ilGetError();
 if (error == IL_NO_ERROR) return;
 
 log(ilErrorToString(error) + " at " + info);
}

string Ce2ResourceManager::findFile(std::string name)
{
 if (fileExists(name)) return name;

 string path = RemoveUpDir(name);
 if (fileExists(path)) return path;
  
 for (unsigned short i = 0; i < manager()->resource_path.size(); i++)
 {
  path  = resource_path[i];
  path += name;
  if (fileExists(path)) return path;
 }

 return "";
}

Ce2TextureObject* Ce2ResourceManager::genTexture2D(string name, int nWidth, int nHeight, int nInternalFormat, int nFormat, int nType, void *data)
{
 Ce2TextureObject * tex = new Ce2TextureObject(name);
 
 tex->format = nFormat;
 tex->internalFormat = nInternalFormat;
 tex->width  = nWidth;
 tex->height = nHeight;
 tex->type   = nType;
 render()->buildTexture(tex, GL_TEXTURE_2D, data);
 
 all.push_back(tex);
 return tex;
}

Ce2TextureObject* Ce2ResourceManager::genCubeTexture(string name, int nWidth, int nHeight, int nInternalFormat, int nFormat, int nType, void* data[6])
{
 Ce2TextureObject* tex = new Ce2TextureObject(name);
 
 tex->format = nFormat;
 tex->internalFormat = nInternalFormat;
 tex->width  = nWidth;
 tex->height = nHeight;
 tex->type   = nType;
 render()->buildCubeTexture(tex, data);
 
 all.push_back(tex);
 return tex;
}

Ce2TextureObject* Ce2ResourceManager::genNoiseTexture2D(std::string name, int nWidth, int nHeight, bool bNormalize)
{
 int nDataSize = nWidth * nHeight * 3;
 
 unsigned char * data = new unsigned char [nDataSize];
 
 for (int i = 0; i < nDataSize / 3; i++)
 {
  vec3ub V = vec3f_to_3ub( bNormalize ? randVector().normalize() : randVector() );
  data[3*i+0] = V.x;
  data[3*i+1] = V.y;
  data[3*i+2] = V.z;
 }
 Ce2TextureObject* R = genTexture2D(name, nWidth, nHeight, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, data);
 
 delete [] data;
 
 return R;
}

void Ce2ResourceManager::InitDefaultTextures()
{
 byte data[4];
 data[0] = 0;    data[1] = 0; data[2]    = 0; data[3] = 255;
 DEFTEX_black  = genTexture2D("DEFTEX BLACK",  1, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);
 data[0] = 255; data[1] = 255; data[2] = 255; data[3] = 255;
 DEFTEX_white  = genTexture2D("DEFTEX WHITE",  1, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);
 data[0] = 128; data[1] = 128; data[2] = 255; data[3] = 255;
 DEFTEX_normal = genTexture2D("DEFTEX NORMAL", 1, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);
 
 byte cell[16] = {0};
 cell[ 0] = 255; cell[ 1] =   0; cell[ 2] = 0; cell[ 3] = 255;
 cell[ 4] =   0; cell[ 5] = 255; cell[ 6] = 0; cell[ 7] = 255;
 cell[ 8] =   0; cell[ 9] = 255; cell[10] = 0; cell[11] = 255;
 cell[12] = 255; cell[13] =   0; cell[14] = 0; cell[15] = 255;
 DEFTEX_cell = genTexture2D("DEFTEX CELL", 2, 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, cell);
 
 DEFTEX_noise       = manager()->genNoiseTexture2D("DEFTEX NOISE",        256, 256, false);
 DEFTEX_normalnoise = manager()->genNoiseTexture2D("DEFTEX NORMAL NOISE",   4,   4, true);
 
 checkError("Ce2ResourceManager::InitDefaultTextures()");
}

void Ce2ResourceManager::Cleanup()
{
 checkError("Ce2ResourceManager::Cleanup()");
 
 ResourceMap::iterator i = all.begin();
 while (all.size() > 0)
 {
   Unload<Ce2Resource>( *all.rbegin() ); 
 } 
}

void Ce2ResourceManager::log(string info)
{
 core.log("MANAGER: " + info, true);
}

/////////////////////////////////////////////////////////////////////////////////////
//////////////////  R E S O U R C E S  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/// TEXTURE
bool Ce2TextureObject::load(const string& filename, const string& params)
{
 ILuint _IM;
 ilGenImages(1, &_IM);
 manager()->checkILError("ilGenImages : " + filename);

 ilBindImage(_IM);
 ilEnable(IL_ORIGIN_SET);
 ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
 ilLoadImage(filename.c_str());
 
 manager()->checkILError("ilLoadImage : " + filename);
 
 height = ilGetInteger(IL_IMAGE_HEIGHT);
 width = ilGetInteger(IL_IMAGE_WIDTH);
 type = ilGetInteger(IL_IMAGE_TYPE);
 nBPP = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
 
 internalFormat = ilGetInteger(IL_IMAGE_FORMAT);
 
 if (internalFormat == IL_BGR)
  ilConvertImage(IL_RGB, type);
 if (internalFormat == IL_BGRA)
  ilConvertImage(IL_RGBA, type);
  
 internalFormat = ilGetInteger(IL_IMAGE_FORMAT);

 format = GL_RGBA;  
 switch (nBPP)
 {
  case 24: 
  case 96: 
  case 48: 
   format = GL_RGB;
 }

 if ((nBPP == 128)&&(format == GL_RGBA)) internalFormat = GL_RGBA32F;
 if ((nBPP ==  64)&&(format == GL_RGBA)) internalFormat = GL_RGBA16F;
 if ((nBPP ==  96)&&(format == GL_RGB))  internalFormat = GL_RGB32F;
 if ((nBPP ==  48)&&(format == GL_RGB))  internalFormat = GL_RGB16F;
 
 render()->buildTexture(this, GL_TEXTURE_2D, ilGetData());
 manager()->checkILError("ilGetData : " + filename);
 
 ilDeleteImages(1, &_IM);
 manager()->checkILError("ilDeleteImages : " + filename);

 return true;
}

bool Ce2TextureObject::unload()
{
 render()->unloadTexture(this);
 return true;
}

void Ce2TextureObject::setWrap(GLenum S, GLenum T, GLenum R)
{
 render()->bindTexture(0, this);
 glTexParameteri(target, GL_TEXTURE_WRAP_S, S); render()->checkErrorF("glTexParameteri<WRAP_S>", name); 
 glTexParameteri(target, GL_TEXTURE_WRAP_T, T); render()->checkErrorF("glTexParameteri<WRAP_T>", name); 
 glTexParameteri(target, GL_TEXTURE_WRAP_R, R); render()->checkErrorF("glTexParameteri<WRAP_R>", name); 
}

void Ce2TextureObject::setFiltration(GLenum min_f, GLenum mag_f)
{
 render()->bindTexture(0, this);
 glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min_f); render()->checkErrorF("glTexParameteri<GL_TEXTURE_MIN_FILTER>", name); 
 glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag_f); render()->checkErrorF("glTexParameteri<GL_TEXTURE_MAG_FILTER>", name); 
}

void Ce2TextureObject::compareRefToTexture(bool enable, GLenum compareFunc)
{
 render()->bindTexture(16, this);
 if (enable)
 {
  glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, compareFunc);
 }
 else
 {
  glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
 }
}

string LoadFile(string const& f_name)
{
 string ret;
 
 ifstream file(f_name.c_str(), ios::beg | ios::binary);
 ret.resize( fileSize(file) );
 
  file.read( (char*)ret.data(), ret.size() );
  
 file.close();
 
 return ret;
}

/// PROGRAM OBJECT
bool Ce2ProgramObject::load(const string& filename, const string& params)
{
 _defines = params;

 string vertex_source;
 string geometry_source;
 string fragment_source;
 
 ifstream progFile(filename.c_str(), ios::binary);
 
 string s;
 
 while (!progFile.eof())
 {
  getline(progFile, s);
  trim(s);
 
  string id = s.substr(0, s.find(':'));
  trim(id); 
  lowercase(id);
  
  if (id == "vs")
   vertex_source   = s.substr(s.find_first_of(':') + 1);
  if (id == "gs") 
   geometry_source = s.substr(s.find_first_of(':') + 1);
  if (id == "fs") 
   fragment_source = s.substr(s.find_first_of(':') + 1);
  if (id == "defines") 
   _defines += ", " + s.substr(s.find_first_of(':') + 1);
 } 
 progFile.close();

 trim(_defines);
 
 trim(vertex_source);
 trim(geometry_source);
 trim(fragment_source);
 
 string vertex_shader;
 string geom_shader = "none";
 string frag_shader;

 _prog_source = filename;
 _prog_folder = GetFilePath(filename);
 
// parse uniforms 
 Uniforms.clear();
 
 string fName = _prog_folder + vertex_source;
 if (!fileExists(fName)) fName = manager()->findFile(fName);
 if (!fileExists(fName)) fName = manager()->findFile(vertex_source);
 if ( fileExists(fName))
 { 
  vertex_shader = LoadFile(fName);
  parse(vertex_shader);
 } 
 else vertex_shader = DEFAULT_VERTEXSHADER;
 
#ifdef LOG_SHADERS
 string ofile = "output\\" + getFileName(filename) + "_VS.txt";
 ofstream o1(ofile);
 o1 << vertex_shader;
 o1.close();
#endif

 if (geometry_source != "none")
 {
  fName = _prog_folder + geometry_source;
  if (!fileExists(fName)) fName = manager()->findFile(fName);
  if (!fileExists(fName)) fName = manager()->findFile(geometry_source);
  if ( fileExists(fName)) 
  {
   geom_shader = LoadFile(fName);
   parse(geom_shader);
  } 
}

#ifdef LOG_SHADERS
 ofile = "output\\" + getFileName(filename) + "_GS.txt";
 ofstream o2(ofile);
 o2 << geom_shader;
 o2.close();
#endif 
 
 fName = _prog_folder + fragment_source;
 if (!fileExists(fName)) fName = manager()->findFile(fName);
 if (!fileExists(fName)) fName = manager()->findFile(fragment_source);
 if ( fileExists(fName)) 
 {
  frag_shader = LoadFile(fName);
  parse(frag_shader);
 } 
 else frag_shader = DEFAULT_FRAGMENTSHADER;

#ifdef LOG_SHADERS
 ofile = "output\\" + getFileName(filename) + "_FS.txt";
 ofstream o3(ofile);
 o3 << frag_shader;
 o3.close();
#endif 
 
 render()->buildProgram(this, vertex_shader, geom_shader, frag_shader);

 return true;
}

bool Ce2ProgramObject::unload()
{
 Uniforms.clear();
 render()->unloadProgram(this);
 return true; 
}

void Ce2ProgramObject::bind()
{
 render()->bindProgram(this);
}

void Ce2ProgramObject::parse(string &source)
{
 string header = "#version 150\nprecision highp float;\n";
 
 string defines = _defines;
 
 int ip;
 while ((ip = defines.find(',')) != string::npos)
 {
  string define = defines.substr(0, ip);
  trim(define);
  if (define.length() > 0)
  {
   header += "\n#define " + define;
  }
  defines.erase(0, ip + 1);
  trim(defines);
 }

 if (defines.length() > 0)
 {
  header += "\n#define " + defines;
 }

 source = header + "\n" + source;

 ip;
 while ( (ip = source.find("#include")) != string::npos )
 {
  string before = source.substr(0, ip);
  
  source.erase(0, before.size());
   string ifname = source.substr(0, source.find_first_of(char(10)));
   string include_name = ifname;
  source.erase(0, ifname.size());
  string after = source.substr();

  if (ifname.find_first_of('"') != -1)
  {
   ifname.erase(0, ifname.find_first_of('"') + 1);
   ifname.erase(ifname.find_last_of('"'));
  }
  else 
  {
   ifname.erase(0, ifname.find_first_of('<') + 1);
   ifname.erase(ifname.find_last_of('>'));
  }
  
  string inc = "";
  
  if (fileExists(_prog_folder + ifname))
    inc = LoadFile(_prog_folder + ifname);
  else
  {
   ifname = manager()->findFile(ifname);
   if (fileExists(ifname))
    inc = LoadFile(ifname);
   else
    manager()->log("Failed to include " + include_name); 
  }
  
  source = before + inc + after;
 }
}

UniformIterator Ce2ProgramObject::FindUniform(string name)
{
 return Uniforms.find(name);
}

void Ce2ProgramObject::_uniform(string name, void *value, int nCount)
{
 UniformIterator U = FindUniform(name);
 if (U == Uniforms.end()) 
 {
  render()->log("Set missed uniform " + name + " for " + this->name);
  return;
 }
 
 int nLoc = U->second.nLocation;
 switch(U->second.nType)
 {
  case GL_FLOAT      : glUniform1fv(nLoc, nCount, (GLfloat*)value); break;
  case GL_FLOAT_VEC2 : glUniform2fv(nLoc, nCount, (GLfloat*)value); break;
  case GL_FLOAT_VEC3 : glUniform3fv(nLoc, nCount, (GLfloat*)value); break;
  case GL_FLOAT_VEC4 : glUniform4fv(nLoc, nCount, (GLfloat*)value); break;
  case GL_FLOAT_MAT2 : glUniformMatrix2fv(nLoc, nCount, false, (GLfloat*)value); break;
  case GL_FLOAT_MAT3 : glUniformMatrix3fv(nLoc, nCount, false, (GLfloat*)value); break;
  case GL_FLOAT_MAT4 : glUniformMatrix4fv(nLoc, nCount, false, (GLfloat*)value); break;
  case GL_SAMPLER_1D:
  case GL_SAMPLER_1D_ARRAY:
  case GL_SAMPLER_1D_ARRAY_SHADOW:
  case GL_SAMPLER_1D_SHADOW:
  case GL_SAMPLER_2D:
  case GL_SAMPLER_2D_ARRAY:
  case GL_SAMPLER_2D_ARRAY_SHADOW:
  case GL_SAMPLER_2D_SHADOW:
  case GL_SAMPLER_2D_MULTISAMPLE:
  case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
  case GL_SAMPLER_2D_RECT:
  case GL_SAMPLER_2D_RECT_SHADOW:
  case GL_SAMPLER_3D:
  case GL_SAMPLER_BUFFER:
  case GL_SAMPLER_CUBE:
  case GL_SAMPLER_CUBE_MAP_ARRAY:
  case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
  case GL_SAMPLER_CUBE_SHADOW : glUniform1iv(nLoc, 1, (GLint*)value); break;
 }
 string info = "SetUniform(" + name + ")";
 render()->checkError(info);
}

void Ce2ProgramObject::setUniformTextures(int n, string names[], int values[])
{
 bind();
 for (int i = 0; i < n; i++)
  _uniform(names[i], &values[i]);
}

void Ce2ProgramObject::setModelViewMatrix(const mat4& m)
{
 if (_mvm_loc < 0) return;
 glUniformMatrix4fv(_mvm_loc, 1, false, (GLfloat*)&m);
 render()->checkError("glUniformMatrix4fv");
}

void Ce2ProgramObject::setMVPMatrix(const mat4& m)
{
 if (_mvp_loc < 0) return;
 glUniformMatrix4fv(_mvp_loc, 1, false, (GLfloat*)&m);
 render()->checkError("glUniformMatrix4fv");
}

void Ce2ProgramObject::setCameraPosition(const vec3& p)
{                 
 if (_cam_loc < 0) return;
 glUniform3fv(_cam_loc, 1, (GLfloat*)&p);
 render()->checkError("SetCameraPosition");
}

void Ce2ProgramObject::setPrimaryLightPosition(const vec3 &p)
{
 if (_l0_loc < 0) return;
 glUniform3fv(_l0_loc, 1, (GLfloat*)&p);
 render()->checkError("SetPrimaryLightPosition");
}

GLint Ce2ProgramObject::GetUniformLocation(string uniform)
{
 UniformIterator I = FindUniform(uniform);
 if (I == Uniforms.end())
  return -1;
 else 
  return I->second.nLocation;
}

void Ce2ProgramObject::setLightProjectionMatrix(const mat4& m)
{
 if (_lp_loc < 0) return;
 
 glUniformMatrix4fv(_lp_loc, 1, false, (GLfloat*)&m);
 render()->checkError("glUniformMatrix4fv");
}

void Ce2ProgramObject::setCameraPropsFromRender()
{
 setMVPMatrix(render()->camera->mvpMatrix());
 setCameraPosition(render()->camera->position());
}