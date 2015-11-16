#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <e2const.h>
#include <e2tools.h>
#include <e2core.h>
#include <e2render.h>

using namespace std;
using namespace et;

Ce2Core::Ce2Core()
{
 _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

 _hInst = GetModuleHandle(0);
 scene = 0;
 _lastFPS = 0;
 
 char ExePath[MAX_PATH];
 GetModuleFileNameA(_hInst, ExePath, MAX_PATH);
 appPath = GetFilePath(ExePath);
 
 string logFileName = appPath + "e2engine.log";
 _logFile.open(logFileName.c_str());
 log("Начало работы");
}

Ce2Core::~Ce2Core()
{
 log("Завершение работы");
 _logFile.close();
}

void Ce2Core::setParameters(const ENGINEPARAMS& p)
{
 _startupParams = p;
}

void Ce2Core::cleanup()
{ 
 KillTimer(primaryWindow, E2_TIMER_ID);
 
 if (scene) scene->unload();
 
 manager->Cleanup();
 render->cleanup();
 
 for (wndClassExList::iterator i = _wClasses.begin(); i != _wClasses.end(); ++i) 
 {
  UnregisterClassA(i->lpszClassName, _hInst);
 }

 for (hwndList::iterator w = _wWindows.begin(); w != _wWindows.end(); ++w) 
 {
  DestroyWindow(*w);
 }
 
 delete manager;
 delete render;
}                                                

void Ce2Core::log(string info, bool bNewLine)
{
 _logFile << info.c_str(); 
 
 if (bNewLine) 
  _logFile << endl;
}

LRESULT CALLBACK Ce2Core::PrimaryWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
 {        
/////////////////////
  case WM_MOUSEMOVE:
  {
   if (core.scene) 
    core.scene->_mmove( LOWORD(lParam), HIWORD(lParam) ); 
   break;
  }           
/////////////////////
  case WM_LBUTTONDOWN:
  {
   if (core.scene) 
    core.scene->_mleftdown( LOWORD(lParam), HIWORD(lParam) ); 
   break;                        
  }
  case WM_RBUTTONDOWN:
  {
   if (core.scene) 
    core.scene->_mrightdown( LOWORD(lParam), HIWORD(lParam) ); 
   break;                        
  }
  case WM_MBUTTONDOWN:
  {
   if (core.scene) 
    core.scene->_mmiddown( LOWORD(lParam), HIWORD(lParam) ); 
   break;                        
  }
/////////////////////
  case WM_LBUTTONUP:
  {
   if (core.scene) 
    core.scene->_mleftup( LOWORD(lParam), HIWORD(lParam) ); 
   break;                        
  }
  case WM_RBUTTONUP:
  {
   if (core.scene) 
    core.scene->_mrightup( LOWORD(lParam), HIWORD(lParam) ); 
   break;                        
  }
  case WM_MBUTTONUP:
  {
   if (core.scene) 
    core.scene->_mmidup( LOWORD(lParam), HIWORD(lParam) ); 
   break;                        
  }
/////////////////////
  case WM_KEYDOWN:
  { 
    if (core.scene) 
     core.scene->_keydown(char(wParam));
    break; 
  }
  case WM_KEYUP:
  { 
    if (core.scene) 
     core.scene->_keyup(wParam);
    break;
  }

/////////////////////
  case WM_TIMER: 
  {
   if (wParam == E2_TIMER_ID) 
    core.tick();
   break;
  }

  case WM_SIZE:
  {      
   core.windowSize.x = LOWORD(lParam);
   core.windowSize.y = HIWORD(lParam);
   core.windowAspect = float(core.windowSize.x) / float(core.windowSize.y);
   break;
  } 

  case WM_CLOSE:
  case WM_QUIT: 
  case WM_DESTROY:
   {
    PostQuitMessage(0);
    core.done = true;
    break;
   }
 }
 
 return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND Ce2Core::CreateRenderWindow(int style, vec2i pos, vec2i size, WNDPROC wFunction)
{
 string wClassName = "e2windowclass" + intToStr(_wClasses.size());
  
 WNDCLASSEX wClass = {0};
 wClass.cbSize = sizeof(wClass);
 wClass.style = CS_HREDRAW | CS_VREDRAW;
 wClass.lpfnWndProc = wFunction;
 wClass.hInstance = _hInst;
 wClass.hCursor = LoadCursor(0, IDC_ARROW);
 wClass.lpszClassName = wClassName.c_str();
   
 if (!RegisterClassEx(&wClass))
 {
  log("Невозможно выполнить RegisterClass");
  return 0;
 }
 
 _wClasses.push_back(wClass);
 
 bool bHasCaption = (style == RW_FIXED);
 bool bHasBorder = (style == RW_FIXED_NO_CAPTION);

 UINT wCaptionStyle = WS_MINIMIZEBOX | WS_CAPTION | WS_ACTIVECAPTION;
  
 UINT wStyle = WS_POPUP | WS_VISIBLE | WS_SYSMENU |
               (wCaptionStyle) * bHasCaption | 
               (WS_BORDER) * bHasBorder;

 bHasCaption |= bHasBorder;
 
 vec2i vPos;
 vec2i vSize;
 vPos.x = pos.x * bHasCaption;
 vPos.y = pos.y * bHasCaption;
 vSize = size;
 
 tagRECT wRect; 
 wRect.left = pos.x;
 wRect.top  = pos.y;
 
 int nScreenW;
 int nScreenH;

 RECT wR;
 SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&wR, 0);
 
 if (style == RW_FITSCREEN)
 {
  nScreenW = GetSystemMetrics(SM_CXSCREEN);
  nScreenH = GetSystemMetrics(SM_CYSCREEN);
 }
 else
 {
  nScreenH = wR.bottom - wR.top;
  nScreenW = wR.right  - wR.left;
 }

 if (!bHasCaption)
  {
   wRect.right  = nScreenW;
   wRect.bottom = nScreenH;
  } 
 else
  {
   wRect.right  = pos.x + size.x;
   wRect.bottom = pos.y + size.y;
   AdjustWindowRect(&wRect, wStyle, false);
   if (pos.x == 0)
    vPos.x = (nScreenW - wRect.right  + wRect.left) / 2;
   if (pos.y == 0)
    vPos.y = (nScreenH - wRect.bottom + wRect.top ) / 2;
  };

 // DEBUG
 //vPos.x = wR.right - wR.left - vSize.x - 10;
 //vPos.y = wR.bottom - wR.top - vSize.y - 10;
 
 vSize.x = wRect.right  - wRect.left;
 vSize.y = wRect.bottom - wRect.top;
 
 bool bPrimary = (_wWindows.size() == 0);
 
  HWND hWnd = CreateWindowExA(bPrimary ? WS_EX_APPWINDOW : WS_EX_TOOLWINDOW, wClass.lpszClassName, 
                "et2::e2w",  wStyle, vPos.x, vPos.y, vSize.x, vSize.y, 0, 0, _hInst, 0);
   
 if (!hWnd)
  {
   log("Невозможно создать окно");
   return 0;
  }
  
 _wWindows.push_back(hWnd);

 SetActiveWindow(hWnd);
 ShowWindow(hWnd, SW_SHOWDEFAULT);
 UpdateWindow(hWnd);
 BringWindowToTop(hWnd);
 SetFocus(hWnd);
  
 if (bPrimary) 
 {
  primaryWindow = hWnd;
 } 

 if (!bHasCaption)
 {
     SetCapture(hWnd);
 }

 GetClientRect(hWnd, &wRect);
 windowSize = vec2i(wRect.right - wRect.left, wRect.bottom - wRect.top);
 
 return hWnd;
}

