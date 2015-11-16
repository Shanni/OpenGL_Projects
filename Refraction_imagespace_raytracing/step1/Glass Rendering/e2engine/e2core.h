#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <winbase.h>
#include <gl3/glgen.h>
#include <e2const.h>
#include <e2geometry.h>
#include <e2render.h>
#include <e2scene.h>
#include <e2classes.h>

#pragma once

#define core   __core()

using namespace std;

namespace et
{

class Ce2Core : public Ce2Singleton<Ce2Core>
{
 public:
  Ce2Core();
  ~Ce2Core();
  
  int run(Ce2Scene * scene);
  void quit() {done=true;};

  void setParameters(const ENGINEPARAMS& p);
  void setScene(Ce2Scene * scene);
  inline void setFramerate(double rate) {_frameRate = rate;}

  HWND CreateRenderWindow(int style, vec2i pos, vec2i size, WNDPROC wFunction);

  void log(string info, bool bNewLine = true);
  void setCaption(string &caption);

  inline const int fps() const {return _lastFPS;}
  inline const std::string fpsString() const {return intToStr(_lastFPS);}

  void tick();

 public:
  HWND primaryWindow;

  string appPath;
  float runTime;
  float frameTime;
  float windowAspect;
  vec2i windowSize;
  
  Ce2Scene* scene;
  Ce2Render* render;
  Ce2ResourceManager* manager;

 private:
  static LRESULT CALLBACK PrimaryWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  void cleanup();
  void idle(); 

 private:
  friend class Ce2Scene;
  friend struct Ce2CorePIMPL;

  typedef vector<WNDCLASSEX> wndClassExList;
  typedef vector<HWND> hwndList;

  bool done;
  int _nFPS;
  int _lastFPS;
  double _prevTime;
  double _frameRate;

  HINSTANCE _hInst;
  wndClassExList _wClasses;
  hwndList _wWindows;
  ofstream _logFile;
  ENGINEPARAMS _startupParams;
};

inline Ce2Core& __core() {return Ce2Core::instance();}

}