#include <vector>
#include <e2tools.h>
#include <e2render.h>
#include <e2core.h>
#include <e2geometry.h>
#include <e2primitives.h>

using namespace std;
using namespace et;

static int _sh_count = 0;

static const GLenum RENDERBUFFERS_ENUM[16] = 
 { GL_COLOR_ATTACHMENT0, 
   GL_COLOR_ATTACHMENT1,
   GL_COLOR_ATTACHMENT2,
   GL_COLOR_ATTACHMENT3,
   GL_COLOR_ATTACHMENT4,
   GL_COLOR_ATTACHMENT5,
   GL_COLOR_ATTACHMENT6,
   GL_COLOR_ATTACHMENT7,
   GL_COLOR_ATTACHMENT8,
   GL_COLOR_ATTACHMENT9,
   GL_COLOR_ATTACHMENT10,
   GL_COLOR_ATTACHMENT11,
   GL_COLOR_ATTACHMENT12,
   GL_COLOR_ATTACHMENT13,
   GL_COLOR_ATTACHMENT14,
   GL_COLOR_ATTACHMENT15 };

////////////////////////// OVERLOADED OPENGL FUNCTIONS //////////////////////////////////
void TexImage2D(GLenum  target,  GLint level, GLint internalformat, GLsizei width, 
                GLsizei height, GLenum format, GLenum type, const GLvoid * pixels, string info = "")
{
 glTexImage2D(target, level, internalformat, width, height, 0, format, type, pixels);
 GLenum error = glGetError();
 if (error == GL_NO_ERROR) return;
 
/////////////////////////
 string ERRSTR = glErrorToString(error);
 ERRSTR += " at glTexImage2D(" + glTexTargetToString(target) = ", " + intToStr(level) 
         + ", " + glInternalFormatToString(internalformat)
         + ", " + intToStr(width)
         + ", " + intToStr(height)
         + ", 0, " + glInternalFormatToString(format)
         + ", " + glTypeToString(type)
         + ", *" + intToStr( (int)pixels ) + ") " + info;
 render()->log(ERRSTR.c_str());
/////////////////////////
}

void et::DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
 glDrawElements(mode, count, type, indices);
 
 render()->checkError("glDrawElemets");
 if (mode == GL_TRIANGLES)      render()->nElementCounter += count / 3;
 if (mode == GL_TRIANGLE_STRIP) render()->nElementCounter += count - 2;
}

////////////////////////// END OVERLOADED OPENGL FUNCTIONS ////////////////////////////////

Ce2Render* Ce2Render::instance;
HDC   Ce2Render::Primary_DC;
HGLRC Ce2Render::Primary_RC;

Ce2Render::Ce2Render(HWND PrimaryWnd, bool bForwardContext)
{
 instance = this;
 
 CreateContext(PrimaryWnd, bForwardContext); 
 
 canRender = (Primary_RC != 0);
 if (!canRender) return;

 log((char*)glGetString(GL_VENDOR));
 log((char*)glGetString(GL_RENDERER));
 
 support_mipmap_generation = ( glGenerateMipmap != NULL );
 support_shaders = ( glShaderSource != NULL ) && ( glCreateProgram != NULL );
 support_vertex_attrib_arrays = (glVertexAttribPointer != NULL);
 support_vertex_buffers = (glGenBuffers != NULL) && (glBindBuffer != NULL) && (glBufferData != NULL);

 createVertexBufferFunc = &et::Ce2Render::createVertexBuffer_vbo;
 
 primaryCamera_ = new Ce2Camera();
 primaryCamera_->perspective(DEG_45, core.windowAspect, 1.0f, 1000.0f);
 primaryCamera_->lookAt(vec3(100.0, 100.0, 100.0), NULL_VEC3, vec3(0.0, 1.0, 0.0));
 
 //modelManager = new Ce2ModelManager();
 
 camera = primaryCamera_;
 nLastFBO = 0;
 
 nLastVPx = nLastVPy = 0;
 nLastB0  = nLastB1  = 0;
 viewport(core.windowSize);

 _cullState = GL_NONE;
 _blendEnabled = false;
 
 glEnable(GL_DEPTH_TEST);
 wglSwapIntervalEXT(0);
 checkError("wglSwapIntervalEXT(0)");
}

void Ce2Render::cleanup()
{
//modelManager->cleanUp();
//delete modelManager;

 delete primaryCamera_;
 
 while (Buffers.size() > 0) 
  deleteVertexBuffer( *Buffers.rbegin() );
  
 vector<HANDLE_REC>::iterator I;
 for (I = Handles.begin(); I != Handles.end(); I++)
 {
  wglMakeCurrent(I->hDC, 0);
  wglDeleteContext(I->hGLRC);
  ReleaseDC(I->hWND, I->hDC);
 }
}

void Ce2Render::log(string info)
{
 string tolog = "RENDER: ";
 tolog += info;
 core.log(tolog);
}

void Ce2Render::checkError(string info)
{
#ifdef _DEBUG
 GLenum error = glGetError();
 if (error == GL_NO_ERROR) return;

 string err_inf = glErrorToString(error) + " at " + info;
 log(err_inf.c_str());
#endif 
}

void Ce2Render::checkErrorF(string funcname, string &info)
{
//#ifdef _DEBUG     
 GLenum error = glGetError();
 if (error == GL_NO_ERROR) return;

 string err_inf = glErrorToString(error) + " at " + funcname + "(" + info + ")";
 log(err_inf.c_str());
//#endif 
}

