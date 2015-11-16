#include <e2primitives.h>

using namespace et;

void Ce2Primitives::createSphere_v3n3t2( VERT_V3_N3_T2*& buffer, float radius, const vec2i& gridDensity )
{
 float dPhi = DOUBLE_PI / (gridDensity.x - 1.0f);
 float dTheta = PI / (gridDensity.y - 1.0f);

 int k = 0;
 float theta = 0;
 for (int i = 0; i < gridDensity.y; i++)
 {
  float phi = 0;
  for (int j = 0; j < gridDensity.x; j++)
  {
   buffer[k].vPosition = radius * fromSphericalRotated(theta, phi);
   buffer[k].vNormal = buffer[k].vPosition.normalize();
   buffer[k].vTexCoord = vec2(float(j) / (gridDensity.x - 1), 1.0f - float(i) / (gridDensity.y - 1));
   phi += dPhi;
   k++;
  }
  theta += dTheta;
 } 

}
