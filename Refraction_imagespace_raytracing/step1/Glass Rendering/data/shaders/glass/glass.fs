#define shininess 0.0325

uniform samplerCube environment_map;

#ifdef DOUBLE_REFRACTION
 uniform sampler2D backface_texture;
 uniform sampler2D backface_depth;
 uniform mat4 mModelViewProjection;
 uniform mat4 mModelViewProjectionInverse;
 uniform vec3 vCamera;
#endif

uniform vec3 cLightColor;
uniform float indexOfRefraction;

in vec3 vLightWS;
in vec3 vViewWS;
in vec3 vNormalWS;
in vec2 TexCoord;
in vec4 vProjectedVertex;
in vec4 vVertexWS;

out vec4 FragColor;

#include <include\CookTorranceEx.h>
#include <include\ImageSpaceIntersection.h>
#include <include\Fresnel.h>
#include <include\Refraction.h>

void main()
{
 vec3 vNormal = normalize(vNormalWS);
 vec3 vLightNormal = normalize(vLightWS);
 vec3 vViewNormal = normalize(vViewWS);

 float VdotN = max(0.0, dot(vViewNormal, vNormal));
 float fFresnel = fresnel(VdotN, indexOfRefraction);

 vec3 vReflected = reflect(-vViewNormal, vNormal);
 vec4 cReflection = texture(environment_map, vReflected);

 vec3 vRefracted = computeRefractedVector(-vViewNormal, vNormal, indexOfRefraction);

#ifdef DOUBLE_REFRACTION
 vec3 vBackfaceIntersection = estimateIntersection(vVertexWS.xyz, vRefracted, mModelViewProjection, mModelViewProjectionInverse, backface_depth);
 vec3 vInnerRay = normalize(vBackfaceIntersection - vVertexWS.xyz);
 vec4 vProjectedBI = mModelViewProjection * vec4(vBackfaceIntersection, 1.0);
 vec3 vBackfaceNormal = vec3(1.0) - 2.0 * texture(backface_texture, vec2(0.5) + 0.5 * vProjectedBI.xy / vProjectedBI.w).xyz;
 vec3 vBackfaceRefracted = computeRefractedVector(vInnerRay, vBackfaceNormal.xyz, 1.0 / indexOfRefraction);
 vec4 cBackfaceColor = texture(environment_map, vBackfaceRefracted);
#else
 vec4 cBackfaceColor = texture(environment_map, vRefracted);
#endif

 vec4 cColor = mix(cBackfaceColor, cReflection, fFresnel);
 float fLight = CookTorranceEx(vNormal, vLightNormal, vViewNormal, fFresnel, shininess);
 vec4 vSpecularColor = cLightColor.xyzz * fLight;

 FragColor = cColor + vSpecularColor;
}