void Ce2Render::CreateContext(HWND hWnd, bool bForwardContext)
{
 HANDLE_REC R;
 
 R.hDC = 0;
 R.hWND = hWnd;
 R.hGLRC = 0;
 Primary_DC = 0;
 Primary_RC = 0;
 
 bool bFirstContext = (Handles.size() == 0);

 OSVERSIONINFO osvi = {0};
 osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
 if (!GetVersionEx(&osvi))
 { 
  log("Невозможно выполнить GetVersionEx");
  return;
 }

 R.hDC  = GetDC(R.hWND);
 if (!(R.hDC)) 
 {
  log("Невозможно получить DC");
  return;
 }
 
 if (bFirstContext) 
  Primary_DC = R.hDC;
  
 PIXELFORMATDESCRIPTOR PFD = {0};
 PFD.nSize    = sizeof(PIXELFORMATDESCRIPTOR);
 PFD.nVersion = 1;
 PFD.dwFlags  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION * (osvi.dwMajorVersion >= 6);
 PFD.cColorBits = 32;
 PFD.cDepthBits = 32;
 
 int nPixelFormat = ChoosePixelFormat(R.hDC, &PFD);
 if (nPixelFormat == 0)
 { 
  ReleaseDC(R.hWND, R.hDC);
  log("Невозможно выполнить ChoosePixelFormat");
  return;
 }
 
 if (!SetPixelFormat(R.hDC, nPixelFormat, &PFD))
 {
  ReleaseDC(R.hWND, R.hDC);
  log("Невозможно выполнить SetPixelFormat");
  return;
 }
 
 HGLRC tmpRC;
 if (!(tmpRC = wglCreateContext(R.hDC)))
 {
  ReleaseDC(R.hWND, R.hDC);
  log("Невозможно выполнить wglCreateContext");
  return;
 }
 
 if (!wglMakeCurrent(R.hDC, tmpRC))
 {
  wglDeleteContext(R.hGLRC);
  ReleaseDC(R.hWND, R.hDC);
  log("Невозможно выполнить wglMakeCurrent");
  return;
 }

 glgInit();
 
 wglCreateContextAttribsARB = 
  (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

 if (!bForwardContext || (wglCreateContextAttribsARB == NULL))
 {
  R.hGLRC = tmpRC;
  if (bFirstContext) Primary_RC = tmpRC;
  string info = "Создан backward-context OpenGL ";
  info += (char*)glGetString(GL_VERSION);
  checkError("glGetString(GL_VERSION)");
  log(info);
  glgInitExts();
  Handles.push_back(R);
  return;
 }
 
 int nAttribs[] = 
 {
  WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
  WGL_CONTEXT_MINOR_VERSION_ARB, 3,
  WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
  0, 0
 };

 while (!(R.hGLRC = wglCreateContextAttribsARB(R.hDC, 0, nAttribs)) && (nAttribs[3] >= 0))
 {
  nAttribs[3] -= 1;
 }

 if (nAttribs[3] == -1)
 {
  R.hGLRC = tmpRC;
  if (bFirstContext) Primary_RC = tmpRC;
  string info = "Невозможно создать контекст OpenGL 3, используется OpenGL 2";
  glgInitExts();
  Handles.push_back(R);
  return;
 }
 
 if (!wglMakeCurrent(R.hDC, R.hGLRC))
 {
  wglDeleteContext(R.hGLRC);
  R.hGLRC = tmpRC;
  if (bFirstContext) Primary_RC = tmpRC;
  log("Невозможно выполнить wglMakeCurrent для forward-контекста");
  Handles.push_back(R);
  return;
 }
 
 wglDeleteContext(tmpRC);
 checkError("wglDeleteContext(backward context)");
 
 string info = "Создан forward-context OpenGL ";
 info += (char*)glGetString(GL_VERSION);
 checkError("glGetString(GL_VERSION)");
 log(info);
 
 if (bFirstContext) Primary_RC = R.hGLRC;
 glgInitExts();
 
 Handles.push_back(R);
}

void Ce2Render::viewport(int szX, int szY)
{
 if ((nLastVPx != szX) || (nLastVPy != szY))
 {
  nLastVPx = szX;
  nLastVPy = szY;
  glViewport(0, 0, szX, szY);
 }
}

void Ce2Render::render()
{
 nElementCounter = 0;
 checkError("Ce2Render::Render(begin)");
 
 if (core.scene)
 {
   core.scene->renderScene();
 }
 
 SwapBuffers(Primary_DC);
 checkError("Ce2Render::Render(end)");
}

void Ce2Render::bindTexture(char unit, Ce2TextureObject* texture, int target)
{
 if (unit != nLastUnit)
  glActiveTexture(GL_TEXTURE0 + (nLastUnit = unit));

 if (texture)
 {
  if (TEXMAP[unit] != texture->glID)
  {
   glBindTexture(texture->target, TEXMAP[unit] = texture->glID);
   checkError("glBindTexture(" + texture->name + ")");
  } 
 }
 else glBindTexture(target, TEXMAP[unit] = 0); 
}

void Ce2Render::buildCubeTexture(Ce2TextureObject* texture, void* data[], bool bBuildMIPMAPs)
{
 string info = "buildCubeTexture<ENTER>("; info += texture->name; info += ");";
 checkError(info);
 
 texture->target = GL_TEXTURE_CUBE_MAP;
 glGenTextures(1, &(texture->glID));
 checkErrorF("glGenTextures", texture->name);
 
 bindTexture(0, texture, GL_TEXTURE_CUBE_MAP);
 checkErrorF("bindTexture<CUBE>", texture->name);
 
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  checkErrorF("glTexParameteri<CUBE, MIN>", texture->name);
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkErrorF("glTexParameteri<CUBE, MAG>", texture->name);
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  checkErrorF("glTexParameteri<CUBE, WRAP_S>", texture->name);
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  checkErrorF("glTexParameteri<CUBE, WRAP_T>", texture->name);
 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
  checkErrorF("glTexParameteri<CUBE, WRAP_R>", texture->name);

 for (int face = 0; face < 6; face++)
 {
   TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 
    texture->internalFormat, texture->width, texture->height, texture->format, 
    texture->type, (data == 0) ? 0 : data[face], texture->name); 
   checkErrorF("glTexImage2D<CUBE, " + intToStr(face) + ">", texture->name);
 }    
}

void Ce2Render::buildTexture(Ce2TextureObject * texture, int target, void * data, bool bBuildMIPMAPs)
{
 string info = "Ce2ResourceManager::BuildTexture2D("; info += texture->name; info += ");";
 checkError(info);

 texture->target = GL_TEXTURE_2D;
 texture->texel = vec2(1.0f/texture->width, 1.0f/texture->height);
 glGenTextures(1, &(texture->glID));

 info = "glGenTextures("; info += texture->name; info += ");";
 checkError(info);

 bindTexture(0, texture);

 if (data && support_mipmap_generation)
 {
  glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  info = "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR) for "; info += texture->name;
 } 
 else
 {
  glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  info = "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) for "; info += texture->name;
 } 
 checkError(info);

 glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 info = "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) for "; info += texture->name;
 checkError(info);

 glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
 info = "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) for "; info += texture->name;
 checkError(info);

 glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_REPEAT);
 info = "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) for "; info += texture->name;
 checkError(info);

 TexImage2D(texture->target, 0,  
   texture->internalFormat, 
   texture->width, 
   texture->height,
   texture->format,
   texture->type, data, texture->name);
   
 if (data && support_mipmap_generation)
 {
  glGenerateMipmap(target);
  checkErrorF("glGenerateMipmap", glTexTargetToString(target));
 }
  
}

void Ce2Render::unloadTexture(Ce2TextureObject * tex)
{
 glDeleteTextures(1, &(tex->glID));
 string info = "glDeleteTextures("; info += tex->name; info += ")";
 checkError(info);
}

Ce2Buffer* Ce2Render::createVertexBuffer(string name, renderAttributeVector _attribs, 
    int nVertices, void * data, int nIndices, unsigned int *indices, int nDrawMode)
{
 //(this->createVertexBufferFunc)(name, _attribs, nVertices, data, nIndices, indices, nDrawMode);

 if (support_vertex_buffers)
  return createVertexBuffer_vbo(name, _attribs, nVertices, data, nIndices, indices, nDrawMode);
 else	
  return createVertexBuffer_std(name, _attribs, nVertices, data, nIndices, indices, nDrawMode);

}

Ce2Buffer* Ce2Render::createVertexBuffer_vbo(string name, renderAttributeVector _attribs, 
    int nVertices, void * data, int nIndices, unsigned int *indices, int nDrawMode)
{
 Ce2Buffer* Buffer = new Ce2Buffer;
 
 Buffer->name = name;
 Buffer->nDrawMode = nDrawMode;
 Buffer->nAttribs = _attribs.size();
 Buffer->bufferData = 0;
 Buffer->indexData = 0;
 
 Buffer->attribs.clear();
 for (int i = 0; i < Buffer->nAttribs; i++) 
   Buffer->attribs.push_back(_attribs[i]);
 
 int fSize = sizeof(float);
 
 Buffer->nStructSize = 0;
 for (int k = 0; k < Buffer->nAttribs; k++)
  Buffer->nStructSize += Buffer->attribs[k].numFloats;
 Buffer->nStructSize *= fSize;
 
 Buffer->nIndices  = nIndices;
 Buffer->nVertices = nVertices;
 Buffer->nIndSize  = Buffer->nIndices * sizeof(unsigned int);
 Buffer->nDataSize = Buffer->nStructSize * Buffer->nVertices; 

// glGenVertexArrays(1, &(Buffer->nVAO));  checkErrorF("glGenVertexArrays", Buffer->name);
// glBindVertexArray(Buffer->nVAO);        checkErrorF("glBindVertexArray", Buffer->name);

 glGenBuffers(1, &(Buffer->nElemID));                                              
 checkErrorF("glGenBuffers", Buffer->name);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer->nElemID);                           
 checkErrorF("glBindBuffer<ELEMENT>", Buffer->name);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, Buffer->nIndSize, indices, GL_STATIC_DRAW); 
 checkErrorF("glBufferData<ELEMENT>", Buffer->name);
 
 glGenBuffers(1, &(Buffer->nDataID));                                    
 checkErrorF("glGenBuffers", Buffer->name);
 glBindBuffer(GL_ARRAY_BUFFER, Buffer->nDataID);                         
 checkErrorF("glBindBuffer<ARRAY>", Buffer->name);
 glBufferData(GL_ARRAY_BUFFER, Buffer->nDataSize, data, GL_STATIC_DRAW); 
 checkErrorF("glBufferData<ARRAY>", Buffer->name);
