//
// Created by MacBook on 25-5-9.
//

#include "MVP.hpp"
#include "rasterizer.hpp"
#include <Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "../frame/global.hpp"
#include "rasterizer.hpp"
#include "RasTriangle.hpp"
#include "Shader.cpp"
// #include "Shader.hpp"
#include "Texture.hpp"
#include "../frame/OBJ_Loader.cpp"

static void draw_triangle_wireframe(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 4) {
        command_line = true;
        angle = std::stof(argv[3]); // -r by default
        if (argc == 5) {
            filename = std::string(argv[4]);
        }
        else
            return;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.drawWireFrame(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.drawWireFrame(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return;
}

static void draw_triangle_colored(int argc, const char** argv)
{
    float angle = 0;
    // bool command_line = false;
    // std::string filename = "output.png";
    //
    // if (argc == 2)
    // {
    //     command_line = true;
    //     filename = std::string(argv[1]);
    // }

    bool SSAA;
    if (std::string(argv[2]) == std::string("SSAA")) {
        SSAA = true;
    } else {
        SSAA = false;
    }


    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0,0,5};


    std::vector<Eigen::Vector3f> pos
            {
                    {2, 0, -2},
                    {0, 2, -2},
                    {-2, 0, -2},
                    {3.5, -1, -5},
                    {2.5, 1.5, -5},
                    {-1, 0.5, -5}
            };

    std::vector<Eigen::Vector3i> ind
            {
                    {0, 1, 2},
                    {3, 4, 5}
            };

    std::vector<Eigen::Vector3f> cols
            {
                    {217.0, 238.0, 185.0},
                    {217.0, 238.0, 185.0},
                    {217.0, 238.0, 185.0},
                    {185.0, 217.0, 238.0},
                    {185.0, 217.0, 238.0},
                    {185.0, 217.0, 238.0}
            };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
    auto col_id = r.load_colors(cols);

    int key = 0;
    int frame_count = 0;

    // if (command_line)
    // {
    //     r.clear(rst::Buffers::Color | rst::Buffers::Depth);
    //
    //     r.set_model(get_model_matrix(angle));
    //     r.set_view(get_view_matrix(eye_pos));
    //     r.set_projection(get_projection_matrix(45, 1, 0.1, 50));
    //
    //     r.drawColoredTriangle(pos_id, ind_id, col_id, rst::Primitive::Triangle);
    //     cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
    //     image.convertTo(image, CV_8UC3, 1.0f);
    //     cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    //
    //     cv::imwrite(filename, image);
    //
    //     return;
    // }

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.drawColoredTriangle(pos_id, ind_id, col_id, rst::Primitive::Triangle, SSAA);

        // std::cout << r.frame_buffer().empty() << std::endl;

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        // cv::randu(image, 0.0, 1.0); // 填充随机浮点数
        // cv::imshow("image", image);
        image.convertTo(image, CV_8UC3, 1.f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return;
}

static void draw_obj(int argc, const char** argv) {
    bool SSAA = false;
    if (std::string(argv[2]) == std::string("SSAA")) {
        SSAA = true;
    } else {
        SSAA = false;
    }

    std::vector<RasTriangle*> TriangleList;

    float angle = 140.0;
    // bool command_line = false;

    // std::string filename = "output.png";
    objl::Loader Loader;
    std::string obj_path = "../models/spot/";

    // Load .obj File
    bool loadout = Loader.LoadFile("../models/spot/spot_triangulated_good.obj");
    for(auto mesh:Loader.LoadedMeshes)
    {
        for(int i=0;i<mesh.Vertices.size();i+=3)
        {
            RasTriangle* t = new RasTriangle();
            for(int j=0;j<3;j++)
            {
                t->setVertex(j,Vector4f(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0f));
                t->setNormal(j,Vector3f(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z));
                t->setTexCoord(j,Vector2f(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));
            }
            TriangleList.push_back(t);
        }
    }

    rst::rasterizer r(700, 700);

    auto texture_path = "hmap.jpg";
    r.set_texture(Texture(obj_path + texture_path));

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = phong_fragment_shader;

    if (argc >= 2)
    {
        // command_line = true;
        // filename = std::string(argv[1]);

        if (argc == 3 && std::string(argv[2]) == "texture")
        {
            std::cout << "Rasterizing using the texture shader\n";
            active_shader = texture_fragment_shader;
            texture_path = "spot_texture.png";
            r.set_texture(Texture(obj_path + texture_path));
        }
        else if (argc == 3 && std::string(argv[2]) == "normal")
        {
            std::cout << "Rasterizing using the normal shader\n";
            active_shader = normal_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "phong")
        {
            std::cout << "Rasterizing using the phong shader\n";
            active_shader = phong_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "bump")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = bump_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "displacement")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = displacement_fragment_shader;
        }
    }

    Eigen::Vector3f eye_pos = {0,0,10};

    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(active_shader);

    int key = 0;
    int frame_count = 0;

    // if (command_line)
    // {
    //     r.clear(rst::Buffers::Color | rst::Buffers::Depth);
    //     r.set_model(get_model_matrix(angle));
    //     r.set_view(get_view_matrix(eye_pos));
    //     r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));
    //
    //     r.drawObj(TriangleList, SSAA);
    //     cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
    //     image.convertTo(image, CV_8UC3, 1.0f);
    //     cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    //
    //     cv::imwrite(filename, image);
    //
    //     return ;
    // }

    while(key != 27)
    {
        // std::cout << 1 ;
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        //r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        r.drawObj(TriangleList, SSAA);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        // cv::imwrite(filename, image);
        key = cv::waitKey(10);

        if (key == 'a' )
        {
            angle -= 0.1;
        }
        else if (key == 'd')
        {
            angle += 0.1;
        }

    }
    return;
}