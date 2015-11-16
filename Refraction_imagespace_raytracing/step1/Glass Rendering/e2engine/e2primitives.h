#pragma once

#include <e2geometry.h>

namespace et
{

class Ce2Primitives
{
 public:
  static void createSphere_v3n3t2( VERT_V3_N3_T2*& buffer, float radius, const vec2i& gridDensity );
  
 private:
};

}