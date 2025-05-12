//
// Created by Göksu Güvendiren on 2019-05-14.
//

#pragma once

#include "../frame/Vector.hpp"
#include "Light.hpp"
#include "../frame/global.hpp"

class AreaLight : public Light
{
public:
    AreaLight(const MyVector::Vector3f &p, const MyVector::Vector3f &i) : Light(p, i)
    {
        normal = MyVector::Vector3f(0, -1, 0);
        u = MyVector::Vector3f(1, 0, 0);
        v = MyVector::Vector3f(0, 0, 1);
        length = 100;
    }

    MyVector::Vector3f SamplePoint() const
    {
        auto random_u = get_random_float();
        auto random_v = get_random_float();
        return position + random_u * u + random_v * v;
    }

    float length;
    MyVector::Vector3f normal;
    MyVector::Vector3f u;
    MyVector::Vector3f v;
};
