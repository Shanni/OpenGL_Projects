int rayIntersectsTriangle(float *p, float *d,
      float *v0, float *v1, float *v2) {

  float e1[3],e2[3],h[3],s[3],q[3];
  float a,f,u,v;
  vector(e1,v1,v0);
  vector(e2,v2,v0);

  crossProduct(h,d,e2);
  a = innerProduct(e1,h);

  if (a > -0.00001 && a < 0.00001)
    return(false);

  f = 1/a;
  vector(s,p,v0);
  u = f * (innerProduct(s,h));

  if (u < 0.0 || u > 1.0)
    return(false);

  crossProduct(q,s,e1);
  v = f * innerProduct(d,q);

  if (v < 0.0 || u + v > 1.0)
    return(false);

  // at this stage we can compute t to find out where
  // the intersection point is on the line
  float t = f * innerProduct(e2,q);

  if (t > 0.00001) // ray intersection
    return(true);

  else // this means that there is a line intersection
     // but not a ray intersection
     return (false);

}