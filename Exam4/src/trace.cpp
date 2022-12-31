#include "head.h"
Vec_3f img [WIDTH*HEIGHT];
unsigned char img_buf[WIDTH*HEIGHT*3];
float fov = 50;

// 光线追踪函数，输入光线起点、方向、场景中的球体、光线追踪的最大深度
Vec_3f trace(
  const Vec_3f &rayorigin,
  const Vec_3f &raydirection,
  const std::vector<Sphere> & spheres,
  const int &depth
  )
{
  float tnear = INFINITY; // 最近相交参数
  const Sphere * sphere = NULL; // 最近相交的球体
  // 与场景中的球体求交，找到最近的交点
  for(unsigned i=0; i<spheres.size(); i++){
    float t0 = INFINITY;
    float t1 = INFINITY;
    if(spheres[i].intersect(rayorigin, raydirection, t0, t1)){
      if(t0<0) t0=t1;
      if(t0<tnear){
        tnear = t0;
        sphere = & spheres[i];
      }
    }
  }
  // 光线不与场景中物体相交，返回背景色
  if(sphere==NULL)  return Vec_3f(BG_COLOR);
  Vec_3f surfaceColor = 0;  // 定义表面颜色
  Vec_3f phit = rayorigin + raydirection * tnear; // 计算交点
  Vec_3f nhit = phit - sphere->center; // 计算交点指向球心的向量nhit，球的表面法向
  nhit.normal();  // 正则化
  float bias = 1e-4;
  bool inside = false;
  // nhit与光线点积为正，说明光线起点位于球中心
  if(raydirection.dot(nhit)>0){
    nhit = -nhit;
    inside = true;
  }
  if((sphere->transparency>0 || sphere->reflection>0) && depth<MAX_DEPTH){
    float facingratio = - raydirection.dot(nhit);
    float fresneleffect = mix(pow(1-facingratio, 3), 1, 0.1);
    // 计算发射方向
    Vec_3f reflect_direction = raydirection - nhit * 2 * raydirection.dot(nhit);
    reflect_direction.normal();
    // 递归调用光线追踪函数
    Vec_3f next_reflection = trace(phit + nhit * bias, reflect_direction, spheres, depth+1);
    Vec_3f next_refraction = 0;
    // 球体透明，才计算透射
    if(sphere->transparency){
      // 判断是内侧投射or外侧投射
      float ior = 1.1, eta = inside? ior : 1/ior;
      // 计算投射角度
      float cosi = -nhit.dot(raydirection);
      float k = 1- eta * eta * (1 - cosi * cosi);
      Vec_3f refraction_direction = raydirection * eta + nhit * (eta * cosi - sqrt(k));
      refraction_direction.normal();
      // 递归调用光线追踪
      next_refraction = trace(phit - nhit * bias, refraction_direction, spheres, depth+1);
    }
    // 表面颜色是反射&投射的叠加
    surfaceColor = (next_reflection * fresneleffect + next_refraction*
                    (1- fresneleffect)* sphere->transparency) * sphere->getSurfaceColor(rayorigin, raydirection, tnear);
    // surfaceColor = (next_reflection * fresneleffect + next_refraction*(1- fresneleffect)* sphere->transparency) * sphere->surfaceColor;
  }
  else{
    for(unsigned i=0; i<spheres.size(); i++){
      if(sphere[i].emissionColor.x >0){
        Vec_3f transmission = 1;
        // 计算光线方向
        Vec_3f lightDirection = sphere[i].center - phit;
        lightDirection.normal();
        // 计算光源核其他交点间是否有其他物体，添加阴影效果
        for(unsigned j=0; j<spheres.size();++j){
          if(i!=j){
            float t0, t1;
            if(sphere[j].intersect(phit + nhit * bias, lightDirection, t0, t1)){
              transmission = 0;
              break;
            }
          }
        }
        // 叠加物体颜色
        surfaceColor += sphere->getSurfaceColor(rayorigin, raydirection, tnear) 
                  * transmission * std::max(float(0), nhit.dot(lightDirection))*spheres[i].emissionColor;
        // surfaceColor += sphere->surfaceColor * transmission * std::max(float(0), nhit.dot(lightDirection))*spheres[i].emissionColor;
      }
    }
  }
  // 返回最终表面颜色
  return surfaceColor + sphere->emissionColor;
}

// 渲染函数，计算每个像素的颜色
void render(const std::vector<Sphere> & spheres, float fov){
  Vec_3f *pixel = img;
  float invWidth = 1/float(WIDTH), invHeight = 1/float(HEIGHT);
  float aspectratio = WIDTH/float(HEIGHT);
  float angle = tan((M_PI * 0.5 * fov)/180.);
  // 对所有像素光线追踪
  for(unsigned y=0; y<HEIGHT; y++){
    for(unsigned x=0; x<WIDTH; x++, pixel++){
        float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
#if (MODE == LIVE_PIC || MODE == LIVE_ANIM)
        float yy = (2 * ((y + 0.5) * invHeight) - 1) * angle;   
#else
        float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
#endif
        Vec_3f raydir(xx, yy, -1);
        // Vec_3f raydir(xx, yy, -1);
        raydir.normal();
        *pixel = trace(Vec_3f(0), raydir, spheres, 0);
    }
  }
  for(unsigned i = 0;i < WIDTH * HEIGHT; i++){
    img_buf[3*i] = (unsigned char)(min(float(1), img[i].x) * 255);
    img_buf[3*i+1] = (unsigned char)(min(float(1), img[i].y) * 255);
    img_buf[3*i+2] = (unsigned char)(min(float(1), img[i].z) * 255);
  } 
}