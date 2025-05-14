#pragma once

#include "../RTRenderer/BVH.hpp"
#include "../RTRenderer/Intersection.hpp"
#include "Material.hpp"
#include "OBJ_Loader.h"
// #include "Object.hpp"
#include "Triangle.hpp"
#include <cassert>
#include <array>
#include <Eigen>


static bool rayTriangleIntersect(const MyVector::Vector3f& v0, const MyVector::Vector3f& v1,
                          const MyVector::Vector3f& v2, const MyVector::Vector3f& orig,
                          const MyVector::Vector3f& dir, float& tnear, float& u, float& v)
{
    MyVector::Vector3f edge1 = v1 - v0;
    MyVector::Vector3f edge2 = v2 - v0;
    MyVector::Vector3f pvec = crossProduct(dir, edge2);
    float det = dotProduct(edge1, pvec);
    if (det == 0 || det < 0)
        return false;

    MyVector::Vector3f tvec = orig - v0;
    u = dotProduct(tvec, pvec);
    if (u < 0 || u > det)
        return false;

    MyVector::Vector3f qvec = crossProduct(tvec, edge1);
    v = dotProduct(dir, qvec);
    if (v < 0 || u + v > det)
        return false;

    float invDet = 1 / det;

    tnear = dotProduct(edge2, qvec) * invDet;
    u *= invDet;
    v *= invDet;

    return true;
}

class Triangle : public Object
{
public:
    MyVector::Vector3f v0, v1, v2; // vertices A, B ,C , counter-clockwise order
    MyVector::Vector3f e1, e2;     // 2 edges v1-v0, v2-v0;
    MyVector::Vector3f t0, t1, t2; // texture coords
    MyVector::Vector3f normal;
    float area;
    MyVector::Vector3f color[3];   // color at each vertex;
    Material* m;

    Triangle();

    Triangle(MyVector::Vector3f _v0, MyVector::Vector3f _v1, MyVector::Vector3f _v2, Material* _m = nullptr)
        : v0(_v0), v1(_v1), v2(_v2), m(_m)
    {
        e1 = v1 - v0;
        e2 = v2 - v0;
        normal = normalize(crossProduct(e1, e2));
        area = crossProduct(e1, e2).norm()*0.5f;
    }

    MyVector::Vector3f a() const { return v0; }
    MyVector::Vector3f b() const { return v1; }
    MyVector::Vector3f c() const { return v2; }

    void setVertex0(MyVector::Vector3f ver) { v0 = ver; }
    void setVertex1(MyVector::Vector3f ver) { v1 = ver; }
    void setVertex2(MyVector::Vector3f ver) { v2 = ver; }

    void setNormal(MyVector::Vector3f n) { normal = n; }

    void setColor(int ind, float r, float g, float b)
    {
        if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) ||
            (b > 255.)) {
            throw std::runtime_error("Invalid color values");
            }

        color[ind] = MyVector::Vector3f((float)r / 255., (float)g / 255., (float)b / 255.);
    }

    void setTexCoord0(float s, float t) { t0 = (s, t, 1.f); }
    void setTexCoord1(float s, float t) { t1 = (t, s, 1.f); }
    void setTexCoord2(float s, float t) { t2 = (t, s, 1.f); }

    // std::array<Vector4f, 3> Triangle::toVector4() const
    // {
    //     std::array<Vector4f, 3> res;
    //     std::transform(std::begin(v), std::end(v), res.begin(), [](auto& vec) {
    //         return Vector4f(vec.x(), vec.y(), vec.z(), 1.f);
    //     });
    //     return res;
    // }

    bool intersect(const Ray& ray) override;
    bool intersect(const Ray& ray, float& tnear,
                   uint32_t& index) const override;
    Intersection getIntersection(Ray ray) override;
    void getSurfaceProperties(const MyVector::Vector3f& P, const MyVector::Vector3f& I,
                              const uint32_t& index, const MyVector::Vector2f& uv,
                              MyVector::Vector3f& N, MyVector::Vector2f& st) const override
    {
        N = normal;
        //        throw std::runtime_error("triangle::getSurfaceProperties not
        //        implemented.");
    }
    MyVector::Vector3f evalDiffuseColor(const MyVector::Vector2f&) const override;
    Bounds3 getBounds() override;
    void Sample(Intersection &pos, float &pdf){
        float x = std::sqrt(get_random_float()), y = get_random_float();
        pos.coords = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);
        pos.normal = this->normal;
        pdf = 1.0f / area;
    }
    float getArea(){
        return area;
    }
    bool hasEmit(){
        return m->hasEmission();
    }
};

class MeshTriangle : public Object
{
public:
    MeshTriangle(const std::string& filename, Material *mt = new Material())
    {
        objl::Loader loader;
        loader.LoadFile(filename);
        area = 0;
        m = mt;
        assert(loader.LoadedMeshes.size() == 1);
        auto mesh = loader.LoadedMeshes[0];

        MyVector::Vector3f min_vert = MyVector::Vector3f{std::numeric_limits<float>::infinity(),
                                     std::numeric_limits<float>::infinity(),
                                     std::numeric_limits<float>::infinity()};
        MyVector::Vector3f max_vert = MyVector::Vector3f{-std::numeric_limits<float>::infinity(),
                                     -std::numeric_limits<float>::infinity(),
                                     -std::numeric_limits<float>::infinity()};
        for (int i = 0; i < mesh.Vertices.size(); i += 3) {
            std::array<MyVector::Vector3f, 3> face_vertices;

            for (int j = 0; j < 3; j++) {
                auto vert = MyVector::Vector3f(mesh.Vertices[i + j].Position.X,
                                     mesh.Vertices[i + j].Position.Y,
                                     mesh.Vertices[i + j].Position.Z);
                face_vertices[j] = vert;

                min_vert = MyVector::Vector3f(std::min(min_vert.x, vert.x),
                                    std::min(min_vert.y, vert.y),
                                    std::min(min_vert.z, vert.z));
                max_vert = MyVector::Vector3f(std::max(max_vert.x, vert.x),
                                    std::max(max_vert.y, vert.y),
                                    std::max(max_vert.z, vert.z));
            }

            triangles.emplace_back(face_vertices[0], face_vertices[1],
                                   face_vertices[2], mt);
        }

        bounding_box = Bounds3(min_vert, max_vert);

        std::vector<Object*> ptrs;
        for (auto& tri : triangles){
            ptrs.push_back(&tri);
            area += tri.area;
        }
        bvh = new BVHAccel(ptrs);
    }

