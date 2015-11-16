#include <e2core.h>
#include <e2scene.h>
#include <e2render.h>

using namespace et;

Ce2Scene::Ce2Scene()
{
 _guiLayer = new Ce2GUILayer();

 _mprevp = vec2(0.0);
 memset(_keys,   0, sizeof(_keys));
 memset(_mbhold, 0, sizeof(_mbhold));
 wire = false;
 GetCursorPos(&_prevPT);
}

Ce2Scene::~Ce2Scene()
{
 delete _guiLayer;
}

void Ce2Scene::performDefaultCameraControl(float fScale)
{
 float _ft = fScale;
 if (keyPressed(VK_W)) et::render()->camera->MoveForward(_ft);
 if (keyPressed(VK_S)) et::render()->camera->MoveBackward(_ft);
 if (keyPressed(VK_A)) et::render()->camera->StrafeLeft(_ft);
 if (keyPressed(VK_D)) et::render()->camera->StrafeRight(_ft);
 
 POINT _p;
 GetCursorPos(&_p);
 vec2i dm = vec2i( _p.x - _prevPT.x, _prevPT.y - _p.y);
 
 _ft *= 0.1f * PI / 360.0f;
 
 if (mouseButtonHold(MOUSE_BUTTON_LEFT))
  et::render()->camera->rotateView(dm.y * _ft, -dm.x * _ft);
  
 if (mouseButtonHold(MOUSE_BUTTON_RIGHT)) 
  et::render()->camera->rotateUpVector(dm.x * _ft);

 _prevPT = _p;
}

void Ce2Scene::defaultKeyHandle(unsigned char key)
{
 if (key == VK_F8)
 {
  if (wire = !wire)
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else 
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
 }
   
 if (key == VK_ESCAPE) core.quit();
}

void Ce2Scene::_mleftdown(int x, int y)
{
 _clickpt = vec2i(x, y); 
 _mbhold[0] = true; 

 if (_guiLayer->onMouseDown(x, y, MOUSE_BUTTON_LEFT))
  onMouseDown(x, y, MOUSE_BUTTON_LEFT);
}

void Ce2Scene::_mrightdown(int x, int y)
{
 _clickpt = vec2i(x, y); 
 _mbhold[1] = true; 

 if (_guiLayer->onMouseDown(x, y, MOUSE_BUTTON_RIGHT))
  onMouseDown(x, y, MOUSE_BUTTON_RIGHT);
}

void Ce2Scene::_mmiddown(int x, int y)
{
 _clickpt = vec2i(x, y); 
 _mbhold[2] = true; 

 if (_guiLayer->onMouseDown(x, y, MOUSE_BUTTON_MIDDLE))
  onMouseDown(x, y, MOUSE_BUTTON_MIDDLE);
}
  
void Ce2Scene::_mleftup(int x, int y)
{ 
 _mbhold[0] = false; 

 if (_guiLayer->onMouseUp(x, y, MOUSE_BUTTON_LEFT))
  onMouseUp(x, y, MOUSE_BUTTON_LEFT);
}

void Ce2Scene::_mrightup(int x, int y) 
{
 _mbhold[1] = false; 
 if (_guiLayer->onMouseUp(x, y, MOUSE_BUTTON_RIGHT))
  onMouseUp(x, y, MOUSE_BUTTON_RIGHT);
}

void Ce2Scene::_mmidup(int x, int y) 
{
 _mbhold[2] = false; 
 if (_guiLayer->onMouseUp(x, y, MOUSE_BUTTON_MIDDLE))
  onMouseUp(x, y, MOUSE_BUTTON_MIDDLE);
}

void Ce2Scene::_mmove(int x, int y)
{
 _mpoint = vec2i(x, y); 
 _mpointf = vec2( float(_mpoint.x) / core.windowSize.x, float(_mpoint.y) / core.windowSize.y );
 _dmpoint = _mprevp - _mpointf;
 _mprevp = _mpointf;
 
 if (_guiLayer->onMouseMove(x, y))
  onMouseMove(x, y);
}
