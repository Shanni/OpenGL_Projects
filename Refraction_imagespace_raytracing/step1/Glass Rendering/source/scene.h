#include <e2core.h>
#include <e2scene.h>
#include <e2render.h>
#include <e2geometry.h>
#include <e2manager.h>
#include <e2gui.h>

namespace et
{

#define NUM_ENV_OBJECTS 10
/*
#define CUBEMAP_SIZE    1024
#define SHADOWMAP_SIZE  1024
#define PHOTONMAP_SIZE  1024
*/
#define TEXTURE_FORMAT  GL_RGB16F
#define TEXTURE_TYPE    GL_HALF_FLOAT

class ApplicationScene : public Ce2Scene
{
 public:
  void load();
  void unload();
  void update();
  void renderScene();
  void onKeyDown(unsigned char key);

 private:
  void renderGUI();
  void applyPostprocess();
  void loadModel(const std::string& fileName);

  Ce2TextureObject* luminanceTexture() const { return _adaptationTexture[_adaptationIndex]; }
  Ce2TextureObject* nextLuminanceTexture() const { return _adaptationTexture[!_adaptationIndex]; }

  void renderEnvironmentToDepth(const vec3& cameraPosition, const mat4& modelViewProjection);
  void renderEnvironmentToCubeMap(const vec3& cameraPosition);
  void renderEnvironmentToDepthCubeMap(const vec3& cameraPosition);
  void renderEnvironment(const vec3& cameraPosition, const mat4& modelViewProjection);
  
  void prerenderGlass();
  void renderGlass();                         
  void renderShadowmap();

  void prerenderCaustic();
  void renderCaustic();

  void renderEnvObjects(Ce2ProgramObject* program);

 private:
  Ce2GUI* _gui;
  Ce2Font* _primaryFont;
  Ce2Camera* _cubemapCamera;
  Ce2Light* _lightSource0;

  Ce2ProgramObject* _indoorProgram;
  Ce2ProgramObject* _envProgram;
  Ce2ProgramObject* _lightProgram;

  Ce2ProgramObject* _indoorGSProgram;
  Ce2ProgramObject* _envGSProgram;

  Ce2ProgramObject* _depthRenderProgram;

  Ce2ProgramObject* _distanceRenderProgram;
  Ce2ProgramObject* _distanceRenderGSProgram;

  Ce2ProgramObject* _glassSingleRefProgram;
  Ce2ProgramObject* _glassDoubleRefProgram;

  Ce2ProgramObject* _downsampleProgram;
  Ce2ProgramObject* _brightpassProgram;
  Ce2ProgramObject* _blurProgram;
  Ce2ProgramObject* _finalPassProgram;
  Ce2ProgramObject* _adaptationProgram;
  Ce2ProgramObject* _backfaceProgram;
  Ce2ProgramObject* _noiseReductionProgram;

  Ce2ProgramObject* _causticSingleRefProgram;
  Ce2ProgramObject* _causticDoubleRefProgram;

  Ce2TextureObject* _reflectionRefractionTexture; // cubemap
  Ce2TextureObject* _shadowmapTexture; // cubemap

  Ce2TextureObject* _floorTexture;
  Ce2TextureObject* _brickTexture;
  Ce2TextureObject* _postprocessTextures[16];
  Ce2TextureObject* _bloomTexture;
  Ce2TextureObject* _adaptationTexture[2];

  Ce2Framebuffer* _reflectionRefractionBuffer; // plain
  Ce2Framebuffer* _reflectionRefractionCubemapBuffer; // cubemap

  Ce2Framebuffer* _shadowBuffer; // plain
  Ce2Framebuffer* _shadowCubemapBuffer; // cubemap

  Ce2Framebuffer* _frontfaceBuffer;
  Ce2Framebuffer* _backfaceBuffer;

  Ce2Framebuffer* _screenBuffer;
  Ce2Framebuffer* _postprocessBuffer;
  Ce2Framebuffer* _causticBuffer;
  Ce2Framebuffer* _sceneDepthBuffer;

  Ce2Buffer* _floor;
  Ce2Buffer* _envCube;
  Ce2Buffer* _envSphere;
  Ce2Buffer* _glassObject;
  Ce2Buffer* _photonMap;

  bool _applyPostprocess;
  bool _adaptationIndex;
  bool _useGeometryShader;
  bool _doubleRefractionGlass;
  bool _doubleRefractionCaustic;
  bool _drawInfo;
  int _numPPTextures;
  int _cubemapSize;
  int _shadowmapSize;
  int _causticmapSize;
  float _materialIOR;
  vec3 _lightColor0;
  vec3 _modelCenter;
  mat4 _envTransforms[NUM_ENV_OBJECTS];
  mat4 _cubemapProjectionMatrix;
  mat4 _modelTransformation;
  CubemapMatrixArray _cubemapMatrices;
  string _modelName;
};

}