//
// Created by Göksu Güvendiren on 2019-05-14.
//

#pragma once

#include "../frame/Vector.hpp"

class Light
{
public:
    Light(const MyVector::Vector3f &p, const MyVector::Vector3f &i) : position(p), intensity(i) {}
    virtual ~Light() = default;
    MyVector::Vector3f position;
    MyVector::Vector3f intensity;
};
