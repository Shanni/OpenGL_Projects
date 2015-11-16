#include <e2core.h>
#include "scene.h"

using namespace et;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
 ENGINEPARAMS P = {0};

 P.eRender.bForwardContext = false;
 P.eWindowStyle = RW_FIXED_NO_CAPTION;
 P.vSizeX = 800;
 P.vSizeY = 600;
 //P.vPosX = 1100;
 
 core.setParameters(P);
 ApplicationScene scene;
 
 return core.run(&scene);
}