/*
 if (support_vertex_attrib_arrays)
 {
  int nOffset = 0;
  for (int k = 0; k < Buffer->nAttribs; k++)
  {
   glEnableVertexAttribArray(Buffer->attribs[k].ID);
    checkErrorF("glEnableVertexAttribArray<" + intToStr(Buffer->attribs[k].ID) + ">", Buffer->name);
   glVertexAttribPointer(Buffer->attribs[k].ID, Buffer->attribs[k].nFloats, GL_FLOAT, false, Buffer->nStructSize, (void*)(fSize * nOffset));
    checkErrorF("glVertexAttribPointer<POS>", Buffer->name);
   nOffset += Buffer->attribs[k].nFloats;
  }
 }
 glBindVertexArray(0);
*/
 
 glBindBuffer(GL_ARRAY_BUFFER, 0); 
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
 
 Buffer->ID = Buffers.size() + 1;
 Buffers.push_back(Buffer);
 
 return Buffer;
}

Ce2Buffer* Ce2Render::createVertexBuffer_std(string name, renderAttributeVector _attribs, 
    int nVertices, void* data, int nIndices, unsigned int *indices, int nDrawMode)
{
 Ce2Buffer* Buffer = new Ce2Buffer;
 
 Buffer->name = name;
 Buffer->nDrawMode = nDrawMode;
 Buffer->nAttribs = _attribs.size();
 Buffer->nDataID = 0;
 Buffer->nElemID = 0;
 
 Buffer->attribs.clear();
 for (int i = 0; i < Buffer->nAttribs; i++) 
   Buffer->attribs.push_back(_attribs[i]);
 
 int fSize = sizeof(float);
 
 Buffer->nStructSize = 0;
 for (int k = 0; k < Buffer->nAttribs; k++)
  Buffer->nStructSize += Buffer->attribs[k].numFloats;

 Buffer->nStructSize *= fSize;
 Buffer->nIndices  = nIndices;
 Buffer->nVertices = nVertices;
 Buffer->nIndSize  = Buffer->nIndices * sizeof(Index);
 Buffer->nDataSize = Buffer->nStructSize * Buffer->nVertices; 

 Buffer->bufferData = new float[Buffer->nDataSize / sizeof(float)];
 Buffer->indexData = new Index[Buffer->nIndSize / sizeof(Index)];

// memcpy(Buffer->bufferData, data, Buffer->nDataSize);
// memcpy(Buffer->indexData, indices, Buffer->nIndSize);
 CopyMemory(Buffer->bufferData, data, Buffer->nDataSize);
 CopyMemory(Buffer->indexData, indices, Buffer->nIndSize);

 Buffer->ID = Buffers.size() + 1;
 Buffers.push_back(Buffer);
 return Buffer;
}

void Ce2Render::deleteVertexBuffer(Ce2Buffer* buffer)
{
 if (!buffer) return;
 
 GLuint b;
 string info;

 if (!support_vertex_buffers)
 {
	 if (buffer->bufferData)
	  delete [] buffer->bufferData;
	 if (buffer->indexData)
	  delete [] buffer->indexData;
	 buffer->bufferData = 0;
	 buffer->indexData = 0;
 }
 else
 {
	 b = buffer->nDataID; 
	 if (glIsBuffer(b)) 
	 {
	  glDeleteBuffers(1, &b); 
	  checkError("glDeleteBuffers(" + buffer->name + " - data buffer)");
	 }
	 
	 b = buffer->nElemID;
	 if (glIsBuffer(b))
	 {
	  glDeleteBuffers(1, &b);
	  checkError("glDeleteBuffers(" + buffer->name + " - elem buffer)");
	 } 
 }
// b = buffer->nVAO; if (glIsVertexArray(b)) glDeleteVertexArrays(1, &b); info = "glDeleteVertexArrays("; info += buffer->name; info += ")"; checkError(info);

 vector<Ce2Buffer*>::iterator B = find(Buffers.begin(), Buffers.end(), buffer);
 
 if (B != Buffers.end())
  Buffers.erase(B);

 delete buffer;
}

void Ce2Render::init()
{
 vec2 vert[4]  = {vec2(-1.0,  1.0), vec2(-1.0, -1.0), vec2( 1.0,  1.0), vec2( 1.0, -1.0)};
 Index indx[4] = {0, 1, 2, 3};
 FSQuad = createVertexBuffer("Fullscreen quad", vec2::getRA(), 4, vert, 4, indx, GL_TRIANGLE_STRIP);
 
 if (support_shaders)
 {
  _prog_copy = manager()->loadProgram("shaders\\fullscreen.program");
  _prog_copy_scale = manager()->loadProgram("shaders\\fullscreen_scaled.program");
  _pcs_u = _prog_copy_scale->GetUniformLocation("vScale");
 }

}

