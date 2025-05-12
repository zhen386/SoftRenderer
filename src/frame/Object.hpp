//
// Created by LEI XU on 5/13/19.
//
#pragma once
#ifndef RAYTRACING_OBJECT_H
#define RAYTRACING_OBJECT_H

#include "Vector.hpp"
#include "global.hpp"
#include "../RTRenderer/Bounds3.hpp"
#include "../RTRenderer/Ray.hpp"
#include "../RTRenderer/Intersection.hpp"

class Object
{
public:
    Object() {}
    virtual ~Object() {}
    virtual bool intersect(const Ray& ray) = 0;
    virtual bool intersect(const Ray& ray, float &, uint32_t &) const = 0;
    virtual Intersection getIntersection(Ray _ray) = 0;
    virtual void getSurfaceProperties(const MyVector::Vector3f &, const MyVector::Vector3f &, const uint32_t &, const MyVector::Vector2f &, MyVector::Vector3f &, MyVector::Vector2f &) const = 0;
    virtual MyVector::Vector3f evalDiffuseColor(const MyVector::Vector2f &) const =0;
    virtual Bounds3 getBounds()=0;
    virtual float getArea()=0;
    virtual void Sample(Intersection &pos, float &pdf)=0;
    virtual bool hasEmit()=0;
};



#endif //RAYTRACING_OBJECT_H
