//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_BOUNDS3_H
#define RAYTRACING_BOUNDS3_H
#include "Ray.hpp"
#include "../frame/Vector.hpp"
#include <limits>
#include <array>

class Bounds3
{
  public:
    MyVector::Vector3f pMin, pMax; // two points to specify the bounding box
    Bounds3()
    {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        pMax = MyVector::Vector3f(minNum, minNum, minNum);
        pMin = MyVector::Vector3f(maxNum, maxNum, maxNum);
    }
    Bounds3(const MyVector::Vector3f p) : pMin(p), pMax(p) {}
    Bounds3(const MyVector::Vector3f p1, const MyVector::Vector3f p2)
    {
        pMin = MyVector::Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
        pMax = MyVector::Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
    }

    MyVector::Vector3f Diagonal() const { return pMax - pMin; }
    int maxExtent() const
    {
        MyVector::Vector3f d = Diagonal();
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

    double SurfaceArea() const
    {
        MyVector::Vector3f d = Diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    MyVector::Vector3f Centroid() { return 0.5 * pMin + 0.5 * pMax; }
    Bounds3 Intersect(const Bounds3& b)
    {
        return Bounds3(MyVector::Vector3f(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y),
                                fmax(pMin.z, b.pMin.z)),
                       MyVector::Vector3f(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y),
                                fmin(pMax.z, b.pMax.z)));
    }

    MyVector::Vector3f Offset(const MyVector::Vector3f& p) const
    {
        MyVector::Vector3f o = p - pMin;
        if (pMax.x > pMin.x)
            o.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y)
            o.y /= pMax.y - pMin.y;
        if (pMax.z > pMin.z)
            o.z /= pMax.z - pMin.z;
        return o;
    }

    bool Overlaps(const Bounds3& b1, const Bounds3& b2)
    {
        bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
        bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
        bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
        return (x && y && z);
    }

    bool Inside(const MyVector::Vector3f& p, const Bounds3& b)
    {
        return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
                p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
    }
    inline const MyVector::Vector3f& operator[](int i) const
    {
        return (i == 0) ? pMin : pMax;
    }

    inline bool IntersectP(const Ray& ray, const MyVector::Vector3f& invDir,
                           const std::array<int, 3>& dirisNeg) const;
};



inline bool Bounds3::IntersectP(const Ray& ray, const MyVector::Vector3f& invDir,
                                const std::array<int, 3>& dirIsNeg) const
{
    // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), use this because Multiply is faster that Division
    // dirIsNeg: ray direction(x,y,z), dirIsNeg=[int(x>0),int(y>0),int(z>0)], use this to simplify your logic
    // TODO test if ray bound intersects

    float t_min_x, t_min_y, t_min_z, t_max_x, t_max_y, t_max_z;

    if (dirIsNeg[0]) {
        t_min_x = (pMin.x - ray.origin.x) * invDir.x;
        t_max_x = (pMax.x - ray.origin.x) * invDir.x;
    } else {
        //std::cout << 1 << std::endl;
        t_min_x = (pMax.x - ray.origin.x) * invDir.x;
        t_max_x = (pMin.x - ray.origin.x) * invDir.x;
    };

    if (dirIsNeg[1]) {
        t_min_y = (pMin.y - ray.origin.y) * invDir.y;
        t_max_y = (pMax.y - ray.origin.y) * invDir.y;
    } else {
        //std::cout << 1 << std::endl;
        t_min_y = (pMax.y - ray.origin.y) * invDir.y;
        t_max_y = (pMin.y - ray.origin.y) * invDir.y;
    }

    if (dirIsNeg[2]) {
        t_min_z = (pMin.z - ray.origin.z) * invDir.z;
        t_max_z = (pMax.z - ray.origin.z) * invDir.z;
    } else {
        //std::cout << 1 << std::endl;
        t_min_z = (pMax.z - ray.origin.z) * invDir.z;
        t_max_z = (pMin.z - ray.origin.z) * invDir.z;
    }

    float t_min = std::max(std::max(t_min_x, t_min_y), t_min_z);
    float t_max = std::min(std::min(t_max_x, t_max_y), t_max_z);

    return t_min <= t_max && t_max > EPSILON;
}

// MyVector::Vector3f t1 = (pMin - ray.origin) * invDir;
// MyVector::Vector3f t2 = (pMax - ray.origin) * invDir;
//
// MyVector::Vector3f t_min_v = MyVector::Vector3f::Min(t1, t2);
// MyVector::Vector3f t_max_v = MyVector::Vector3f::Max(t1, t2);
//
// std::cout << "min: " << t_min_v.x << " " << t_min_v.y << " " << t_min_v.z << std::endl;
// std::cout << "max: " << t_max_v.x << " " << t_max_v.y << " " << t_max_v.z << std::endl;
//
// float t_min = std::max(t_min_v.x, std::max(t_min_v.y, t_min_v.z));
// float t_max = std::min(t_max_v.x, std::min(t_max_v.y, t_max_v.z));

inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)
{
    Bounds3 ret;
    ret.pMin = MyVector::Vector3f::Min(b1.pMin, b2.pMin);
    ret.pMax = MyVector::Vector3f::Max(b1.pMax, b2.pMax);
    return ret;
}

inline Bounds3 Union(const Bounds3& b, const MyVector::Vector3f& p)
{
    Bounds3 ret;
    ret.pMin = MyVector::Vector3f::Min(b.pMin, p);
    ret.pMax = MyVector::Vector3f::Max(b.pMax, p);
    return ret;
}

#endif // RAYTRACING_BOUNDS3_H
