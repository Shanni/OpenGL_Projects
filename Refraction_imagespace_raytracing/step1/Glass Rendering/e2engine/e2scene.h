#pragma once

#include <windows.h>
#include <e2render.h>
#include <e2geometry.h>
#include <e2gui.h>

namespace et
{

class Ce2Scene
{
 public:
  Ce2Scene();
  virtual ~Ce2Scene();
  
// LOADING/UNLOADING 
  virtual void load(){};
  virtual void unload(){};
  virtual void update(){};
  
// EVENTS  
  virtual void onMouseUp(int x, int y, int button){};
  virtual void onMouseDown(int x, int y, int button){};
  virtual void onMouseMove(int x, int y){};
  virtual void onKeyDown(unsigned char key){};
  virtual void onKeyUp(unsigned char key){};
  
// RENDERS  
  virtual void RenderToLight(){};
  virtual void RenderToDepth(){};
  virtual void RenderToReflection(){};
  virtual void renderScene(){};

 protected:
  bool wire;

  vec2i cursorPos() {return _mpoint;}
  vec2i clickPos() {return _clickpt;}

  vec2 cursorPosF() {return _mpointf;}
  
  vec2 mouseMovement() {return _dmpoint;}
  
  bool mouseButtonHold(int btn) {return _mbhold[btn];}
  bool& keyPressed(unsigned char key) {return _keys[key];}
  
  void performDefaultCameraControl(float fScale = 1.0);
  void defaultKeyHandle(unsigned char key);

 private:
  friend class Ce2Core;

  bool _keys[255];
  bool _mbhold[3];
  Ce2GUILayer* _guiLayer;

  POINT _prevPT;
  
  vec2i _mpoint;
  vec2 _mpointf;
  
  vec2i _clickpt;
  vec2 _mprevp;
  vec2 _dmpoint;
  
  void _keydown(unsigned char key) {_keys[key] = true; onKeyDown(key);}
  void _keyup(unsigned char key) {_keys[key] = false; onKeyUp(key);}
  
  void _mleftdown(int x, int y);
  void _mrightdown(int x, int y);
  void _mmiddown(int x, int y);
  void _mleftup(int x, int y);
  void _mrightup(int x, int y);
  void _mmidup(int x, int y);

  void _mmove(int x, int y);
};

}