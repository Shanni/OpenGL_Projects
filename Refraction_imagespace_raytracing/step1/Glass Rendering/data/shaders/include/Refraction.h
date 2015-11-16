vec3 computeRefractedVector(vec3 incidence, vec3 normal, float eta)
{
 float etaIdotN = eta * dot(incidence, normal);
 float k = 1.0 - eta*eta + etaIdotN*etaIdotN;

 if (k < 0.0)
  return reflect(incidence, normal);
 else
  return eta * incidence - normal * (etaIdotN + sqrt(k));
}