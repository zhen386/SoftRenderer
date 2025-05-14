//
// Created by MacBook on 25-5-9.
//

//
// Created by LEI XU on 4/11/19.
//

#ifndef RASTERIZER_TRIANGLE_H
#define RASTERIZER_TRIANGLE_H

#include <Eigen>

using namespace Eigen;
class RasTriangle
{
public:
    Vector4f v[3]; /*the original coordinates of the triangle, v0, v1, v2 in
                      counter clockwise order*/
    // Vector4f v_obj[3]; /* Assignment3 */
    /*Per vertex values*/
    Vector3f color[3];      // color at each vertex;
    Vector2f tex_coords[3]; // texture u,v
    Vector3f normal[3];     // normal vector for each vertex

    // Texture *tex;
    RasTriangle();

    Eigen::Vector3f a() const { return v[0].head<3>(); }
    Eigen::Vector3f b() const { return v[1].head<3>(); }
    Eigen::Vector3f c() const { return v[2].head<3>(); }

    void setVertex(int ind, Eigen::Vector4f ver); /*set i-th vertex coordinates */
    // void setVertex(int ind, Eigen::Vector4f ver);
    void setNormal(int ind, Eigen::Vector3f n);   /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/
    Vector3f getColor() const { return color[0]*255; } // Only one color per triangle.
    void setTexCoord(int ind, Vector2f uv);
    std::array<Eigen::Vector4f, 3> toVector4() const;
};

#endif // RASTERIZER_TRIANGLE_H

