#pragma once

#include <vector>
#include <windows.h>
#include <gl3/glgen.h>
#include <e2manager.h>
#include <e2tools.h>
#include <e2helpers.h>
#include <e2types.h>

using namespace std;

namespace et
{

class Ce2Render;

struct HANDLE_REC
{
 HWND  hWND;
 HDC   hDC;
 HGLRC hGLRC;
};

struct Ce2Buffer
{
 string name;
 int ID;
 int nIndSize;
 int nDataSize;
 int nVertices;
 int nIndices;
 int nStructSize;
 int nDrawMode;
 GLuint nDataID;
 GLuint nElemID;
 int nAttribs;
 renderAttributeVector attribs;

// backward compatibility
 float* bufferData;
 Index* indexData;

 void bind();
 void renderAll();
 void drawAllElements();
 void drawElements(Index mode, Index count, Index offset);
 void unbind();
};

struct Ce2Framebuffer
{
 string name;
 unsigned int nID;
 int szWidth;
 int szHeight;
 int nTargets;
 Ce2TextureObject * renderTarget[16];
 Ce2TextureObject * depthBuffer;
 Ce2Framebuffer();
 void bind();
 bool check();
 bool addRenderTarget(Ce2TextureObject * texture);
 bool setDepthTarget(Ce2TextureObject * texture);
 
 void addSameRendertarget();

 void setCurrentRenderTarget(Ce2TextureObject *texture, GLenum target);
 void setCurrentRenderTarget(Ce2TextureObject * texture);
 void setCurrentRenderTarget(int rt);
 
 void setDrawBuffersCount(int c);
};

class Ce2Render
{
 private:
  typedef Ce2Buffer* (et::Ce2Render::*CreateVertexBufferFunc)
           (string, renderAttributeVector, int, void*, int, Index*, int nDrawMode);

  CreateVertexBufferFunc createVertexBufferFunc;
 public:
  bool canRender;
  int nElementCounter;
  
  Ce2Camera* camera;
  inline Ce2Camera* primaryCamera() {return primaryCamera_;};
  
  Ce2Render(){};
  Ce2Render(HWND PrimaryWnd, bool bForwardContext);
  void log(string info);
  void checkError(string info);
  void checkErrorF(string funcname, string &info);
  void cleanup(); 
  static Ce2Render* instance;
  
  void init();
  void render();
  
  void viewport(int szX, int szY);
  inline void viewport(vec2i size) {viewport(size.x, size.y);}
  
//////// backward-compatibility
  inline bool supportVertexAttribArrays() { return support_vertex_attrib_arrays; }
  inline bool supportShaders() { return support_shaders; }
  inline bool supportVertexBuffers() {return support_vertex_buffers;}
  
//////// TEXTURE
  void bindTexture(char unit, Ce2TextureObject * texture, int target = GL_TEXTURE_2D);
  void buildTexture(Ce2TextureObject * texture, int target, void * data, bool bBuildMIPMAPs = true);
  void buildCubeTexture(Ce2TextureObject* texture, void* data[6], bool bBuildMIPMAPs = true);
  void unloadTexture(Ce2TextureObject * tex);
  
//////// BUFFERS  
  Ce2Buffer* createVertexBuffer(string name, renderAttributeVector attribs, 
    int nVertices, void * data, int nIndices, unsigned int *indices, int nDrawMode = GL_TRIANGLES);
  void deleteVertexBuffer(Ce2Buffer * buffer);
  
  Index buildTriangleStripIndexes(Index*& i, vec2i gridDimension);
  
  Ce2Buffer* createPlane(string name, vec3 v00, vec3 v01, vec3 v11, vec3 v10, vec2i nDiv);
  Ce2Buffer* createBox(string name, vec3 vDimension, bool invertNormals);
  Ce2Buffer* createSphere(string name, float radius, int nVer, int nHor);
  Ce2Buffer* createCylinder(string name, float baseRadius, float topRadius, float height, int nVer, int nHor, bool invNormals = false);
  Ce2Buffer* createElipseCylinder(string name, vec2 radius, float height, int nVer, int nHor);

  Ce2Buffer* createSphere_V3    (string name, float radius, int nVer, int nHor);
  Ce2Buffer* createHemiSphere_V3(string name, float radius, int nVer, int nHor);

