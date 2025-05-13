#include "Renderer.hpp"
#include "../frame/Triangle.hpp"
#include "../frame/Sphere.hpp"
#include "../frame/Vector.hpp"
#include "../frame/global.hpp"
#include <chrono>
#include "../Rasterizer/DrawTriangle.hpp"
#include "../frame/Material.hpp"
#include "../frame/Scene.hpp"

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().


int main(int argc, const char** argv)
{

    // cv测试
    // cv::Mat image(480, 640, CV_8UC3, cv::Scalar(100, 0, 0)); // 蓝色背景
    // cv::rectangle(image, cv::Point(100, 100), cv::Point(200, 200), cv::Scalar(0, 0, 255), -1); // 红色矩形
    //
    // // 检查图像数据
    // std::cout << "图像尺寸: " << image.size() << std::endl;
    //
    // // 显示图像
    // cv::namedWindow("Window", cv::WINDOW_NORMAL);
    // cv::imshow("Window", image);
    // cv::waitKey(0);
    // return 0;


    // draw_triangle_colored(argc, argv);
    if (argc < 2) {
        std::cerr << "too few arguments" << std::endl;
        return 1;
    }

    if (argc > 5) {
        std::cerr << "too many arguments" << std::endl;
    }
    if (std::string(argv[1]) == std::string("wireframe")) {
        draw_triangle_wireframe(argc, argv);
        return 0;
    } else if (std::string(argv[1]) == std::string("color")) {
        draw_triangle_colored(argc, argv);
        return 0;
    }


    // Change the definition here to change resolution
    Scene scene(800, 800);

    Material* red = new Material(DIFFUSE, MyVector::Vector3f(0.0f));
    red->Kd = MyVector::Vector3f(0.63f, 0.065f, 0.05f);
    Material* green = new Material(DIFFUSE, MyVector::Vector3f(0.0f));
    green->Kd = MyVector::Vector3f(0.14f, 0.45f, 0.091f);
    Material* white = new Material(DIFFUSE, MyVector::Vector3f(0.0f));
    white->Kd = MyVector::Vector3f(0.725f, 0.71f, 0.68f);
    Material* light = new Material(DIFFUSE, (8.0f * MyVector::Vector3f(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * MyVector::Vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f *MyVector::Vector3f(0.737f+0.642f,0.737f+0.159f,0.737f)));
    light->Kd = MyVector::Vector3f(0.65f);

    MeshTriangle floor("../models/cornellbox/floor.obj", white);
    MeshTriangle shortbox("../models/cornellbox/shortbox.obj", white);
    MeshTriangle tallbox("../models/cornellbox/tallbox.obj", white);
    // MeshTriangle bunny("../models/bunny/bunny.obj", white);
    MeshTriangle left("../models/cornellbox/left.obj", red);
    MeshTriangle right("../models/cornellbox/right.obj", green);
    MeshTriangle light_("../models/cornellbox/light.obj", light);

    scene.Add(&floor);
    scene.Add(&shortbox);
    scene.Add(&tallbox);
    // scene.Add(&bunny);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);

    scene.buildBVH();

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}