//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "../frame/Vector.hpp"
#include "../frame/Material.hpp"
class Object;
class Sphere;

struct Intersection
{
    Intersection(){
        happened=false;
        coords=MyVector::Vector3f();
        normal=MyVector::Vector3f();
        distance= std::numeric_limits<double>::max();
        obj =nullptr;
        m=nullptr;
    }
    bool happened;
    MyVector::Vector3f coords;
    MyVector::Vector3f tcoords;
    MyVector::Vector3f normal;
    MyVector::Vector3f emit;
    double distance;
    Object* obj;
    Material* m;
};
#endif //RAYTRACING_INTERSECTION_H
