//
// Created by MacBook on 25-5-13.
//


#include <iostream>
#include <opencv2/opencv.hpp>

#include "../frame/global.hpp"
#include "rasterizer.hpp"
#include "../frame/Triangle.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "../frame/OBJ_Loader.cpp"

static Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload)
{
    return payload.position;
}

static Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
    Eigen::Vector3f result;
    result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
    return result;
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
{
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

struct light
{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

static Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.texture)
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        // payload.tex_coords
        return_color = payload.texture->getColor(payload.tex_coords.x(), payload.tex_coords.y());
    }
    Eigen::Vector3f texture_color;
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = texture_color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular*
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f l = light.position - point;
        Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized()).normalized();
        float r_square = (l).dot(l);
        float lambert_cosine = std::max(0.0f, normal.normalized().dot(l.normalized()));
        float specular_cosine = std::max(0.0f, normal.normalized().dot(h.normalized()));
        Eigen::Vector3f ambient_color = ka.cwiseProduct(amb_light_intensity);

        Eigen::Vector3f diffuse_color = kd.cwiseProduct(light.intensity / r_square) * lambert_cosine;
        Eigen::Vector3f specular_color = ks.cwiseProduct(light.intensity / r_square) * std::pow(specular_cosine, p);
        result_color += (ambient_color + diffuse_color + specular_color);

    }

    return result_color * 255.f;
}

static Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;      // specular_cos幂次

    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};
    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular*
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f l = light.position - point;
        Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized()).normalized();
        float r_square = (l).dot(l);
        float lambert_cosine = std::max(0.0f, normal.normalized().dot(l.normalized()));
        float specular_cosine = std::max(0.0f, normal.normalized().dot(h.normalized()));
        Eigen::Vector3f ambient_color = ka.cwiseProduct(amb_light_intensity);

        Eigen::Vector3f diffuse_color = kd.cwiseProduct(light.intensity / r_square) * lambert_cosine;
        Eigen::Vector3f specular_color = ks.cwiseProduct(light.intensity / r_square) * std::pow(specular_cosine, p);
        result_color += (ambient_color + diffuse_color + specular_color);
    }

    return result_color * 255.f;
}

static Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;

    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Position p = p + kn * n * h(u,v)
    // Normal n = normalize(TBN * ln)

    float x = normal.x();
    float y = normal.y();
    float z = normal.z();

    Eigen::Vector3f t(x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z));
    Eigen::Vector3f b = normal.cross(t);
    float u = payload.tex_coords.x();
    float v = payload.tex_coords.y();
    float w = payload.texture->width;
    float h = payload.texture->height;
    Eigen::Matrix3f TBN;
    TBN << t, b, normal;

    float dU = kh * kn * (payload.texture->getColor(u+1/w,v).norm() - payload.texture->getColor(u,v).norm());
    float dV = kh * kn * (payload.texture->getColor(u,v+1/h).norm() - payload.texture->getColor(u,v).norm());

    Eigen::Vector3f ln(-dU, -dV, 1.f);
    point += kn * normal * payload.texture->getColor(u,v).norm();
    normal = (TBN * ln).normalized();

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular*
        // components are. Then, accumulate that result on the *result_color* object.

        Eigen::Vector3f l = light.position - point;
        Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized()).normalized();
        float r_square = (l).dot(l);
        float lambert_cosine = std::max(0.0f, normal.normalized().dot(l.normalized()));
        float specular_cosine = std::max(0.0f, normal.normalized().dot(h.normalized()));

        Eigen::Vector3f ambient_color = ka.cwiseProduct(amb_light_intensity);
        Eigen::Vector3f diffuse_color = kd.cwiseProduct(light.intensity / r_square) * lambert_cosine;
        Eigen::Vector3f specular_color = ks.cwiseProduct(light.intensity / r_square) * std::pow(specular_cosine, p);

        result_color += (ambient_color + diffuse_color + specular_color);

    }

    return result_color * 255.f;
}


static Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;


    float kh = 0.2, kn = 0.1;

    // TODO: Implement bump mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Normal n = normalize(TBN * ln)

    float x = normal.x();
    float y = normal.y();
    float z = normal.z();

    Eigen::Vector3f t(x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z));
    Eigen::Vector3f b = normal.cross(t);
    float u = payload.tex_coords.x();
    float v = payload.tex_coords.y();
    float w = payload.texture->width;
    float h = payload.texture->height;
    Eigen::Matrix3f TBN;
    TBN << t, b, normal;

    float dU = kh * kn * (payload.texture->getColor(u+1/w,v).norm() - payload.texture->getColor(u,v).norm());
    float dV = kh * kn * (payload.texture->getColor(u,v+1/h).norm() - payload.texture->getColor(u,v).norm());

    Eigen::Vector3f ln(-dU, -dV, 1.f);
    normal = (TBN * ln).normalized();

    Eigen::Vector3f result_color = {0, 0, 0};
    result_color = normal;

    return result_color * 255.f;
}

