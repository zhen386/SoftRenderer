//
// Created by LEI XU on 5/13/19.
//
#pragma once
#ifndef RAYTRACING_VECTOR_H
#define RAYTRACING_VECTOR_H

#include <iostream>
#include <cmath>
#include <algorithm>

namespace MyVector {
    class Vector3f {
    public:
        float x, y, z;
        Vector3f() : x(0), y(0), z(0) {}
        Vector3f(float xx) : x(xx), y(xx), z(xx) {}
        Vector3f(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
        Vector3f operator * (const float &r) const { return Vector3f(x * r, y * r, z * r); }
        Vector3f operator / (const float &r) const { return Vector3f(x / r, y / r, z / r); }

        float norm() {return std::sqrt(x * x + y * y + z * z);}
        Vector3f normalized() {
            float n = std::sqrt(x * x + y * y + z * z);
            return Vector3f(x / n, y / n, z / n);
        }

        Vector3f operator * (const Vector3f &v) const { return Vector3f(x * v.x, y * v.y, z * v.z); }
        Vector3f operator - (const Vector3f &v) const { return Vector3f(x - v.x, y - v.y, z - v.z); }
        Vector3f operator + (const Vector3f &v) const { return Vector3f(x + v.x, y + v.y, z + v.z); }
        Vector3f operator - () const { return Vector3f(-x, -y, -z); }
        Vector3f& operator += (const Vector3f &v) { x += v.x, y += v.y, z += v.z; return *this; }
        friend Vector3f operator * (const float &r, const Vector3f &v)
        { return Vector3f(v.x * r, v.y * r, v.z * r); }
        friend std::ostream & operator << (std::ostream &os, const Vector3f &v)
        { return os << v.x << ", " << v.y << ", " << v.z; }
        double       operator[](int index) const;
        double&      operator[](int index);


        static Vector3f Min(const Vector3f &p1, const Vector3f &p2) {
            return Vector3f(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                           std::min(p1.z, p2.z));
        }

        static Vector3f Max(const Vector3f &p1, const Vector3f &p2) {
            return Vector3f(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
                           std::max(p1.z, p2.z));
        }
    };
    inline double Vector3f::operator[](int index) const {
        return (&x)[index];
    }


    class Vector2f
    {
    public:
        Vector2f() : x(0), y(0) {}
        Vector2f(float xx) : x(xx), y(xx) {}
        Vector2f(float xx, float yy) : x(xx), y(yy) {}
        Vector2f operator * (const float &r) const { return Vector2f(x * r, y * r); }
        Vector2f operator + (const Vector2f &v) const { return Vector2f(x + v.x, y + v.y); }
        float x, y;
    };

    inline Vector3f lerp(const Vector3f &a, const Vector3f& b, const float &t)
    { return a * (1 - t) + b * t; }

    inline Vector3f normalize(const Vector3f &v)
    {
        float mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
        if (mag2 > 0) {
            float invMag = 1 / sqrtf(mag2);
            return Vector3f(v.x * invMag, v.y * invMag, v.z * invMag);
        }

        return v;
    }

    inline float dotProduct(const Vector3f &a, const Vector3f &b)
    { return a.x * b.x + a.y * b.y + a.z * b.z; }

    inline Vector3f crossProduct(const Vector3f &a, const Vector3f &b)
    {
        return Vector3f(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
        );
    }

    class Vector4f {
    public:
        float x, y, z, w;
        Vector4f() : x(0), y(0), z(0) , w(0) {}
        Vector4f(float xx) : x(xx), y(xx), z(xx), w(xx) {}
        Vector4f(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {}
        Vector4f operator * (const float &r) const { return Vector4f(x * r, y * r, z * r, w * r); }
        Vector4f operator / (const float &r) const { return Vector4f(x / r, y / r, z / r, w / r); }

        float norm() {return std::sqrt(x * x + y * y + z * z + w * w); ;}
        Vector4f normalized() {
            float n = std::sqrt(x * x + y * y + z * z + w * w);
            return Vector4f(x / n, y / n, z / n, w / n);
        }

        Vector4f operator * (const Vector4f &v) const { return Vector4f(x * v.x, y * v.y, z * v.z, w * v.w); }
        Vector4f operator - (const Vector4f &v) const { return Vector4f(x - v.x, y - v.y, z - v.z, w - v.w); }
        Vector4f operator + (const Vector4f &v) const { return Vector4f(x + v.x, y + v.y, z + v.z, w + v.w); }
        Vector4f operator - () const { return Vector4f(-x, -y, -z, -w); }
        Vector4f& operator += (const Vector4f &v) { x += v.x, y += v.y, z += v.z, w += v.w; return *this; }
        friend Vector4f operator * (const float &r, const Vector4f &v)
        { return Vector4f(v.x * r, v.y * r, v.z * r, v.w * r); }
        friend std::ostream & operator << (std::ostream &os, const Vector4f &v)
        { return os << v.x << ", " << v.y << ", " << v.z << ", " << v.w; }
        double       operator[](int index) const;
        double&      operator[](int index);


        static Vector4f Min(const Vector4f &p1, const Vector4f &p2) {
            return Vector4f(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                           std::min(p1.z, p2.z), std::min(p1.w, p2.w));
        }

        static Vector4f Max(const Vector4f &p1, const Vector4f &p2) {
            return Vector4f(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
                           std::max(p1.z, p2.z), std::max(p1.w, p2.w));
        }
    };
    inline double Vector4f::operator[](int index) const {
        return (&x)[index];
    }
}

#endif //RAYTRACING_VECTOR_H