    bool intersect(const Ray& ray) { return true; }

    bool intersect(const Ray& ray, float& tnear, uint32_t& index) const
    {
        bool intersect = false;
        for (uint32_t k = 0; k < numTriangles; ++k) {
            const MyVector::Vector3f& v0 = vertices[vertexIndex[k * 3]];
            const MyVector::Vector3f& v1 = vertices[vertexIndex[k * 3 + 1]];
            const MyVector::Vector3f& v2 = vertices[vertexIndex[k * 3 + 2]];
            float t, u, v;
            if (rayTriangleIntersect(v0, v1, v2, ray.origin, ray.direction, t,
                                     u, v) &&
                t < tnear) {
                tnear = t;
                index = k;
                intersect |= true;
            }
        }

        return intersect;
    }

    Bounds3 getBounds() { return bounding_box; }

    void getSurfaceProperties(const MyVector::Vector3f& P, const MyVector::Vector3f& I,
                              const uint32_t& index, const MyVector::Vector2f& uv,
                              MyVector::Vector3f& N, MyVector::Vector2f& st) const
    {
        const MyVector::Vector3f& v0 = vertices[vertexIndex[index * 3]];
        const MyVector::Vector3f& v1 = vertices[vertexIndex[index * 3 + 1]];
        const MyVector::Vector3f& v2 = vertices[vertexIndex[index * 3 + 2]];
        MyVector::Vector3f e0 = normalize(v1 - v0);
        MyVector::Vector3f e1 = normalize(v2 - v1);
        N = normalize(crossProduct(e0, e1));
        const MyVector::Vector2f& st0 = stCoordinates[vertexIndex[index * 3]];
        const MyVector::Vector2f& st1 = stCoordinates[vertexIndex[index * 3 + 1]];
        const MyVector::Vector2f& st2 = stCoordinates[vertexIndex[index * 3 + 2]];
        st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
    }

    MyVector::Vector3f evalDiffuseColor(const MyVector::Vector2f& st) const
    {
        float scale = 5;
        float pattern =
            (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
        return lerp(MyVector::Vector3f(0.815, 0.235, 0.031),
                    MyVector::Vector3f(0.937, 0.937, 0.231), pattern);
    }

    Intersection getIntersection(Ray ray)
    {
        Intersection intersec;

        if (bvh) {
            intersec = bvh->Intersect(ray);
        }

        return intersec;
    }
    
    void Sample(Intersection &pos, float &pdf){
        bvh->Sample(pos, pdf);
        pos.emit = m->getEmission();
    }
    float getArea(){
        return area;
    }
    bool hasEmit(){
        return m->hasEmission();
    }

    Bounds3 bounding_box;
    std::unique_ptr<MyVector::Vector3f[]> vertices;
    uint32_t numTriangles;
    std::unique_ptr<uint32_t[]> vertexIndex;
    std::unique_ptr<MyVector::Vector2f[]> stCoordinates;

    std::vector<Triangle> triangles;

    BVHAccel* bvh;
    float area;

    Material* m;
};

inline bool Triangle::intersect(const Ray& ray) { return true; }
inline bool Triangle::intersect(const Ray& ray, float& tnear,
                                uint32_t& index) const
{
    return false;
}

inline Bounds3 Triangle::getBounds() { return Union(Bounds3(v0, v1), v2); }

inline Intersection Triangle::getIntersection(Ray ray)
{
    Intersection inter;

    if (dotProduct(ray.direction, normal) > 0)
        return inter;
    double u, v, t_tmp = 0;
    MyVector::Vector3f pvec_s1 = crossProduct(ray.direction, e2);
    double det = dotProduct(e1, pvec_s1);
    if (fabs(det) < EPSILON)
        return inter;

    double det_inv = 1. / det;
    MyVector::Vector3f tvec_s = ray.origin - v0;
    u = dotProduct(tvec_s, pvec_s1) * det_inv;
    if (u < 0 || u > 1)
        return inter;
    MyVector::Vector3f qvec_s2 = crossProduct(tvec_s, e1);
    v = dotProduct(ray.direction, qvec_s2) * det_inv;
    if (v < 0 || u + v > 1)
        return inter;
    t_tmp = dotProduct(e2, qvec_s2) * det_inv;

    // TODO find ray triangle intersection

    if (t_tmp < 0) return inter;
    inter.happened = true;
    inter.coords = ray(t_tmp);
    inter.normal = normal;
    inter.distance = t_tmp;
    inter.obj = this;
    inter.m = m;
    return inter;
}



inline MyVector::Vector3f Triangle::evalDiffuseColor(const MyVector::Vector2f&) const
{
    return MyVector::Vector3f(0.5, 0.5, 0.5);
}
