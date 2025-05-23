//
// Created by LEI XU on 5/13/19.
//

#ifndef RAYTRACING_SPHERE_H
#define RAYTRACING_SPHERE_H

#include "Object.hpp"
#include "Vector.hpp"
#include "../RTRenderer/Bounds3.hpp"
#include "Material.hpp"

class Sphere : public Object{
public:
    MyVector::Vector3f center;
    float radius, radius2;
    Material *m;
    float area;
    Sphere(const MyVector::Vector3f &c, const float &r, Material* mt = new Material()) : center(c), radius(r), radius2(r * r), m(mt), area(4 * M_PI *r *r) {}
    bool intersect(const Ray& ray) {
        // analytic solution
        MyVector::Vector3f L = ray.origin - center;
        float a = dotProduct(ray.direction, ray.direction);
        float b = 2 * dotProduct(ray.direction, L);
        float c = dotProduct(L, L) - radius2;
        float t0, t1;
        float area = 4 * M_PI * radius2;
        if (!solveQuadratic(a, b, c, t0, t1)) return false;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        return true;
    }
    bool intersect(const Ray& ray, float &tnear, uint32_t &index) const
    {
        // analytic solution
        MyVector::Vector3f L = ray.origin - center;
        float a = dotProduct(ray.direction, ray.direction);
        float b = 2 * dotProduct(ray.direction, L);
        float c = dotProduct(L, L) - radius2;
        float t0, t1;
        if (!
        solveQuadratic(a, b, c, t0, t1)) return false;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        tnear = t0;

        return true;
    }
    Intersection getIntersection(Ray ray){
        Intersection result;
        result.happened = false;
        MyVector::Vector3f L = ray.origin - center;
        float a = dotProduct(ray.direction, ray.direction);
        float b = 2 * dotProduct(ray.direction, L);
        float c = dotProduct(L, L) - radius2;
        float t0, t1;
        if (!solveQuadratic(a, b, c, t0, t1)) return result;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return result;
        result.happened=true;

        result.coords = MyVector::Vector3f(ray.origin + ray.direction * t0);
        result.normal = normalize(MyVector::Vector3f(result.coords - center));
        result.m = this->m;
        result.obj = this;
        result.distance = t0;
        return result;

    }
    void getSurfaceProperties(const MyVector::Vector3f &P, const MyVector::Vector3f &I, const uint32_t &index, const MyVector::Vector2f &uv, MyVector::Vector3f &N, MyVector::Vector2f &st) const
    { N = normalize(P - center); }

    MyVector::Vector3f evalDiffuseColor(const MyVector::Vector2f &st)const {
        //return m->getColor();
    }
    Bounds3 getBounds(){
        return Bounds3(MyVector::Vector3f(center.x-radius, center.y-radius, center.z-radius),
                       MyVector::Vector3f(center.x+radius, center.y+radius, center.z+radius));
    }
    void Sample(Intersection &pos, float &pdf){
        float theta = 2.0 * M_PI * get_random_float(), phi = M_PI * get_random_float();
        MyVector::Vector3f dir(std::cos(phi), std::sin(phi)*std::cos(theta), std::sin(phi)*std::sin(theta));
        pos.coords = center + radius * dir;
        pos.normal = dir;
        pos.emit = m->getEmission();
        pdf = 1.0f / area;
    }
    float getArea(){
        return area;
    }
    bool hasEmit(){
        return m->hasEmission();
    }
};




#endif //RAYTRACING_SPHERE_H
