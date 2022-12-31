#include "head.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
std::vector<Sphere> spheres;
char* dest_dir = "./outpic/";
char* texture_dir = "./input/";
ImageTexture* tex[5];
int main(int argc, char* argv[]){
    // 获取纹理贴图源数据并创建纹理
    for(int i=0; i<5; i++){
        int nx, ny, nn;
        std::stringstream sm;
        sm << texture_dir << i << ".jpg";
        string img_name;
        sm >> img_name;
        unsigned char *img = stbi_load(img_name.data(), &nx, &ny, &nn, 0);
        tex[i] = new ImageTexture(img, nx, ny);
    }
#ifdef ADD_TEXTURE
    // 位置，半径，表面颜色， 反射率，透射率，自发光，
    spheres.push_back(Sphere(Vec_3f(0.0, -10004, -20), 10000, Vec_3f(0.44,0.50,0.56), 0.05, 0.0));
    spheres.push_back(Sphere(Vec_3f(0.0, 0, -20),     4, Vec_3f(1.00,0.32,0.36), 1, 0.9, 0.1, tex[1]));
    spheres.push_back(Sphere(Vec_3f(5.0, -1,-15),     2, Vec_3f(0.90,0.76,0.46), 1, 0.9, 0.1, tex[2]));
    spheres.push_back(Sphere(Vec_3f(5.0, 0,-25),     3, Vec_3f(0.65,0.77,0.97), 1, 0.9, 0.1, tex[3]));
    spheres.push_back(Sphere(Vec_3f(-5.5, 0, -15),     3, Vec_3f(0.90,0.90,0.90), 1, 0.9, 0.1, tex[4]));
    // 背景球体
    spheres.push_back(Sphere(Vec_3f(0.0, 0, -20),     400, Vec_3f(1.00,0.32,0.36), 1, 1.0, 0.1, tex[0]));
#else
    spheres.push_back(Sphere(Vec_3f(0.0, -10004, -20), 10000, Vec_3f(0.20,0.20,0.20), 0, 0.0));
    spheres.push_back(Sphere(Vec_3f(0.0, 0, -20),     4, Vec_3f(1.00,0.32,0.36), 1, 0.5));
    spheres.push_back(Sphere(Vec_3f(5.0, -1,-15),     2, Vec_3f(0.90,0.76,0.46), 1, 0.0));
    spheres.push_back(Sphere(Vec_3f(5.0, 0,-25),     3, Vec_3f(0.65,0.77,0.97), 1, 0.0));
    spheres.push_back(Sphere(Vec_3f(-5.5, 0, -15),     3, Vec_3f(0.90,0.90,0.90), 1, 0.0));
#endif
    // 光源
    spheres.push_back(Sphere(Vec_3f(0.0, 20, -30), 3, Vec_3f(0.0,0.0,0.0), 0, 0.0, Vec_3f(1)));
#if MODE==SAVE_PIC
    render(spheres, fov);
    // 保存最终渲染结果
    std::ofstream ofs("./outpic/1.ppm",std::ios::out | std::ios::binary);
    ofs << "P6\n" << WIDTH << " "<<HEIGHT<<"\n255\n";
    for(unsigned i = 0;i < WIDTH * HEIGHT * 3; i++){
        // 0, 255
        ofs <<img_buf[i];
    } 
    ofs.close();

#elif MODE==SAVE_ANIM
    for(int i=0; i<60; i++){
        render(spheres, fov);
        dynamic_Sphere();
        // 保存最终渲染结果
        std::stringstream sm;
        sm << dest_dir << "frame" << i << ".ppm";
        string file_path;
        sm >> file_path;
        cout << "Saving img at "<< file_path <<endl;
        std::ofstream ofs(file_path, std::ios::out | std::ios::binary);
        ofs << "P6\n" << WIDTH << " "<<HEIGHT<<"\n255\n";
        for(unsigned i = 0;i < WIDTH * HEIGHT * 3; i++){
            // 0, 255
            ofs <<img_buf[i];
        } 
        ofs.close();
    }

#else
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("render");

    glutSpecialFunc(SpecialKeys);
    glutMouseFunc(mouseClick);
    glutDisplayFunc(DrawScene);

    glutMainLoop();

#endif
    return 0;
}