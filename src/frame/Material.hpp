//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "Vector.hpp"

enum MaterialType { DIFFUSE};

class Material{
private:

    // Compute reflection direction
    MyVector::Vector3f reflect(const MyVector::Vector3f &I, const MyVector::Vector3f &N) const
    {
        return I - 2 * dotProduct(I, N) * N;
    }

    // Compute refraction direction using Snell's law
    //
    // We need to handle with care the two possible situations:
    //
    //    - When the ray is inside the object
    //
    //    - When the ray is outside.
    //
    // If the ray is outside, you need to make cosi positive cosi = -N.I
    //
    // If the ray is inside, you need to invert the refractive indices and negate the normal N
    MyVector::Vector3f refract(const MyVector::Vector3f &I, const MyVector::Vector3f &N, const float &ior) const
    {
        float cosi = clamp(-1, 1, dotProduct(I, N));
        float etai = 1, etat = ior;
        MyVector::Vector3f n = N;
        if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= -N; }
        float eta = etai / etat;
        float k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
    }

    // Compute Fresnel equation
    //
    // \param I is the incident view direction
    //
    // \param N is the normal at the intersection point
    //
    // \param ior is the material refractive index
    //
    // \param[out] kr is the amount of light reflected
    void fresnel(const MyVector::Vector3f &I, const MyVector::Vector3f &N, const float &ior, float &kr) const
    {
        float cosi = clamp(-1, 1, dotProduct(I, N));
        float etai = 1, etat = ior;
        if (cosi > 0) {  std::swap(etai, etat); }
        // Compute sini using Snell's law
        float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
        // Total internal reflection
        if (sint >= 1) {
            kr = 1;
        }
        else {
            float cost = sqrtf(std::max(0.f, 1 - sint * sint));
            cosi = fabsf(cosi);
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            kr = (Rs * Rs + Rp * Rp) / 2;
        }
        // As a consequence of the conservation of energy, transmittance is given by:
        // kt = 1 - kr;
    }

    MyVector::Vector3f toWorld(const MyVector::Vector3f &a, const MyVector::Vector3f &N){
        MyVector::Vector3f B, C;
        if (std::fabs(N.x) > std::fabs(N.y)){
            float invLen = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
            C = MyVector::Vector3f(N.z * invLen, 0.0f, -N.x *invLen);
        }
        else {
            float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
            C = MyVector::Vector3f(0.0f, N.z * invLen, -N.y *invLen);
        }
        B = crossProduct(C, N);
        return a.x * B + a.y * C + a.z * N;
    }

public:
    MaterialType m_type;
    //MyVector::Vector3f m_color;
    MyVector::Vector3f m_emission;
    float ior;
    MyVector::Vector3f Kd, Ks;
    float specularExponent;
    //Texture tex;

    inline Material(MaterialType t=DIFFUSE, MyVector::Vector3f e=MyVector::Vector3f(0,0,0));
    inline MaterialType getType();
    //inline MyVector::Vector3f getColor();
    inline MyVector::Vector3f getColorAt(double u, double v);
    inline MyVector::Vector3f getEmission();
    inline bool hasEmission();

    // sample a ray by Material properties
    inline MyVector::Vector3f sample(const MyVector::Vector3f &wi, const MyVector::Vector3f &N);
    // given a ray, calculate the PdF of this ray
    inline float pdf(const MyVector::Vector3f &wi, const MyVector::Vector3f &wo, const MyVector::Vector3f &N);
    // given a ray, calculate the contribution of this ray
    inline MyVector::Vector3f eval(const MyVector::Vector3f &wi, const MyVector::Vector3f &wo, const MyVector::Vector3f &N);

};

Material::Material(MaterialType t, MyVector::Vector3f e){
    m_type = t;
    //m_color = c;
    m_emission = e;
}

MaterialType Material::getType(){return m_type;}
///MyVector::Vector3f Material::getColor(){return m_color;}
MyVector::Vector3f Material::getEmission() {return m_emission;}
bool Material::hasEmission() {
    if (m_emission.norm() > EPSILON) return true;
    else return false;
}

MyVector::Vector3f Material::getColorAt(double u, double v) {
    return MyVector::Vector3f();
}


MyVector::Vector3f Material::sample(const MyVector::Vector3f &wi, const MyVector::Vector3f &N){
    switch(m_type){
        case DIFFUSE:
        {
            // uniform sample on the hemisphere
            float x_1 = get_random_float(), x_2 = get_random_float();
            float z = std::fabs(1.0f - 2.0f * x_1);
            float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
            MyVector::Vector3f localRay(r*std::cos(phi), r*std::sin(phi), z);
            return toWorld(localRay, N);
            
            break;
        }
    }
}

float Material::pdf(const MyVector::Vector3f &wi, const MyVector::Vector3f &wo, const MyVector::Vector3f &N){
    switch(m_type){
        case DIFFUSE:
        {
            // uniform sample probability 1 / (2 * PI)
            if (dotProduct(wo, N) > 0.0f)
                return 0.5f / M_PI;
            else
                return 0.0f;
            break;
        }
    }
}

MyVector::Vector3f Material::eval(const MyVector::Vector3f &wi, const MyVector::Vector3f &wo, const MyVector::Vector3f &N){
    switch(m_type){
        case DIFFUSE:
        {
            // calculate the contribution of diffuse   model
            float cosalpha = dotProduct(N, wo);
            if (cosalpha > 0.0f) {
                MyVector::Vector3f diffuse = Kd / M_PI;
                return diffuse;
            }
            else
                return MyVector::Vector3f(0.0f);
            break;
        }
    }
}

#endif //RAYTRACING_MATERIAL_H