void Ce2Render::buildProgram(Ce2ProgramObject *program, string &vertex_source, string &geom_source, string &frag_source)
{
 if (!support_shaders) return;
 
 checkErrorF("Ce2Render::buildProgram", program->name);

 if (!glIsProgram(program->ProgramObject))
 {
  program->ProgramObject = glCreateProgram();
  checkErrorF("glCreateProgram", program->name);
 }
 
 if (!glIsShader(program->VertexShader))   
 {
  program->VertexShader  = glCreateShader(GL_VERTEX_SHADER);
  checkErrorF("glCreateShader<VERT>", program->name);
 } 
 int nLen = vertex_source.size();
 
 const GLchar * src = vertex_source.c_str();
 glShaderSource(program->VertexShader, 1, &src, &nLen);
  checkErrorF("glShaderSource<VERT>", program->name);
 glCompileShader(program->VertexShader);
  checkErrorF("glCompileShader<VERT>", program->name);
  
 int cStatus = 0;
 GLsizei nLogLen = 0;
 glGetShaderiv(program->VertexShader, GL_COMPILE_STATUS, &cStatus);
 checkErrorF("glGetShaderiv<VERT>", program->name + " compile staus ");
 glGetShaderiv(program->VertexShader, GL_INFO_LOG_LENGTH, &nLogLen);
 if (nLogLen > 1)
 {
  GLchar* infoLog = new char[nLogLen + 1];
  memset(infoLog, 0, nLogLen + 1); 
  glGetShaderInfoLog(program->VertexShader, nLogLen, &nLogLen, infoLog);
  
  string s = "Vertex shader " + program->name + " compile report: ";
  log(s);
  log(infoLog);
  if (cStatus != GL_TRUE)
   MessageBoxA(core.primaryWindow, infoLog, s.c_str(), MB_ICONWARNING);
   
  delete [] infoLog;
 }
 if (cStatus)
 {
  glAttachShader(program->ProgramObject, program->VertexShader);
  checkErrorF("glAttachShader<VERT>", program->name);
  
  glBindAttribLocation(program->ProgramObject, RENDER_ATTRIB_POSITION,  "Vertex");
  checkErrorF("glBindAttribLocation<Vertex>", program->name);
  glBindAttribLocation(program->ProgramObject, RENDER_ATTRIB_NORMAL,    "Normal");
  checkErrorF("glBindAttribLocation<Normal>", program->name);
  glBindAttribLocation(program->ProgramObject, RENDER_ATTRIB_TEXCOORD0, "TexCoord0");
  checkErrorF("glBindAttribLocation<TexCoord0>", program->name);
  glBindAttribLocation(program->ProgramObject, RENDER_ATTRIB_TANGENT,   "Tangent");
  checkErrorF("glBindAttribLocation<Tangent>", program->name);
 } 

///////////////////////////////////////////////// GEOMETRY 
 if (geom_source != "none") 
  {
   if (!glIsShader(program->GeometryShader))
   {
    program->GeometryShader= glCreateShader(GL_GEOMETRY_SHADER);
    checkErrorF("glCreateShader<GEOM>", program->name);
    nLen = geom_source.size();
    src = geom_source.c_str();
    glShaderSource(program->GeometryShader, 1, &src, &nLen);
    checkErrorF("glShaderSource<GEOM>", program->name);
    
    cStatus = 0;
    nLogLen = 0;
    glCompileShader(program->GeometryShader);
    checkErrorF("glCompileShader<GEOM>", program->name);
    glGetShaderiv(program->GeometryShader, GL_COMPILE_STATUS, &cStatus);
    checkErrorF("glGetShaderiv<GEOM>", program->name + " compile staus ");
    glGetShaderiv(program->GeometryShader, GL_INFO_LOG_LENGTH, &nLogLen);
    if (nLogLen > 1)
    {
     GLchar* infoLog = new char[nLogLen + 1];
     memset(infoLog, 0, nLogLen + 1); 
     glGetShaderInfoLog(program->GeometryShader, nLogLen, &nLogLen, infoLog);
     
     string s = "Geometry shader " + program->name + " compile report: ";
     log(s);
     log(infoLog);
     if (cStatus != GL_TRUE)
      MessageBoxA(core.primaryWindow, infoLog, s.c_str(), MB_ICONWARNING);
     
     delete [] infoLog;
    }
    if (cStatus)
    {
     glAttachShader(program->ProgramObject, program->GeometryShader);
     checkErrorF("glAttachShader<GEOM>", program->name);
    } 
   } 
  } 
 
///////////////////////////////////////////////// FRAGMENT
 if (!glIsShader(program->FragmentShader))
 {
  program->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  checkErrorF("glCreateShader<FRAG>", program->name);
 } 
 nLen = frag_source.size();
 src  = frag_source.c_str();
 glShaderSource(program->FragmentShader, 1, &src, &nLen);
 checkErrorF("glShaderSource<FRAG>", program->name);
 glCompileShader(program->FragmentShader);
 checkErrorF("glCompileShader<FRAG>", program->name);

 cStatus = 0;
 nLogLen = 0;
 glGetShaderiv(program->FragmentShader, GL_COMPILE_STATUS, &cStatus);
 checkErrorF("glGetShaderiv<FRAG>", program->name + " compile staus ");
 glGetShaderiv(program->FragmentShader, GL_INFO_LOG_LENGTH, &nLogLen);
 if (nLogLen > 1)
 {
  GLchar* infoLog = new char[nLogLen + 1];
  memset(infoLog, 0, nLogLen + 1); 
  
  glGetShaderInfoLog(program->FragmentShader, nLogLen, &nLogLen, infoLog);

  string s = "Fragment shader " + program->name + " compile report: ";
  log(s);
  log(infoLog);
  if (cStatus != GL_TRUE)
   MessageBoxA(core.primaryWindow, infoLog, s.c_str(), MB_ICONWARNING);
  
  delete [] infoLog;
 }
 if (cStatus)
 {
  glAttachShader(program->ProgramObject, program->FragmentShader);
  checkErrorF("glAttachShader<FRAG>", program->name);
  glBindFragDataLocation(program->ProgramObject, 0, "FragColor");
  checkErrorF("glBindFragDataLocation<color0>", program->name);
  glBindFragDataLocation(program->ProgramObject, 1, "FragColor1");
  checkErrorF("glBindFragDataLocation<color1>", program->name);
  glBindFragDataLocation(program->ProgramObject, 2, "FragColor2");
  checkErrorF("glBindFragDataLocation<color2>", program->name);
  glBindFragDataLocation(program->ProgramObject, 3, "FragColor3");
  checkErrorF("glBindFragDataLocation<color3>", program->name);
  glBindFragDataLocation(program->ProgramObject, 4, "FragColor4");
  checkErrorF("glBindFragDataLocation<color4>", program->name);
 } 
 
 glLinkProgram(program->ProgramObject);
  checkErrorF("glLinkProgram", program->name);
 glGetProgramiv(program->ProgramObject, GL_LINK_STATUS, &cStatus);
  checkErrorF("glGetProgramiv<GL_LINK_STATUS>", program->name);
 glGetProgramiv(program->ProgramObject, GL_INFO_LOG_LENGTH, &nLogLen);
  checkErrorF("glGetProgramiv<GL_INFO_LOG_LENGTH>", program->name);
 
 if (nLogLen > 1)
 {
  GLchar* infoLog = new char[nLogLen + 1];
  memset(infoLog, 0, nLogLen + 1); 
  glGetProgramInfoLog(program->ProgramObject, nLogLen, &nLogLen, infoLog);
  checkErrorF("glGetProgramInfoLog<LINK>", program->name);
  string s = "Program object " + program->name + " link report: ";
  log(s);
  log(infoLog);
  if (cStatus != GL_TRUE)
   MessageBoxA(core.primaryWindow, infoLog, s.c_str(), MB_ICONWARNING);
  delete [] infoLog;
 }
 
 if (cStatus)
 {
  int nMaxLen = 0;
  int nUniforms = 0;
  program->Uniforms.clear();
  glGetProgramiv(program->ProgramObject, GL_ACTIVE_UNIFORMS, &nUniforms);
  glGetProgramiv(program->ProgramObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nMaxLen);
//  log("-------" + program->name + "-------");
//  log("Uniforms collected " + intToStr(nUniforms) + "");
  char * uName = new char[nMaxLen];
  for (int i = 0; i < nUniforms; i++)
  {
   GLsizei nLen = 0;
   GLint  nSize = 0;
   
   PROGRAM_UNIFORM P = {0};
   glGetActiveUniform(program->ProgramObject, i, nMaxLen, &nLen, &nSize, &P.nType, uName);
   P.nLocation = glGetUniformLocation(program->ProgramObject, uName);
//   log("Uniform \"" + string(uName) + "\" at location " + intToStr(P.nLocation));
   
   program->Uniforms[uName] = P;
  }
//  log("--------------");
  delete [] uName;
  
  UniformIterator i;
  i = program->FindUniform("mModelView");             if (i != program->Uniforms.end()) program->modelViewMatrixUniformLocation() = i->second.nLocation;
  i = program->FindUniform("mModelViewProjection");   if (i != program->Uniforms.end()) program->mvpMatrixUniformLocation()       = i->second.nLocation;
  i = program->FindUniform("vCamera");                if (i != program->Uniforms.end()) program->cameraUniformLocation()          = i->second.nLocation;
  i = program->FindUniform("vPrimaryLight");          if (i != program->Uniforms.end()) program->primaryLightUniformLocation()    = i->second.nLocation;
  i = program->FindUniform("mLightProjectionMatrix"); if (i != program->Uniforms.end()) program->lightProjectionMatrixLocation()  = i->second.nLocation;
 }
 
}

void Ce2Render::unloadProgram(Ce2ProgramObject *program)
{
 if (!support_shaders) return;
 
 if (glIsShader(program->FragmentShader))
 {
  glDeleteShader(program->FragmentShader);
  checkErrorF("glDeleteShader<FRAG>", program->name);
 }

 if (glIsShader(program->GeometryShader))
 {
  glDeleteShader(program->GeometryShader);
  checkErrorF("glDeleteShader<GEOM>", program->name);
 }
 
 if (glIsShader(program->VertexShader))
 {
  glDeleteShader(program->VertexShader);
  checkErrorF("glDeleteShader<VERT>", program->name);
 }
 
 if (glIsProgram(program->ProgramObject))
 {
  glDeleteProgram(program->ProgramObject);
  checkErrorF("glDeleteProgram", program->name);
 }
}

