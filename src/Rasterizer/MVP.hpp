//
// Created by MacBook on 25-5-9.
//

#include "../frame/Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0],
                 0, 1, 0, -eye_pos[1],
                 0, 0, 1, -eye_pos[2],
                 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    Eigen::Matrix4f rotate;
    float rotation_angle_in_radians = rotation_angle * MY_PI / 180;
    rotate << std::cos(rotation_angle_in_radians), -std::sin(rotation_angle_in_radians), 0, 0,
              std::sin(rotation_angle_in_radians), std::cos(rotation_angle_in_radians), 0, 0,
              0, 0, 1, 0,
              0, 0, 0, 1;

    model = rotate * model;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    float half_fov_in_radians = eye_fov / 2 * std::acos(-1) / 180;
    float yTop = -zNear * tan(half_fov_in_radians);
    float yBottom = -yTop;
    float xLeft = -yTop * aspect_ratio;
    float xRight = -xLeft;

    Eigen::Matrix4f perspective, translate, scale;
    perspective << zNear, 0, 0, 0,
                   0, zNear, 0, 0,
                   0, 0, zNear + zFar, -zNear*zFar,
                   0, 0, 1, 0;

    translate << 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, -(zNear + zFar) / 2,
                 0, 0, 0, 1;

    scale << 1 / xRight, 0, 0, 0,
             0, 1 / yTop, 0, 0,
             0, 0, 2 / (zFar - zNear), 0,
             0, 0, 0, 1;

    projection = scale * translate * perspective * projection;

    return projection;
}

Eigen::Matrix4f get_rotation(Eigen::Vector3f axis, float angle) {
    Eigen::Matrix4f rotation = Eigen::Matrix4f::Identity();
    Eigen::Matrix3f rotation3 = Eigen::Matrix3f::Identity();
    float angle_in_radians = angle * std::acos(-1) / 180;
    Eigen::Matrix3f N;
    N << 0, -axis.z(), axis.y(),
         axis.z(), 0, -axis.x(),
         -axis.y(), axis.x(), 0;

    rotation3 *= std::cos(angle_in_radians);
    rotation3 += (1 - (std::cos(angle_in_radians))) * axis * axis.transpose();
    rotation3 += std::sin(angle_in_radians) * N;

    rotation << rotation3(0, 0), rotation3(0, 1), rotation3(0, 2), 0,
                rotation3(1, 0), rotation3(1, 1), rotation3(1, 2), 0,
                rotation3(2, 0), rotation3(2, 1), rotation3(2, 2), 0,
                0, 0, 0, 1;
    // Eigen::Matrix4f t, s;
    // rotation << std::cos(angle / 180.0 * std::acos(-1)), 0, 0, 0,
    //             0, std::cos(angle / 180.0 * std::acos(-1)), 0, 0,
    //             0, 0, std::cos(angle / 180.0 * std::acos(-1)), 0,
    //             0, 0, 0, 1;
    //
    // t << axis.x() * axis.x(), axis.x() * axis.y(), axis.x() * axis.z(), 0,
    //      axis.y() * axis.x(), axis.y() * axis.y(), axis.y() * axis.z(), 0,
    //      axis.z() * axis.x(), axis.z() * axis.y(), axis.z() * axis.z(), 0,
    //      0, 0, 0, 0;
    //
    // t *= (1 - std::cos(angle / 180.0 * std::acos(-1)));
    //
    // s << 0, -axis.z(), axis.y(), 0,
    //      axis.z(), 0, -axis.x(), 0,
    //      -axis.y(), axis.x(), 0, 0,
    //      0, 0, 0, 0;
    //
    // s *= (std::sin(angle / 180.0 * std::acos(-1)));
    //
    // rotation = rotation + t + s;
    return rotation;
}

