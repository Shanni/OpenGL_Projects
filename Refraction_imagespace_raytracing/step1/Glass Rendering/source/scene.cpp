#include "scene.h"
#include <e2core.h>

using namespace et;

const vec3 LightColor0 = vec3(1.0);
const string config_shadowmap_size = "shadow-map-size:";
const string config_causticmap_size = "caustic-map-size:";
const string config_cubemap_size = "env-map-size:";
const string config_model_name = "model-name:";

void ApplicationScene::load()
{
 srand((unsigned)time(0));

 _shadowmapSize = 256;
 _causticmapSize = 256;
 _cubemapSize = 256;
 _modelName = "use_sphere";

 string configfile_path = core.appPath + "..\\config\\appconfig";
 if (fileExists(configfile_path))
 {
  ifstream config_file(configfile_path);
  string param_name;
  string value;
  while (!config_file.eof())
  {
   config_file >> param_name >> value;
   if (param_name == config_shadowmap_size)  _shadowmapSize = strToInt(value);
   if (param_name == config_causticmap_size) _causticmapSize = strToInt(value);
   if (param_name == config_cubemap_size)    _cubemapSize = strToInt(value);
   if (param_name == config_model_name)      _modelName = value;
  }
  config_file.close();
 }

 _gui = new Ce2GUI();
 _primaryFont = new Ce2Font("Consolas", 16, ET_FONT_BOLD);

 render()->blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 render()->camera->perspective(QUAD_PI, core.windowAspect, 1.0, 4096.0);
 render()->camera->lookAt(vec3(400.0, 150.0, 0.0), vec3(0.0, 50.0, 0.0), vec3(0.0, 1.0, 0.0));
 render()->camera->lockUpVector = true;
 render()->blend(false);
 render()->cullFace(GL_BACK);

 _screenBuffer = render()->createFramebuffer("Screen", core.windowSize.x, core.windowSize.y, TEXTURE_FORMAT, GL_RGB, TEXTURE_TYPE);

 _sceneDepthBuffer = render()->createFramebuffer("Env depth", _causticmapSize, _causticmapSize, 0, 0, 0);

 _causticBuffer = render()->createFramebuffer("Caustic map", _causticmapSize, _causticmapSize, GL_R16F, GL_RGB, GL_HALF_FLOAT, 0, 0, 0);
 _causticBuffer->addSameRendertarget();
 _causticBuffer->renderTarget[0]->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
 _causticBuffer->renderTarget[1]->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

 _reflectionRefractionCubemapBuffer = render()->createCubemapFramebuffer("Cubemap single buffer", _cubemapSize, TEXTURE_FORMAT, GL_RGB, TEXTURE_TYPE);
 _shadowCubemapBuffer = render()->createCubemapFramebuffer("Cubemap double buffer", _shadowmapSize, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);

 _backfaceBuffer = render()->createFramebuffer("Backface",  _causticmapSize, _causticmapSize, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
 _frontfaceBuffer = render()->createFramebuffer("Frontface", _causticmapSize, _causticmapSize, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);

 _screenBuffer->renderTarget[0]->setFiltration(GL_NEAREST, GL_NEAREST);
 _screenBuffer->depthBuffer->setFiltration(GL_NEAREST, GL_NEAREST);
 _backfaceBuffer->renderTarget[0]->setFiltration(GL_NEAREST, GL_NEAREST);
 _backfaceBuffer->depthBuffer->setFiltration(GL_NEAREST, GL_NEAREST);
 _frontfaceBuffer->renderTarget[0]->setFiltration(GL_NEAREST, GL_NEAREST);
 _frontfaceBuffer->depthBuffer->setFiltration(GL_NEAREST, GL_NEAREST);

 _postprocessBuffer = render()->createFramebuffer("PPBuffer", core.windowSize.x / 2, core.windowSize.y / 2, TEXTURE_FORMAT, GL_RGB, TEXTURE_TYPE, 0, 0, 0);
 int sx = core.windowSize.x / 4;
 int sy = core.windowSize.y / 4;
 int i = 0;
 while ( (sx >= 1) && (sy >= 1) )
 {
  _postprocessTextures[i] = manager()->genTexture2D("_pptexture" + intToStr(i), sx, sy, TEXTURE_FORMAT, GL_RGB, TEXTURE_TYPE, 0);
  _postprocessTextures[i]->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE); 

  if (i == 0)
  {
   _bloomTexture = manager()->genTexture2D("bloom", sx, sy, TEXTURE_FORMAT, GL_RGB, TEXTURE_TYPE, 0);
   _bloomTexture->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  }

  sx /= 2;
  sy /= 2;

  if ((sx == 0) && (sy != 0)) sx = 1;
  if ((sy == 0) && (sx != 0)) sy = 1;

  i++;
 }
 _numPPTextures = i;

 _adaptationTexture[0] = manager()->genTexture2D("adaptation texture 1", 1, 1, TEXTURE_FORMAT, GL_RGB, TEXTURE_TYPE, 0);
 _adaptationTexture[1] = manager()->genTexture2D("adaptation texture 2", 1, 1, TEXTURE_FORMAT, GL_RGB, TEXTURE_TYPE, 0);
 _adaptationIndex = 0;

 _reflectionRefractionBuffer = render()->createFramebuffer("Cubemap framebuffer", _cubemapSize, _cubemapSize, 0, 0, 0); 
 _reflectionRefractionTexture = manager()->genCubeTexture("Env cubemap", _cubemapSize, _cubemapSize, TEXTURE_FORMAT, GL_RGB, TEXTURE_TYPE, 0);

 _shadowBuffer = render()->createFramebuffer("Shadowmap buffer", _shadowmapSize, _shadowmapSize, 0, 0, 0);
 _shadowmapTexture = manager()->genCubeTexture("Shadowmap cube texture", _shadowmapSize, _shadowmapSize, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 0);

 _lightSource0 = new Ce2Light();
 _lightSource0->projectionMatrix() = IDENTITY_MATRIX;
 _lightSource0->perspective(QUAD_PI, 1.0, 100.0, 1500.0);
 _lightColor0 = vec3(1.0, 1.0, 1.0);

 string texture_names[10];
 int texture_units[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

 texture_names[0] = "environment_map";
 texture_names[1] = "backface_texture";
 texture_names[2] = "backface_depth";
 texture_names[3] = "environment_depth";

 _glassSingleRefProgram = manager()->loadProgram("glass\\glass.program");
 _glassSingleRefProgram->setUniformTextures(1, texture_names, texture_units);

 _glassDoubleRefProgram = manager()->loadProgram("glass\\glass.program", "DOUBLE_REFRACTION");
 _glassDoubleRefProgram->setUniformTextures(3, texture_names, texture_units);

 _backfaceProgram = manager()->loadProgram("glass\\backface_normals.program");

 _depthRenderProgram = manager()->loadProgram("env\\depth_render.program");
 _distanceRenderGSProgram = manager()->loadProgram("env\\distance_render_gs.program", "WITH_GS");
 _distanceRenderProgram = manager()->loadProgram("env\\distance_render.program");

 texture_names[0] = "diffuse_texture";
 texture_names[1] = "cubemap_shadow";
 texture_names[2] = "caustic_texture";

 _envProgram = manager()->loadProgram("env\\env.program");
 _envProgram->setUniformTextures(3, texture_names, texture_units);

 _envGSProgram = manager()->loadProgram("env\\env_gs.program", "WITH_GS");
 _envGSProgram->setUniformTextures(3, texture_names, texture_units);

 _indoorProgram = manager()->loadProgram("env\\indoor.program");
 _indoorProgram->setUniformTextures(3, texture_names, texture_units);

 _indoorGSProgram = manager()->loadProgram("env\\indoor_gs.program", "WITH_GS");
 _indoorGSProgram->setUniformTextures(3, texture_names, texture_units);

 _lightProgram = manager()->loadProgram("light\\light.program");
 _downsampleProgram = manager()->loadProgram("postprocess\\downsample.program");
 _blurProgram = manager()->loadProgram("postprocess\\linearBlur.program");
 _noiseReductionProgram = manager()->loadProgram("postprocess\\noiseReduction.program");

 texture_names[0] = "refractive_normals";
 texture_names[1] = "refractive_depth";
 texture_names[2] = "receiver_depth";
 texture_names[3] = "refractive_backface_normals";
 texture_names[4] = "refractive_backface_depth";
 _causticSingleRefProgram = manager()->loadProgram("caustic\\single_refraction_tex2d.program");
 _causticSingleRefProgram->setUniformTextures(3, texture_names, texture_units);
 _causticDoubleRefProgram = manager()->loadProgram("caustic\\single_refraction_tex2d.program", "DOUBLE_REFRACTION");
 _causticDoubleRefProgram->setUniformTextures(5, texture_names, texture_units);

 texture_names[0] = "source_image";
 texture_names[1] = "luminocity_texture";
 texture_names[2] = "bloom_image";

 _brightpassProgram = manager()->loadProgram("postprocess\\brightpass.program");
 _brightpassProgram->setUniformTextures(2, texture_names, texture_units);

 _finalPassProgram = manager()->loadProgram("postprocess\\hdrFinalPass.program");
 _finalPassProgram->setUniformTextures(3, texture_names, texture_units);

 texture_names[0] = "new_value";
 texture_names[1] = "old_value";

 _adaptationProgram = manager()->loadProgram("postprocess\\adaptation.program");
 _adaptationProgram->setUniformTextures(2, texture_names, texture_units);

 _floorTexture = manager()->loadTexture("floor.jpg");
 _brickTexture = manager()->loadTexture("brick.jpg");

 _floor = render()->createBox("room", vec3(1024.0, 1024.0, 1024.0), false);
 _envCube = render()->createBox("env cube ", vec3(1.0), false);
 _envSphere = render()->createSphere("env sphere", 5.0, 9, 9);

 for (int i = 0; i < NUM_ENV_OBJECTS; ++i)
 {
  vec3 dim = vec3(25.0f + 25.0f * rand() / RAND_MAX);
  
  vec3 pos = fromSpherical(0.0f, DOUBLE_PI * rand() / RAND_MAX);
  pos = vec3(250.0f + 500.0f * rand() / RAND_MAX) * pos.normalize();
  pos.y = dim.y;
  _envTransforms[i] = translationScaleMatrix(pos, dim);
 }

 loadModel(_modelName);

 _cubemapProjectionMatrix = Ce2Camera::perspectiveProjection(HALF_PI, 1.0, 1.0, 2048.0);
 _cubemapMatrices = _cubemapCamera->cubemapMatrix(_cubemapProjectionMatrix, _modelCenter);

 _photonMap = render()->createPhotonMap("Photon map", vec2i(_causticmapSize));

 _materialIOR = 1.41f;
 _applyPostprocess = false;
 _useGeometryShader = true;
 _doubleRefractionGlass = true;
 _drawInfo = false;
}

void ApplicationScene::unload()
{
 render()->unloadFramebuffer(_screenBuffer);
 render()->unloadFramebuffer(_postprocessBuffer);
 render()->unloadFramebuffer(_reflectionRefractionBuffer);
 render()->unloadFramebuffer(_backfaceBuffer);
 render()->unloadFramebuffer(_frontfaceBuffer);
 render()->unloadFramebuffer(_sceneDepthBuffer);
 render()->unloadFramebuffer(_causticBuffer);
 render()->unloadFramebuffer(_reflectionRefractionCubemapBuffer);
 render()->unloadFramebuffer(_shadowCubemapBuffer);
 render()->unloadFramebuffer(_shadowBuffer);
 delete _lightSource0;
 delete _primaryFont;
 delete _gui;
}

void ApplicationScene::update()
{
 float t = core.runTime / 10.0f;
 vec3 lp = vec3(384.0, _modelCenter.y + 200.0f, 384.0) * vec3(cos(t), 1.0f + 0.25f * cos(t), sin(t));

 _lightSource0->lookAt( lp, _modelCenter, vec3(0.0, 1.0, 0.0) ); 
 _modelTransformation = rotationYXZMatrix(0.0, -2.0f * t, 0.0) * translationMatrix(_modelCenter);
 _cubemapMatrices = _cubemapCamera->cubemapMatrix(_cubemapProjectionMatrix, _modelCenter);

 _envTransforms[0][3][2] = sin(t / 10.0f) * 500.0f;

 performDefaultCameraControl(2.0);
}

void ApplicationScene::onKeyDown(unsigned char key)
{
 if (VK_ESCAPE == key)
 {
  core.quit();
 }

 if (key == 192)
 {
  _drawInfo = !_drawInfo;
 }

 if (VK_ADD == key)
 {
  _materialIOR = min(2.5f, _materialIOR + 0.01f);
 }

 if (VK_SUBTRACT == key)
 {
  _materialIOR = max(1.01f, _materialIOR - 0.01f);
 }

 if (VK_F1 == key)
 {
  _applyPostprocess = !_applyPostprocess;
  _lightColor0 = (1.0f + 99.0f * float(_applyPostprocess)) * LightColor0;
 }

 if (VK_F2 == key)
  _useGeometryShader = !_useGeometryShader;

 if (VK_F3 == key)
  _doubleRefractionGlass = !_doubleRefractionGlass;

 if (VK_F4 == key)
  _doubleRefractionCaustic = !_doubleRefractionCaustic;
}

void ApplicationScene::renderScene()
{
 renderShadowmap();

 prerenderCaustic();
 renderCaustic();

 prerenderGlass();

 if (_applyPostprocess)
  render()->bindFramebuffer(_screenBuffer);
 else
  render()->bindFramebuffer(0);

 glClear(GL_DEPTH_BUFFER_BIT);

 renderEnvironment(render()->camera->position(), render()->camera->mvpMatrix());
 renderGlass();

 _gui->beginRenderUI();
 render()->blend(false);

 if (_applyPostprocess)
 {
  applyPostprocess();
  render()->bindFramebuffer(0);

  _finalPassProgram->bind();
  render()->bindTexture(0, keyPressed('1') ? 0 : _screenBuffer->renderTarget[0]);
  render()->bindTexture(1, luminanceTexture());
  render()->bindTexture(2, keyPressed('2') ? 0 : _bloomTexture);
  render()->drawFSQ();
 };

 renderGUI();
 _gui->endRenderUI();
}

void ApplicationScene::renderGUI()
{
 float scale = core.windowAspect;
 float sx = 0.25f * float(_causticmapSize) / core.windowSize.x;
 float sy = sx * scale;

// render()->drawFSQTexScaled(_backfaceBuffer->renderTarget[0], vec4( -1.0f + 1.0f * sx, -1.0f + sy, sx, sy));
// render()->drawFSQTexScaled(_frontfaceBuffer->renderTarget[0], vec4( -1.0f + 3.0f * sx, -1.0f + sy, sx, sy));
// render()->drawFSQTexScaled(_sceneDepthBuffer->depthBuffer, vec4( -1.0f + 5.0f * sx, -1.0f + sy, sx, sy));
// render()->drawFSQTexScaled(_causticBuffer->renderTarget[0], vec4( -1.0f + sx, -1.0f + sy, sx, sy));

 if (_drawInfo)
 {
  render()->blend(true);
  _gui->renderText(_primaryFont, core.fpsString(), 2, 2);
  _gui->renderText(_primaryFont, "(+/-) Index of refraction = " + floatToStr(_materialIOR, 4), 2, 20);

  int y = 20;
  string header = " (F1) HDR: ";
  _gui->renderText(_primaryFont, header + (_applyPostprocess ? "ON" : "OFF"), 2, y += 20);
  header = " (F2) Geometry shaders: ";
  _gui->renderText(_primaryFont, header + (_useGeometryShader ? "ON" : "OFF"), 2, y += 20);
  header = " (F3) Two refractions on glass: ";
  _gui->renderText(_primaryFont, header + (_doubleRefractionGlass ? "ON" : "OFF"), 2, y += 20);
  header = " (F4) Two refractions on caustic: ";
  _gui->renderText(_primaryFont, header + (_doubleRefractionCaustic ? "ON" : "OFF"), 2, y += 20);
 }
}

void ApplicationScene::applyPostprocess()
{
 render()->bindFramebuffer(_postprocessBuffer);
 _postprocessBuffer->setCurrentRenderTarget(0);

 _downsampleProgram->bind();
 _downsampleProgram->setUniform("vTexel", _screenBuffer->renderTarget[0]->texel);
 render()->bindTexture(0, _screenBuffer->renderTarget[0]);
 render()->drawFSQ();

 render()->bindTexture(0, _postprocessBuffer->renderTarget[0]);
 _downsampleProgram->setUniform("vTexel", _postprocessBuffer->renderTarget[0]->texel);
 for (int i = 0; i < _numPPTextures; ++i)
 {
  _postprocessBuffer->setCurrentRenderTarget(_postprocessTextures[i]);

  if (i != 0)
  {
   render()->bindTexture(0, _postprocessTextures[i - 1]);
  _downsampleProgram->setUniform("vTexel", _postprocessTextures[i - 1]->texel);
  }

  render()->drawFSQ();
 }

 _postprocessBuffer->setCurrentRenderTarget(luminanceTexture());
 _adaptationProgram->bind();
 _adaptationProgram->setUniform("time", 2.0f * core.frameTime);
 render()->bindTexture(0, _postprocessTextures[_numPPTextures - 1]);
 render()->bindTexture(1, nextLuminanceTexture());
 render()->drawFSQ();

 _postprocessBuffer->setCurrentRenderTarget(_bloomTexture);
 _brightpassProgram->bind();
 render()->bindTexture(0, _postprocessTextures[0]);
 render()->bindTexture(1, luminanceTexture());
 render()->drawFSQ();

 const float blurRadius = 5.0;

 _blurProgram->bind();

 _postprocessBuffer->setCurrentRenderTarget(_postprocessTextures[0]);
 _blurProgram->setUniform("texel_radius", vec3( _postprocessTextures[0]->texel.x, 0.0, blurRadius ));
 render()->bindTexture(0, _bloomTexture);
 render()->drawFSQ();

 _postprocessBuffer->setCurrentRenderTarget(_bloomTexture);
 _blurProgram->setUniform("texel_radius", vec3( 0.0, _postprocessTextures[0]->texel.y, blurRadius ));
 render()->bindTexture(0, _postprocessTextures[0]);
 render()->drawFSQ();

 _adaptationIndex = !_adaptationIndex;
}

void ApplicationScene::loadModel(const std::string& fileName)
{
  string fname = manager()->findFile(fileName);

  if (fileExists(fname))
  {
    FILE* file = 0;
    if (fopen_s(&file, fname.c_str(), "rb") != 0) return;

    vec3 min_vert;
    vec3 max_vert;
    fread(&min_vert, 1, sizeof(vec3), file);
    fread(&max_vert, 1, sizeof(vec3), file);

    vec3 size = (max_vert - min_vert);
    _modelCenter = (max_vert + min_vert) * 0.5; 

    float size_mag = size.length();
    float m_scale = 250.0f / size_mag;

    int icount = 0; 
    fread(&icount, 1, sizeof(int), file);
    Index* inds = new Index[icount]; 
    fread(inds, 1, sizeof(GLuint) * icount, file);

    int vcount = 0;
    fread(&vcount, 1, sizeof(int), file);
    VERT_V3_N3* verts = new VERT_V3_N3[vcount];
    fread(verts, 1, sizeof(VERT_V3_N3) * vcount, file);
    fclose(file); 

    vec3 max_new = vec3(-10000.0);
    vec3 min_new = vec3( 10000.0);
    for (int i = 0; i < vcount; ++i)
    {
      verts[i].vPosition = m_scale * (verts[i].vPosition - _modelCenter);
      max_new = maxv(max_new, verts[i].vPosition);
      min_new = minv(min_new, verts[i].vPosition);
    }

    _modelCenter *= m_scale;
    _modelCenter += vec3(0.0, 5.0, 0.0);

    _glassObject = render()->createVertexBuffer("model", VERT_V3_N3::getRA(), vcount, verts, icount, inds, GL_TRIANGLES);
    delete verts;
    delete inds;
  }
  else 
  {
    float radius = 50.0;
    _modelCenter = vec3(0.0, radius, 0.0);
    _glassObject = render()->createSphere("model", radius, 36, 36);
    /*
    _glassObject = render()->createPlane("model", vec3(-100.0, radius, -100.0), 
                                                  vec3( 100.0, radius, -100.0), 
                                                  vec3( 100.0, radius,  100.0), 
                                                  vec3(-100.0, radius,  100.0), 
                                                  vec2i(100) );
    */
  }
}

void ApplicationScene::renderEnvObjects(Ce2ProgramObject* program)
{
 _envCube->bind();
 for (int i = 0; i < NUM_ENV_OBJECTS; ++i)
 {
  program->setUniform("mTransform", _envTransforms[i]);
  _envCube->drawAllElements();
 }
 _envCube->unbind();
}

void ApplicationScene::renderEnvironmentToDepth(const vec3& cameraPosition, const mat4& modelViewProjection)
{
 _depthRenderProgram->bind();
 _depthRenderProgram->setMVPMatrix(modelViewProjection);
 _depthRenderProgram->setUniform("mTransform", translationMatrix(0.0, 1024.0, 0.0));

 render()->cullFace(GL_FRONT);
 _floor->renderAll();
 render()->cullFace(GL_BACK);

 renderEnvObjects(_depthRenderProgram);
}


void ApplicationScene::renderEnvironmentToCubeMap(const vec3& cameraPosition)
{
 render()->bindTexture(1, _useGeometryShader ? _shadowCubemapBuffer->renderTarget[0] : _shadowmapTexture);
 render()->bindTexture(2, _causticBuffer->renderTarget[1]);

 render()->cullFace(GL_FRONT);
 render()->bindTexture(0, _floorTexture);

 _indoorGSProgram->bind();
 _indoorGSProgram->setPrimaryLightPosition(_lightSource0->position());
 _indoorGSProgram->setLightProjectionMatrix(_lightSource0->mvpMatrix());
 _indoorGSProgram->setUniform("mModelViewProjection", _cubemapMatrices.ptr(), 6);
 _indoorGSProgram->setUniform("mTransform", translationMatrix(0.0, 1024.0, 0.0));
 _indoorGSProgram->setUniform("cLightColor", _lightColor0);
 _floor->renderAll();

 render()->cullFace(GL_BACK);

 render()->bindTexture(0, manager()->DEFTEX_white);
 _envSphere->renderAll();

 render()->bindTexture(0, _brickTexture);

 _envGSProgram->bind();
 _envGSProgram->setCameraPosition(cameraPosition);
 _envGSProgram->setLightProjectionMatrix(_lightSource0->mvpMatrix());
 _envGSProgram->setPrimaryLightPosition(_lightSource0->position());
 _envGSProgram->setUniform("mModelViewProjection", _cubemapMatrices.ptr(), 6);
 _envGSProgram->setUniform("cLightColor", _lightColor0);
 renderEnvObjects(_envGSProgram);
}

void ApplicationScene::renderEnvironmentToDepthCubeMap(const vec3& cameraPosition)
{
 CubemapMatrixArray cm_matrix = Ce2Camera::cubemapMatrix(_cubemapProjectionMatrix, cameraPosition);

 glClearColor(1.0e+5, 1.0e+5, 1.0e+5, 0.0);
 render()->bindFramebuffer(_shadowCubemapBuffer);
 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
 glClearColor(0.0, 0.0, 0.0, 0.0);

 glPolygonOffset(4, 4);
 glEnable(GL_POLYGON_OFFSET_FILL);

 _distanceRenderGSProgram->bind();
 _distanceRenderGSProgram->setPrimaryLightPosition(_lightSource0->position());
 _distanceRenderGSProgram->setUniform("mModelViewProjection", cm_matrix.ptr(), 6);
 renderEnvObjects(_distanceRenderGSProgram);

 glDisable(GL_POLYGON_OFFSET_FILL);
}

void ApplicationScene::renderEnvironment(const vec3& cameraPosition, const mat4& modelViewProjection)
{
 render()->bindTexture(1, _useGeometryShader ? _shadowCubemapBuffer->renderTarget[0] : _shadowmapTexture);
 render()->bindTexture(2, _causticBuffer->renderTarget[1]);
 render()->cullFace(GL_FRONT);

 _indoorProgram->bind();
 _indoorProgram->setMVPMatrix(modelViewProjection);
 _indoorProgram->setPrimaryLightPosition(_lightSource0->position());
 _indoorProgram->setLightProjectionMatrix(_lightSource0->mvpMatrix());
 _indoorProgram->setUniform("mTransform", translationMatrix(0.0, 1024.0, 0.0));
 _indoorProgram->setUniform("cLightColor", _lightColor0);

 render()->bindTexture(0, _floorTexture);
 _floor->renderAll();

 render()->cullFace(GL_BACK);
 render()->bindTexture(0, _brickTexture);

 _envProgram->bind();
 _envProgram->setCameraPosition(cameraPosition);
 _envProgram->setMVPMatrix(modelViewProjection);
 _envProgram->setPrimaryLightPosition(_lightSource0->position());
 _envProgram->setLightProjectionMatrix(_lightSource0->mvpMatrix());
 _envProgram->setUniform("cLightColor", _lightColor0);
 renderEnvObjects(_envProgram);

 _lightProgram->bind();
 _lightProgram->setMVPMatrix(modelViewProjection);
 _lightProgram->setUniform("mTransform", translationMatrix(_lightSource0->position()));
 _lightProgram->setUniform("cLightColor", _lightColor0);
 _envSphere->renderAll();
}

void ApplicationScene::prerenderCaustic()
{
 render()->bindFramebuffer(_sceneDepthBuffer);
 glClear(GL_DEPTH_BUFFER_BIT);
 renderEnvironmentToDepth(_lightSource0->position(), _lightSource0->mvpMatrix());

 render()->bindFramebuffer(_frontfaceBuffer);
 glClear(GL_DEPTH_BUFFER_BIT);

 _glassObject->bind();

 _backfaceProgram->bind();
 _backfaceProgram->setMVPMatrix(_lightSource0->mvpMatrix());
 _backfaceProgram->setUniform("mTransform", _modelTransformation);
 _backfaceProgram->setUniform("indexOfRefraction", 1.0f / _materialIOR);
 _glassObject->drawAllElements();

 render()->bindFramebuffer(_backfaceBuffer);
 glClear(GL_DEPTH_BUFFER_BIT);

 _backfaceProgram->bind();
 _backfaceProgram->setMVPMatrix(_lightSource0->mvpMatrix());
 _backfaceProgram->setUniform("mTransform", _modelTransformation);
 _backfaceProgram->setUniform("indexOfRefraction", _materialIOR);

 render()->cullFace(GL_FRONT);
 _glassObject->drawAllElements();
 render()->cullFace(GL_BACK);

 _glassObject->unbind();
}

void ApplicationScene::renderCaustic()
{
 render()->bindFramebuffer(_causticBuffer);
 _causticBuffer->setCurrentRenderTarget(0);

 glClear(GL_COLOR_BUFFER_BIT);

 glDepthMask(false);
 glDepthFunc(GL_ALWAYS);
 render()->blendFunction(GL_ONE, GL_ONE);
 render()->blend(true);

 render()->bindTexture(0, _frontfaceBuffer->renderTarget[0]);
 render()->bindTexture(1, _frontfaceBuffer->depthBuffer);
 render()->bindTexture(2, _sceneDepthBuffer->depthBuffer);
 if (_doubleRefractionCaustic)
 {
  render()->bindTexture(3, _backfaceBuffer->renderTarget[0]);
  render()->bindTexture(4, _backfaceBuffer->depthBuffer);
 }
 Ce2ProgramObject* caustic = _doubleRefractionCaustic ? _causticDoubleRefProgram : _causticSingleRefProgram;

 caustic->bind();
 caustic->setCameraPosition(_lightSource0->position());
 caustic->setMVPMatrix(_lightSource0->mvpMatrix());
 caustic->setUniform("mInverseModelViewProjection", _lightSource0->inverseMVPMatrix());

 _photonMap->renderAll();
 render()->blend(false);

 _causticBuffer->setCurrentRenderTarget(1);
 _noiseReductionProgram->bind();
 _noiseReductionProgram->setUniform("texel", _causticBuffer->renderTarget[0]->texel);
 render()->bindTexture(0, _causticBuffer->renderTarget[0]);
 render()->drawFSQ();

 glDepthFunc(GL_LESS);
 glDepthMask(true);
 render()->blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ApplicationScene::prerenderGlass()
{
 if (_useGeometryShader)
 {
  render()->bindFramebuffer(_reflectionRefractionCubemapBuffer);
  glClear(GL_DEPTH_BUFFER_BIT);
  renderEnvironmentToCubeMap(_modelCenter);
 }
 else
 {
  render()->bindFramebuffer(_reflectionRefractionBuffer);
  for (int i = 0; i < 6; ++i)
  {
   _reflectionRefractionBuffer->setCurrentRenderTarget(_reflectionRefractionTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
   _reflectionRefractionBuffer->setDrawBuffersCount(1);
   glClear(GL_DEPTH_BUFFER_BIT);
   renderEnvironment(_modelCenter, _cubemapMatrices[i]);
  }
 }

 render()->bindFramebuffer(_sceneDepthBuffer);
 glClear(GL_DEPTH_BUFFER_BIT);
 renderEnvironmentToDepth(render()->camera->position(), render()->camera->mvpMatrix());

 render()->bindFramebuffer(_backfaceBuffer);
 glClear(GL_DEPTH_BUFFER_BIT);
 _backfaceProgram->bind();
 _backfaceProgram->setCameraPropsFromRender();
 _backfaceProgram->setUniform("mTransform", _modelTransformation);

 render()->cullFace(GL_FRONT);
 _glassObject->renderAll();
 render()->cullFace(GL_BACK);
}

void ApplicationScene::renderGlass()
{
 Ce2ProgramObject* glass = _doubleRefractionGlass ? _glassDoubleRefProgram : _glassSingleRefProgram;

 glass->bind();
 glass->setCameraPropsFromRender();
 glass->setPrimaryLightPosition(_lightSource0->position());
 glass->setUniform("cLightColor", _lightColor0);
 glass->setUniform("mTransform", _modelTransformation);
 glass->setUniform("indexOfRefraction", 1.0f / _materialIOR);

 if (_doubleRefractionGlass)
 {
  glass->setUniform("mModelViewProjectionInverse", render()->camera->inverseMVPMatrix());
  render()->bindTexture(1, _backfaceBuffer->renderTarget[0]);
  render()->bindTexture(2, _backfaceBuffer->depthBuffer);
 }

 render()->bindTexture(0, _useGeometryShader ? _reflectionRefractionCubemapBuffer->renderTarget[0] : _reflectionRefractionTexture);
 _glassObject->renderAll();
}

void ApplicationScene::renderShadowmap()
{
 if (_useGeometryShader)
 {
  renderEnvironmentToDepthCubeMap(_lightSource0->position());
 }
 else
 {
  CubemapMatrixArray cm_matrix = Ce2Camera::cubemapMatrix(_cubemapProjectionMatrix, _lightSource0->position());

  render()->bindFramebuffer(_shadowBuffer);

  _distanceRenderProgram->bind();
  _distanceRenderProgram->setPrimaryLightPosition(_lightSource0->position());

  glPolygonOffset(4, 4);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glClearColor(1.0e+5, 1.0e+5, 1.0e+5, 0.0);
  for (int i = 0; i < 6; ++i)
  {
   _distanceRenderProgram->setMVPMatrix(cm_matrix[i]);
   _shadowBuffer->setCurrentRenderTarget(_shadowmapTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
   _shadowBuffer->setDrawBuffersCount(1);
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
   renderEnvObjects(_distanceRenderProgram);
  }
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glDisable(GL_POLYGON_OFFSET_FILL);
 }
}