void Ce2Render::bindProgram(Ce2ProgramObject * program)
{
 checkErrorF("Ce2Render::bindProgram", program->name);
 if (program)
 {
  glUseProgram(program->ProgramObject);
  checkErrorF("glUseProgram", program->name);
 }
 else glUseProgram(0); 
}

Index Ce2Render::buildTriangleStripIndexes(Index*& i, vec2i dim)
{
 Index k = 0;
 
 for (int v = 0; v < dim.y - 1; v++)
 {
  if (v % 2)
   for (int u = dim.x - 1; u >= 0; u--)
   {
    i[k++] = u + ( v ) * dim.x;
    i[k++] = u + (v+1) * dim.x;
   }
  else
   for (int u = 0; u < dim.x; u++)
   {
    i[k++] = u + (v+1) * dim.x;
    i[k++] = u + ( v ) * dim.x;
   }
 }
 
 return k;
}

Ce2Buffer* Ce2Render::createPlane(string name, vec3 v00, vec3 v01, vec3 v11, vec3 v10, vec2i nDiv)
{                        
 vec3 n = planeNormal(v00, v01, v10);
 vec2i dv = vec2i(2, 2) + nDiv;
 
 int nV = dv.x * dv.y;
 int nI = nV * 6;
 
 VERT_V3_N3_T2* vert = new VERT_V3_N3_T2[nV];
 Index* ind  = new Index[nI];
 
 float dx = 1.0f / (dv.x-1);
 float dy = 1.0f / (dv.y-1);
 
 int k = 0;
 for (int v = 0; v < dv.y; v++)
  for (int u = 0; u < dv.x; u++)
  {
   float px =        u * dx;
   float py = 1.0f - v * dy;
   vec3 m0 = mix(v00, v10, py);
   vec3 m1 = mix(v01, v11, py);
   vert[k].vPosition = mix(m0, m1, px);
   vert[k].vTexCoord = vec2(px, py);
   ++k;
  }
 
 for (int v = 0; v < dv.y; v++)
  for (int u = 0; u < dv.x; u++)
  {
   int n00 = getIndex(  u  ,   v  , dv.x, dv.y);
   int n01;
   int n10;

   vec3 p00 = vert[n00].vPosition;
   vec3 p01;
   vec3 p10;
   
   if (u == dv.x - 1)
   {
    n01 = getIndex(u - 1,   v  , dv.x, dv.y);
    p01 = p00 + (p00 - vert[n01].vPosition);
   }
   else
   {
    n01 = getIndex(u + 1,   v  , dv.x, dv.y);
    p01 = vert[n01].vPosition;
   } 
   if (v == dv.y -1 )
   {
    n10 = getIndex(  u,   v - 1, dv.x, dv.y);
    p10 = p00 + (p00 - vert[n10].vPosition); 
   }
   else
   {
    n10 = getIndex(  u,   v + 1, dv.x, dv.y);
    p10 = vert[n10].vPosition; 
   }
   vert[ n00 ].vNormal = planeNormal(p00, p10, p01);
  }

 for (int v = 0; v < dv.y; v++)
  for (int u = 0; u < dv.x; u++)
  {
   vec3 n = 
    1.0f * vert[getIndex(u-1, v-1, dv.x, dv.y)].vNormal + 
    2.0f * vert[getIndex(  u, v-1, dv.x, dv.y)].vNormal + 
    1.0f * vert[getIndex(u+1, v-1, dv.x, dv.y)].vNormal + 
    2.0f * vert[getIndex(u-1,   v, dv.x, dv.y)].vNormal + 
    4.0f * vert[getIndex(  u,   v, dv.x, dv.y)].vNormal + 
    2.0f * vert[getIndex(u+1,   v, dv.x, dv.y)].vNormal + 
    1.0f * vert[getIndex(u-1, v+1, dv.x, dv.y)].vNormal +
    2.0f * vert[getIndex(  u, v+1, dv.x, dv.y)].vNormal +
    1.0f * vert[getIndex(u+1, v+1, dv.x, dv.y)].vNormal ;
   
   vert[getIndex(u, v, dv.x, dv.y)].vNormal = n.normalize(); 
  }

 k = buildTriangleStripIndexes(ind, dv);

 Ce2Buffer * R = createVertexBuffer(name, VERT_V3_N3_T2::getRA(), nV, vert, k, ind, GL_TRIANGLE_STRIP);
 
 delete [] vert;
 delete [] ind;
 
 return R;
}

Ce2Buffer * Ce2Render::createBox(std::string name, vec3 vDimension, bool invertNormals)
{
 const int num_verts = 36;

 static VERT_V3_N3_T2 vert[num_verts];
 static Index index[num_verts];

 float fInv = invertNormals ? -1.0f : 1.0f;

 for (int i =  0; i <  6; ++i) vert[i].vNormal = fInv * vec3( 0.0, -1.0,  0.0);
 for (int i =  6; i < 12; ++i) vert[i].vNormal = fInv * vec3( 0.0,  1.0,  0.0);
 for (int i = 12; i < 18; ++i) vert[i].vNormal = fInv * vec3(-1.0,  0.0,  0.0);
 for (int i = 18; i < 24; ++i) vert[i].vNormal = fInv * vec3( 1.0,  0.0,  0.0);
 for (int i = 24; i < 30; ++i) vert[i].vNormal = fInv * vec3( 0.0,  0.0,  1.0);
 for (int i = 30; i < 36; ++i) vert[i].vNormal = fInv * vec3( 0.0,  0.0, -1.0);

 for (int i = 0; i < 6; ++i)
 {
  vert[6*i+0].vTexCoord = vec2(1.0, 1.0);
  vert[6*i+1].vTexCoord = vec2(0.0, 1.0);
  vert[6*i+2].vTexCoord = vec2(0.0, 0.0);
  vert[6*i+3].vTexCoord = vec2(1.0, 0.0);
  vert[6*i+4].vTexCoord = vec2(1.0, 1.0);
  vert[6*i+5].vTexCoord = vec2(0.0, 0.0);
 }

 vert[ 0].vPosition = vec3( vDimension.x, -vDimension.y,  vDimension.z);
 vert[ 1].vPosition = vec3(-vDimension.x, -vDimension.y,  vDimension.z);
 vert[ 2].vPosition = vec3(-vDimension.x, -vDimension.y, -vDimension.z);
 vert[ 3].vPosition = vec3( vDimension.x, -vDimension.y, -vDimension.z);
 vert[ 4].vPosition = vec3( vDimension.x, -vDimension.y,  vDimension.z);
 vert[ 5].vPosition = vec3(-vDimension.x, -vDimension.y, -vDimension.z);
 vert[ 6].vPosition = vec3(-vDimension.x,  vDimension.y,  vDimension.z);
 vert[ 7].vPosition = vec3( vDimension.x,  vDimension.y,  vDimension.z);
 vert[ 8].vPosition = vec3( vDimension.x,  vDimension.y, -vDimension.z);
 vert[ 9].vPosition = vec3(-vDimension.x,  vDimension.y, -vDimension.z);
 vert[10].vPosition = vec3(-vDimension.x,  vDimension.y,  vDimension.z);
 vert[11].vPosition = vec3( vDimension.x,  vDimension.y, -vDimension.z);
 vert[12].vPosition = vec3(-vDimension.x,  vDimension.y, -vDimension.z);
 vert[13].vPosition = vec3(-vDimension.x, -vDimension.y, -vDimension.z);
 vert[14].vPosition = vec3(-vDimension.x, -vDimension.y,  vDimension.z);
 vert[15].vPosition = vec3(-vDimension.x,  vDimension.y,  vDimension.z);
 vert[16].vPosition = vec3(-vDimension.x,  vDimension.y, -vDimension.z);
 vert[17].vPosition = vec3(-vDimension.x, -vDimension.y,  vDimension.z);
 vert[18].vPosition = vec3( vDimension.x,  vDimension.y,  vDimension.z);
 vert[19].vPosition = vec3( vDimension.x, -vDimension.y,  vDimension.z);
 vert[20].vPosition = vec3( vDimension.x, -vDimension.y, -vDimension.z);
 vert[21].vPosition = vec3( vDimension.x,  vDimension.y, -vDimension.z);
 vert[22].vPosition = vec3( vDimension.x,  vDimension.y,  vDimension.z);
 vert[23].vPosition = vec3( vDimension.x, -vDimension.y, -vDimension.z);
 vert[24].vPosition = vec3(-vDimension.x,  vDimension.y,  vDimension.z);
 vert[25].vPosition = vec3(-vDimension.x, -vDimension.y,  vDimension.z);
 vert[26].vPosition = vec3( vDimension.x, -vDimension.y,  vDimension.z);
 vert[27].vPosition = vec3( vDimension.x,  vDimension.y,  vDimension.z);
 vert[28].vPosition = vec3(-vDimension.x,  vDimension.y,  vDimension.z);
 vert[29].vPosition = vec3( vDimension.x, -vDimension.y,  vDimension.z);
 vert[30].vPosition = vec3( vDimension.x,  vDimension.y, -vDimension.z);
 vert[31].vPosition = vec3( vDimension.x, -vDimension.y, -vDimension.z);
 vert[32].vPosition = vec3(-vDimension.x, -vDimension.y, -vDimension.z);
 vert[33].vPosition = vec3(-vDimension.x,  vDimension.y, -vDimension.z);
 vert[34].vPosition = vec3( vDimension.x,  vDimension.y, -vDimension.z);
 vert[35].vPosition = vec3(-vDimension.x, -vDimension.y, -vDimension.z);

 for (int i = 0; i < num_verts; i++) 
  index[i] = i;

 return createVertexBuffer(name, VERT_V3_N3_T2::getRA(), num_verts, vert, num_verts, index, GL_TRIANGLES);
}

