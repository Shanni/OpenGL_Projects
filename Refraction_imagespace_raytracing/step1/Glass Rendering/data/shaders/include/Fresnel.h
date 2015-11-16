float fresnel(float VdotN, float eta)
{
 float sqr_eta = eta * eta;
 float etaCos = eta * VdotN;
 float sqr_etaCos = etaCos*etaCos;
 float one_minSqrEta = 1.0 - sqr_eta;
 float value = etaCos - sqrt(one_minSqrEta + sqr_etaCos);
 value *= value / one_minSqrEta;
 return min(1.0, value * value);
}
