#include <hash_map>
#include <vector>
#include <gl3/glgen.h>
#include <il/il.h>
#include <e2const.h>
#include <e2geometry.h>

#pragma once

#define LOG_SHADERS

static int _resID = 0;

using namespace std;

namespace et

{

class Ce2Resource
{
 public:
  int ID;
  string name;
  inline Ce2Resource(string & _name)
   {
    ID   = _resID++;
    name = _name;
   }
  virtual ~Ce2Resource(){_resID--;}; 
  virtual bool load(const string& filename, const string& params) {return true;};
  virtual bool unload(){return true;};
};

class Ce2TextureObject : public Ce2Resource
{
 public:
  GLenum glID;
  int width, height, nBPP, internalFormat, format, type, target;
  vec2 texel;

  Ce2TextureObject(string & _name):Ce2Resource(_name)
   {
    width = height = nBPP = glID = internalFormat = format = type = 0;
    texel = vec2(0.0);
    target = GL_TEXTURE_2D;
   };
  virtual bool load(const string& filename, const string& params);
  virtual bool unload();
  void setWrap(GLenum S, GLenum T, GLenum R = GL_CLAMP_TO_EDGE);
  void setFiltration(GLenum min_f, GLenum mag_f);
  void compareRefToTexture(bool enable, GLenum compareFunc = GL_LEQUAL);
};

struct PROGRAM_UNIFORM
{
 GLenum nType;
 GLint  nLocation;
};
typedef stdext::hash_map<string, PROGRAM_UNIFORM> UniformMap;
typedef UniformMap::iterator UniformIterator;

class Ce2ProgramObject : public Ce2Resource
{
 private:
  int _mvm_loc;
  int _mvp_loc;
  int _cam_loc;
  int _l0_loc;
  int _lp_loc;
  string _prog_source;
  string _prog_folder;
  string _defines;
  void parse(string &source);
  void _uniform(string name, void * value, int nCount = 1);
 public:
  GLint ProgramObject;
  GLint VertexShader;
  GLint GeometryShader;
  GLint FragmentShader;
  UniformMap Uniforms;

  Ce2ProgramObject(string &_name):Ce2Resource(_name) 
   {
    ProgramObject = VertexShader = GeometryShader = FragmentShader = 0;
    _mvp_loc = _cam_loc = _l0_loc = _lp_loc = _mvm_loc = -1;
   }           
  GLint GetUniformLocation(string uniform);
  UniformIterator FindUniform(string name);
  virtual bool load(const string& filename, const string& params);
  virtual bool unload();
  void bind();
 
  int& modelViewMatrixUniformLocation(){return _mvm_loc;}
  int& mvpMatrixUniformLocation()      {return _mvp_loc;}
  int& cameraUniformLocation()         {return _cam_loc;}
  int& primaryLightUniformLocation()   {return _l0_loc;}
  int& lightProjectionMatrixLocation() {return _lp_loc;}
  
  void setModelViewMatrix(const mat4 &m);
  void setMVPMatrix(const mat4 &m);
  void setCameraPosition(const vec3& p);
  void setPrimaryLightPosition(const vec3& p);
  void setLightProjectionMatrix(const mat4 &m);
  
  void setCameraPropsFromRender();
  
  void setUniform(string name, float value){_uniform(name, &value);}
  void setUniform(string name,  vec2 value){_uniform(name, &value);}
  void setUniform(string name,  vec3 value){_uniform(name, &value);}
  void setUniform(string name,  vec4 value){_uniform(name, &value);}
  void setUniform(string name,                   float x, float y){float val[2] = {x, y};       _uniform(name, val);}
  void setUniform(string name,          float x, float y, float z){float val[3] = {x, y, z};    _uniform(name, val);}
  void setUniform(string name, float x, float y, float z, float w){float val[4] = {x, y, z, w}; _uniform(name, val);}
  void setUniform(string name, mat4& m){_uniform(name, &m);}
  void setUniform(string name, int count, void* value){_uniform(name, value, count);}
 
  // arrays
  void setUniform(string name, float value[], int nCount){_uniform(name, value, nCount);}
  void setUniform(string name, vec2  value[], int nCount){_uniform(name, value, nCount);}
  void setUniform(string name, vec3  value[], int nCount){_uniform(name, value, nCount);}
  void setUniform(string name, vec4  value[], int nCount){_uniform(name, value, nCount);}
  void setUniform(string name, mat4  value[], int nCount){_uniform(name, value, nCount);}
 
  // samplers
  void setUniform(string name, int value){_uniform(name, &value);}
  void setUniformTextures(int n, string names[], int values[]);
 
};

typedef vector<Ce2Resource*> ResourceMap;

class Ce2ResourceManager
{
 private:
  ResourceMap all;
  vector<string> resource_path;
  void InitDefaultTextures();
 public:
  Ce2TextureObject * DEFTEX_white;
  Ce2TextureObject * DEFTEX_black;
  Ce2TextureObject * DEFTEX_normal;
  Ce2TextureObject * DEFTEX_cell;
  Ce2TextureObject * DEFTEX_noise;
  Ce2TextureObject * DEFTEX_normalnoise;
  
  static Ce2ResourceManager * Instance;
  Ce2ResourceManager();
  void Cleanup();
  
  string findFile(string name);
  inline void addSearchPath(string path) { resource_path.push_back(path);}
  
  void checkILError(string info);
  void checkError(string info);
  
  void log(string info);
  
  Ce2TextureObject* loadTexture(string fname){return load<Ce2TextureObject>(fname, "");}
  void unloadTexture(Ce2TextureObject * texture){Unload<Ce2TextureObject>(texture);};
  
  Ce2TextureObject* genTexture2D(string name, int nWidth, int nHeight, int nInternalFormat, int nFormat, int nType, void *data);
  Ce2TextureObject* genCubeTexture(string name, int nWidth, int nHeight, int nInternalFormat, int nFormat, int nType, void* data[6]);
  Ce2TextureObject* genNoiseTexture2D(string name, int nWidth, int nHeight, bool bNormalize);
  
  Ce2ProgramObject * loadProgram(const string& fname, const string& defines = ""){return load<Ce2ProgramObject>(fname, defines);}
  void UnloadProgram(Ce2ProgramObject * program){Unload<Ce2ProgramObject>(program);}
  
  template<typename RT>
  RT* load(const string& filename, const string& params)
  {          
   ResourceMap::iterator P;
   
   for (P = all.begin(); P != all.end(); P++)
    if ((*P)->name == filename)
    {
     return dynamic_cast<RT*>(*P);
    }

   string fname = findFile(filename);
   
   RT*  R = new RT(fname);
   all.push_back(R);
    
   if (!fileExists(fname)) 
   {
    log("Unable to find " + filename + " or " + fname);
    return R; 
   } 
   
   R->load(fname, params);
   return R;
  }

  template<typename RT>
  void Unload(RT* resource)
  {
   if (!resource) return;
   
   resource->unload();
   ResourceMap::iterator i = find(all.begin(), all.end(), resource);
   if (i != all.end())
   {
    delete *i;
    all.erase(i);
   }
  }
 
};

inline Ce2ResourceManager* manager() {return Ce2ResourceManager::Instance;}

}