Ce2Buffer* Ce2Render::createSphere(string name, float radius, int nVer, int nHor)
{
 vec2i dimension = vec2i(max(4, nHor), max(3, nVer));
 Index num_verts = dimension.square();
 
 VERT_V3_N3_T2* V = new VERT_V3_N3_T2[num_verts];
 Ce2Primitives::createSphere_v3n3t2(V, radius, dimension);
 
 Index* I = new Index[6 * num_verts];
 Index num_index = buildTriangleStripIndexes(I, dimension);
 
 Ce2Buffer* R = createVertexBuffer(name, VERT_V3_N3_T2::getRA(), num_verts, V, num_index, I, GL_TRIANGLE_STRIP);
 
 delete [] I;
 delete [] V;
 
 return R;
}

Ce2Buffer* Ce2Render::createCylinder(string name, float baseRadius, float topRadius, float height, int nVer, int nHor, bool invNormals)
{
 int xsz = max(4, nHor);
 int ysz = max(2, nVer);
 
 int nV = xsz * ysz;
 int nI = nV  * 6;
 
 Index* I = new Index[nI];
 VERT_V3_N3_T2* V = new VERT_V3_N3_T2[nV];
 int k = 0;

 float dy = (baseRadius - topRadius) / height;
 
 for (int i = 0; i < ysz; i++)
 {
  float fLinearY = float(i) / (ysz - 1);
  float cRadius = baseRadius * (1.0f - fLinearY) + topRadius * fLinearY;
  for (int j = 0; j < xsz; j++)
  {
   float MoveY = DOUBLE_PI * float(j) / (xsz - 1);
  
   V[k].vPosition.x = cosf(MoveY) * cRadius;
   V[k].vPosition.y = height * (fLinearY - 0.5f);
   V[k].vPosition.z = sinf(MoveY) * cRadius;
   
   V[k].vNormal = vec3( cosf(MoveY), dy, sinf(MoveY) ).normalize();
   if (invNormals)
    V[k].vNormal *= -1;
    
   V[k].vTexCoord = vec2(float(j) / (xsz-1), 1.0f - float(i) / (ysz - 1));
   k++;
  }
 } 
 
 int in = int(invNormals);
 int out = 1 - in;
 
 k = 0;
 for (int v = 0; v < ysz - 1; v++)
 {
  if (v % 2)
   for (int u = xsz - 1; u >= 0; u--)
   {
    I[k++] = u + (v+out) * xsz;
    I[k++] = u + (v+in) * xsz;
   }
  else
   for (int u = 0; u < xsz; u++)
   {
    I[k++] = u + (v+in) * xsz;
    I[k++] = u + (v+out) * xsz;
   }
 }
 
 Ce2Buffer * R = createVertexBuffer(name, VERT_V3_N3_T2::getRA(), 
                    nV, V, k, I, GL_TRIANGLE_STRIP);
 
 delete [] I;
 delete [] V;

 return R;
}

Ce2Buffer* Ce2Render::createElipseCylinder(string name, vec2 radius, float height, int nVer, int nHor)
{
 int xsz = max(4, nHor);
 int ysz = max(2, nVer);
 
 int nV = xsz * ysz;
 int nI = nV  * 6;
 
 Index* I = new Index[nI];
 VERT_V3_N3_T2* V = new VERT_V3_N3_T2[nV];
 int k = 0;

 for (int i = 0; i < ysz; i++)
 {
  float fLinearY = float(i) / (ysz - 1);
  for (int j = 0; j < xsz; j++)
  {
   float phi = DOUBLE_PI * float(j) / (xsz - 1);
   float cphi = cosf(phi);
   float sphi = sinf(phi);
   V[k].vPosition.x = cphi * radius.x;
   V[k].vPosition.y = height * fLinearY;
   V[k].vPosition.z = sphi * radius.y;
   V[k].vNormal = vec3( cphi / radius.x, 0.0, sphi / radius.y ).normalize();
   V[k].vTexCoord = vec2(float(j) / (xsz-1), 1.0f - float(i) / (ysz - 1));
   k++;
  }
 } 
 
 k = 0;
 for (int v = 0; v < ysz - 1; v++)
 {
  if (v % 2)
   for (int u = xsz - 1; u >= 0; u--)
   {
    I[k++] = u + (v+1) * xsz;
    I[k++] = u + (v) * xsz;
   }
  else
   for (int u = 0; u < xsz; u++)
   {
    I[k++] = u + (v)  * xsz;
    I[k++] = u + (v+1) * xsz;
   }
 }
 
 Ce2Buffer* R = createVertexBuffer(name, VERT_V3_N3_T2::getRA(), nV, V, k, I, GL_TRIANGLE_STRIP);
 
 delete [] I;
 delete [] V;

 return R;
}

Ce2Buffer * Ce2Render::createSphere_V3(string name, float radius, int nVer, int nHor)
{
 int xsz = max(4, nHor);
 int ysz = max(3, nVer);
 
 int nV = xsz * ysz;
 int nI = nV  * 6;
 
 Index* I = new Index[nI];
 vec3* V = new vec3[nV];
 int k = 0;
 
 for (int i = 0; i < ysz; i++)
 {
  float latitude = float(PI * i) / (ysz - 1);
  for (int j = 0; j < xsz; j++)
  {
   float logitude = float(DOUBLE_PI * j) / (xsz - 1);
   V[k].x = sinf(latitude) * cosf(logitude) * radius;
   V[k].y = cosf(latitude)                  * radius;
   V[k].z = sinf(latitude) * sinf(logitude) * radius;
   k++;
  }
 } 
 
 k = 0;
 for (int v = 0; v < ysz - 1; v++)
 {
  if (v % 2)
   for (int u = xsz - 1; u >= 0; u--)
   {
    I[k++] = u + ( v ) * xsz;
    I[k++] = u + (v+1) * xsz;
   }
  else
   for (int u = 0; u < xsz; u++)
   {
    I[k++] = u + (v+1)  * xsz;
    I[k++] = u + ( v ) * xsz;
   }
 }
 
 Ce2Buffer* R = createVertexBuffer(name, vec3::getRA(), nV, V, k, I, GL_TRIANGLE_STRIP);
 
 delete [] I; 
 delete [] V; 
 
 return R;
}