int Ce2Core::run(Ce2Scene* newScene)
{
 if (!(primaryWindow = CreateRenderWindow(_startupParams.eWindowStyle, vec2i(_startupParams.vPosX, _startupParams.vPosY), 
                        vec2i(_startupParams.vSizeX, _startupParams.vSizeY), (WNDPROC)(&et::Ce2Core::PrimaryWndProc) ))) return 1;


 runTime = 0.0;
 _frameRate = 0.0;
 _prevTime = QueryTime();

 render = new Ce2Render(primaryWindow, _startupParams.eRender.bForwardContext);
 if (!render->canRender)
  {
   render->cleanup();
   return 1;
  }
 manager = new Ce2ResourceManager(); 
 render->init();
 
 SetTimer(primaryWindow, E2_TIMER_ID, 1000, 0);

 _nFPS = 0;
 frameTime = 0.0;

 setScene(newScene);

 MSG msg;
 done = false;

 while (!done)
 {
  if (PeekMessage(&msg, primaryWindow, 0, 0, PM_REMOVE))
  {
   TranslateMessage(&msg);
   DispatchMessage(&msg);
  }
  else
  {
   idle();
  }
 }

 cleanup(); 

 return 0;
}

void Ce2Core::idle()
{
 double t = QueryTime();

 if (t - _prevTime < _frameRate) return;

  if (scene)
    scene->update();

  render->render();

  frameTime = float(t - _prevTime); 
  runTime += frameTime;
  _prevTime = t;

  ++_nFPS;
}

void Ce2Core::tick()
{ 
 _lastFPS = _nFPS;
 setCaption( intToStr( _nFPS ) );
 _nFPS = 0;
}

void Ce2Core::setCaption(std::string & caption)
{
 SendMessage(primaryWindow, WM_SETTEXT, 0, (LPARAM)(caption.c_str()));
}

void Ce2Core::setScene(Ce2Scene* newScene)
{
 if (scene)
 {
  scene->unload();
  scene = 0;
 }
 
 if (newScene)
 {
  scene = newScene;
  scene->load();
 } 
 
}