  Ce2Buffer* createPhotonMap(const string& name, const vec2i& size);
  
//////// PROGRAMS
  void buildProgram(Ce2ProgramObject *program, string &vertex_source, string &geom_source, string &frag_source);
  void unloadProgram(Ce2ProgramObject * program);
  void bindProgram(Ce2ProgramObject * program);
  
///////// FRAMEBUFFERS
  Ce2Framebuffer* createFramebuffer(string name, int width, int height, int texInternalFormat = GL_RGBA, 
   int texFormat = GL_RGBA, int texType = GL_UNSIGNED_BYTE, int depthInternalFormat = GL_DEPTH_COMPONENT, 
   int depthFormat = GL_DEPTH_COMPONENT, int depthType = GL_UNSIGNED_BYTE);
   
  Ce2Framebuffer* createCubemapFramebuffer(string name, int size, int texInternalFormat = GL_RGBA, 
   int texFormat = GL_RGBA, int texType = GL_UNSIGNED_BYTE, int depthInternalFormat = GL_DEPTH_COMPONENT24, 
   int depthFormat = GL_DEPTH_COMPONENT, int depthType = GL_UNSIGNED_BYTE);
   
  void unloadFramebuffer(Ce2Framebuffer * FBO);
  void bindFramebuffer(Ce2Framebuffer * FBO);
  
///////// SERVICE
  void cullFace(GLenum C); 
  const GLenum cullFace() const {return _cullState;}

  void blend(bool B);
  const bool blend() const {return _blendEnabled;} 

  void blendFunction(GLenum B0, GLenum B1);
  
///////// ADDITIONAL SERVICE
  inline Ce2Buffer* fullscreenQuad() const {return FSQuad;}

  void drawFSQ();
  void drawFSQTex(Ce2TextureObject * texture);
  void drawFSQTexScaled(Ce2TextureObject * texture, vec4 scale);

 private:
  static HDC Primary_DC;
  static HGLRC Primary_RC;
  vector<HANDLE_REC>Handles;
  vector<Ce2Buffer*>Buffers;
  vector<Ce2Framebuffer*>Framebuffers;
  UINT TEXMAP[32];   // textures bind controller
  char nLastUnit;    // last active texture unit
  int nLastFBO, nLastVPx, nLastVPy, nLastB0, nLastB1;
  void CreateContext(HWND hWnd, bool bForwardContext);

  Ce2Buffer* FSQuad;
  Ce2ProgramObject* _prog_copy;
  Ce2ProgramObject* _prog_copy_scale;
  Ce2Camera* primaryCamera_;
  
  GLint  _pcs_u;
  GLenum _cullState;

  bool support_mipmap_generation;
  bool support_shaders;
  bool support_vertex_attrib_arrays;
  bool support_vertex_buffers;
  bool _blendEnabled;

  Ce2Buffer* createVertexBuffer_vbo(string name, renderAttributeVector attribs, 
    int nVertices, void * data, int nIndices, unsigned int *indices, int nDrawMode = GL_TRIANGLES);
  Ce2Buffer* createVertexBuffer_std(string name, renderAttributeVector attribs, 
    int nVertices, void * data, int nIndices, unsigned int *indices, int nDrawMode = GL_TRIANGLES);
};

inline Ce2Render* render() {return Ce2Render::instance;}

void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices);

const GLuint RENDERBUFFERS[16] = 
 {
  GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,
  GL_COLOR_ATTACHMENT2,  GL_COLOR_ATTACHMENT3,
  GL_COLOR_ATTACHMENT4,  GL_COLOR_ATTACHMENT5,
  GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7,
  GL_COLOR_ATTACHMENT8,  GL_COLOR_ATTACHMENT9,
  GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
  GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13,
  GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
 };
 
const GLuint RENDERBUFFERS_DISABLED[16] = 
 {
  GL_NONE,  GL_NONE,  GL_NONE,  GL_NONE,
  GL_NONE,  GL_NONE,  GL_NONE,  GL_NONE,
  GL_NONE,  GL_NONE,  GL_NONE,  GL_NONE,
  GL_NONE,  GL_NONE,  GL_NONE,  GL_NONE
 };
 
 }