Ce2Buffer* Ce2Render::createHemiSphere_V3(string name, float radius, int nVer, int nHor)
{
 int xsz = max(4, nHor);
 int ysz = max(3, nVer);
 
 int nV = xsz * ysz;
 int nI = nV  * 6;
 
 Index* I = new Index[nI];
 vec3* V = new vec3[nV];
 int k = 0;
 
 for (int i = 0; i < ysz; i++)
 {
  float MoveXZ = float(PI * i) / (ysz - 1);
  for (int j = 0; j < xsz; j++)
  {
   float MoveY = float(PI * j) / (xsz - 1);
   V[k].x = sinf(MoveXZ) * cosf(MoveY) * radius;
   V[k].y = cosf(MoveXZ)               * radius;
   V[k].z = sinf(MoveXZ) * sinf(MoveY) * radius;
   k++;
  }
 } 
 
 k = 0;
 for (int v = 0; v < ysz - 1; v++)
 {
  if (v % 2)
   for (int u = xsz - 1; u >= 0; u--)
   {
    I[k++] = u + ( v ) * xsz;
    I[k++] = u + (v+1) * xsz;
   }
  else
   for (int u = 0; u < xsz; u++)
   {
    I[k++] = u + (v+1)  * xsz;
    I[k++] = u + ( v ) * xsz;
   }
 }
 
 Ce2Buffer* R = createVertexBuffer(name, vec3::getRA(), nV, V, k, I, GL_TRIANGLE_STRIP);
 
 delete [] I;
 delete [] V;
 
 return R;
}

void Ce2Render::cullFace(GLenum C)
{
 if (_cullState == C) return;

 _cullState = C;

 if (C == GL_NONE)
  glDisable(GL_CULL_FACE);
 else
 {
  glCullFace(C);
  glEnable(GL_CULL_FACE);
 } 
}

void Ce2Render::blend(bool B)
{
 if (_blendEnabled = B)
  glEnable(GL_BLEND);
 else 
  glDisable(GL_BLEND);
}

void Ce2Render::blendFunction(GLenum B0, GLenum B1)
{
 if ( (B0 != nLastB0) || (B1 != nLastB1) )
  {
   nLastB0 = B0;
   nLastB1 = B1;
   glBlendFunc(B0, B1);
  } 
}

Ce2Framebuffer * Ce2Render::createCubemapFramebuffer(string name, int size, int texInternalFormat, 
   int texFormat, int texType, int depthInternalFormat, int depthFormat, int depthType)
{
 Ce2Framebuffer* F = new Ce2Framebuffer();
 
 glGenFramebuffers(1, &(F->nID));
 F->szWidth  = size;
 F->szHeight = size;
 F->name     = name;
 F->bind();

 if (texInternalFormat != 0)
 {
  Ce2TextureObject* color = manager()->genCubeTexture("FBO" + name + "color0", size, size,
   texInternalFormat, texFormat, texType, 0);
  F->addRenderTarget(color);
 } 
 else
 {
  glReadBuffer(GL_NONE);
  glDrawBuffer(GL_NONE);
 }
 
 if (depthInternalFormat != 0)
 {
  Ce2TextureObject* depth = manager()->genCubeTexture("FBO" + name + "depth", size, size,
   depthInternalFormat, depthFormat, depthType, 0);
  F->setDepthTarget(depth); 
 }
 
 F->check();

 Framebuffers.push_back(F);
 bindFramebuffer(0);
 
 return F;
}

Ce2Framebuffer* Ce2Render::createFramebuffer(string name, int width, int height, int texInternalFormat, 
   int texFormat, int texType, int depthInternalFormat, int depthFormat, int depthType)
{
 Ce2Framebuffer * F = new Ce2Framebuffer();
 
 glGenFramebuffers(1, &(F->nID));
 F->szWidth  = width;
 F->szHeight = height;
 F->name     = name;
 F->bind();

 if (texInternalFormat != 0)
 {
  Ce2TextureObject * C = manager()->genTexture2D("FBO " + name + " color 0", F->szWidth, F->szHeight,  texInternalFormat, texFormat, texType, NULL);
  C->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  F->addRenderTarget(C);
 }
 else
 {
   glReadBuffer(GL_NONE);
   glDrawBuffer(GL_NONE);
 }
 
 if (depthInternalFormat != 0)
 {
  Ce2TextureObject * D = manager()->genTexture2D("FBO " + name + " depth",  F->szWidth, F->szHeight,  depthInternalFormat, depthFormat, depthType, NULL);
  D->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  F->setDepthTarget(D);
 }
 
 F->check();
 
 Framebuffers.push_back(F);
 bindFramebuffer(0);
 
 return F;
}

void Ce2Render::bindFramebuffer(Ce2Framebuffer * FBO)
{
 if (!FBO) 
 {
  if (nLastFBO == 0) return;
  
  nLastFBO = 0;
  viewport(core.windowSize);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  checkError("glBindFramebuffer(0)");
 }
 else
 {
  if ( FBO->nID == nLastFBO ) return;
  
  nLastFBO = FBO->nID;
  viewport(FBO->szWidth, FBO->szHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO->nID);
  checkErrorF("glBindFramebuffer", FBO->name);
 } 
}

void Ce2Render::unloadFramebuffer(Ce2Framebuffer *FBO)
{
 if (!FBO) return;
 
 manager()->unloadTexture(FBO->depthBuffer);
 
 for (int i = 0; i < FBO->nTargets; i++)
  manager()->unloadTexture(FBO->renderTarget[i]);
  
 glDeleteFramebuffers(1, &(FBO->nID));
 
 vector<Ce2Framebuffer*>::iterator i = find(Framebuffers.begin(), Framebuffers.end(), FBO);
 if (i != Framebuffers.end()) Framebuffers.erase(i);
 
 delete FBO;
}

void Ce2Render::drawFSQ()
{
 FSQuad->renderAll();
}

void Ce2Render::drawFSQTex(Ce2TextureObject *texture)
{
 bindTexture(0, texture);
 _prog_copy->bind();
 FSQuad->renderAll();
}

void Ce2Render::drawFSQTexScaled(Ce2TextureObject *texture, vec4 scale)
{
 bindTexture(0, texture);
 
 _prog_copy_scale->bind();
 glUniform4fv(_pcs_u, 1, (GLfloat*)&scale);
 
 FSQuad->renderAll();
}

Ce2Buffer* Ce2Render::createPhotonMap(const string& name, const vec2i& size)
{
  int numPhotons = size.x * size.y;
  vec2 texel = vec2(1.0f / size.x, 1.0f / size.y);
  vec2 dxdy = vec2(0.5f / size.x, 0.5f / size.y);

  vec2* photons = new vec2[numPhotons];
  Index* indices = new Index[numPhotons];

  int k = 0;
  for (int i = 0; i < size.y; ++i)
    for (int j = 0; j < size.x; ++j)
    {
      photons[k] = vec2(j * texel.x, i * texel.y) + dxdy;
      indices[k] = k;
      k++;
    }

  Ce2Buffer* photonBuffer = createVertexBuffer(name, vec2::getRA(), numPhotons, photons, numPhotons, indices, GL_POINTS);

  delete [] photons;                 
  delete [] indices; 

  return photonBuffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Ce2Buffer::bind()
{
 if (!render()->supportVertexBuffers())
 {
   int nOffset = 0;
   for (int i = 0; i < nAttribs; i++)
   {
    switch (attribs[i].id)
    {
     case RENDER_ATTRIB_NORMAL :
     {
      glEnableClientState(GL_NORMAL_ARRAY);
	  glNormalPointer(GL_FLOAT, nStructSize, (GLvoid*)(&bufferData[nOffset]));
      break;
     }
     case RENDER_ATTRIB_TEXCOORD0 :
     {
      glActiveTexture(GL_TEXTURE0);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(attribs[i].numFloats, GL_FLOAT, nStructSize, (GLvoid*)(&bufferData[nOffset]));
      break;
     }
     case RENDER_ATTRIB_POSITION :
     {
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(attribs[i].numFloats, GL_FLOAT, nStructSize, (GLvoid*)(&bufferData[nOffset]));
      break;
     }
    }
    nOffset += attribs[i].numFloats;
   }
   return;
 }

 glBindBuffer(GL_ARRAY_BUFFER, nDataID);
 render()->checkErrorF("glBindBuffer<ARRAY>", name);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nElemID);
 render()->checkErrorF("glBindBuffer<ELEMENT>", name);
 
 int nOffset = 0;
 if (render()->supportVertexAttribArrays())
 {
   for (int i = 0; i < nAttribs; i++)
   {
    glEnableVertexAttribArray(attribs[i].id);
     render()->checkErrorF("glEnableVertexAttribArray<" + intToStr(attribs[i].id) +  ">", name);
    glVertexAttribPointer(attribs[i].id, attribs[i].numFloats, GL_FLOAT, false, nStructSize, (GLvoid*)(nOffset * sizeof(float)));
     render()->checkErrorF("glVertexAttribPointer<" + intToStr(attribs[i].id) +  ">", name);
    nOffset += attribs[i].numFloats;
   }
 }
 else
 {
   for (int i = 0; i < nAttribs; i++)
   {
    switch (attribs[i].id)
    {
     case RENDER_ATTRIB_NORMAL :
     {
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, nStructSize, (GLvoid*)(nOffset * sizeof(float)));
      break;
     }
     case RENDER_ATTRIB_TEXCOORD0 :
     {
      glActiveTexture(GL_TEXTURE0);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(attribs[i].numFloats, GL_FLOAT, nStructSize, (GLvoid*)(nOffset * sizeof(float)));
      break;
     }
     case RENDER_ATTRIB_POSITION :
     {
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(attribs[i].numFloats, GL_FLOAT, nStructSize, (GLvoid*)(nOffset * sizeof(float)));
      break;
     }
    }
    nOffset += attribs[i].numFloats;
   }
 }
 
}

void Ce2Buffer::unbind()
{
 if (!render()->supportVertexBuffers())
 {
   for (int i = 0; i < nAttribs; i++)
   {
    switch (attribs[i].id)
    {
     case RENDER_ATTRIB_NORMAL : glDisableClientState(GL_NORMAL_ARRAY); break;
     case RENDER_ATTRIB_POSITION : glDisableClientState(GL_VERTEX_ARRAY); break;
     case RENDER_ATTRIB_TEXCOORD0 :  glDisableClientState(GL_TEXTURE_COORD_ARRAY); break;
    }
   }
   return;
 }

 if (render()->supportVertexAttribArrays())
 {
  for (int i = 0; i < nAttribs; ++i) 
   glDisableVertexAttribArray(attribs[i].id);
 }
 else
 {
   for (int i = 0; i < nAttribs; i++)
   {
    switch (attribs[i].id)
    {
     case RENDER_ATTRIB_NORMAL : glDisableClientState(GL_NORMAL_ARRAY); break;
     case RENDER_ATTRIB_POSITION : glDisableClientState(GL_VERTEX_ARRAY); break;
     case RENDER_ATTRIB_TEXCOORD0 :  glDisableClientState(GL_TEXTURE_COORD_ARRAY); break;
    }
   }
 }
    
  
 glBindBuffer(GL_ARRAY_BUFFER, 0); 
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
}

void Ce2Buffer::drawAllElements()
{
 if (render()->supportVertexBuffers())
  DrawElements(nDrawMode, nIndices, GL_UNSIGNED_INT, NULL);
 else
  DrawElements(nDrawMode, nIndices, GL_UNSIGNED_INT, (GLvoid*)indexData);
}

void Ce2Buffer::renderAll()
{
 bind();
 drawAllElements();
 unbind();
}

void Ce2Buffer::drawElements(Index mode, Index count, Index offset)
{
 if (render()->supportVertexBuffers())
  DrawElements(mode, count, GL_UNSIGNED_INT, (GLvoid*)(offset * sizeof(Index)));
 else
  DrawElements(mode, count, GL_UNSIGNED_INT, (GLvoid*)(&indexData[offset]));
}

///////////////////////////////////////////////////////////////////////////////////////////
Ce2Framebuffer::Ce2Framebuffer()
{
 nID = nTargets = szWidth = szHeight = 0;
 for (int i = 0; i < 4; ++i)
 {
  renderTarget[4*i+0] = NULL;
  renderTarget[4*i+1] = NULL;
  renderTarget[4*i+2] = NULL;
  renderTarget[4*i+3] = NULL;
 } 
 depthBuffer = NULL;
}

void Ce2Framebuffer::bind()
{
 render()->bindFramebuffer(this);
}

bool Ce2Framebuffer::check()
{
 bind();
 int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
 
 if (status != GL_FRAMEBUFFER_COMPLETE)
 {
  string info = FramebufferStatusToString(status) + " for " + name;
  render()->log(info);
 }
 
 return (status == GL_FRAMEBUFFER_COMPLETE);
}

bool Ce2Framebuffer::addRenderTarget(Ce2TextureObject* rt)
{
 if ( (!rt) || (rt->width != szWidth) || (rt->height != szHeight)) return false;
 
 bind();
 glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nTargets, rt->glID, 0);
 renderTarget[nTargets++] = rt;

 /*
 if (rt->target == GL_TEXTURE_2D)
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nTargets, rt->target, rt->glID, 0);
 else   
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nTargets, rt->glID, 0);
 */ 
 return check();
}

bool Ce2Framebuffer::setDepthTarget(Ce2TextureObject * rt)
{
 if ( (!rt) || (rt->width != szWidth) || (rt->height != szHeight)) return false;
 
 bind();
 glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, rt->glID, 0);
 depthBuffer = rt;
/*
 if (rt->target == GL_TEXTURE_2D)
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, rt->target, rt->glID, 0);
 else
*/    
 return check();
}

void Ce2Framebuffer::addSameRendertarget()
{
 int k = nTargets - 1;
 
 Ce2TextureObject * tex = manager()->genTexture2D(name + "rendertarget" + intToStr(nTargets), 
  renderTarget[k]->width, renderTarget[k]->height, renderTarget[k]->internalFormat,
  renderTarget[k]->format, renderTarget[k]->type, 0);
 tex->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE); 
  
 addRenderTarget( tex );
}

void Ce2Framebuffer::setCurrentRenderTarget(Ce2TextureObject* texture)
{
 bind();
 render()->viewport(texture->width, texture->height);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->target, texture->glID, 0);
 check();
}

void Ce2Framebuffer::setCurrentRenderTarget(Ce2TextureObject *texture, GLenum target)
{
 bind();
 render()->viewport(texture->width, texture->height);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture->glID, 0);
 check();
}

void Ce2Framebuffer::setCurrentRenderTarget(int rt)
{
 if (rt < nTargets)
  setCurrentRenderTarget(renderTarget[rt]);
}

void Ce2Framebuffer::setDrawBuffersCount(int c)
{
 bind();
  glDrawBuffers(c, RENDERBUFFERS_ENUM);
 check();
}