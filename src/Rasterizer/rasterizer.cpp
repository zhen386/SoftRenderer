//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>
#include <stdexcept>
#include "RasTriangle.hpp"
#include "Shader.cpp"


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

static bool insideTriangle(const float x, const float y, const Vector4f* _v)
{
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    // Eigen::Vector3f pA((x - _v[0].x()), (y - _v[0].y()), 0);
    // Eigen::Vector3f pB((x - _v[1].x()), (y - _v[1].y()), 0);
    // Eigen::Vector3f pC((x - _v[2].x()), (y - _v[2].y()), 0);
    //
    // Eigen::Vector3f AB(_v[1].x() - _v[0].x(), _v[1].y() - _v[0].y(), 0);
    // Eigen::Vector3f BC(_v[2].x() - _v[1].x(), _v[2].y() - _v[1].y(), 0);
    // Eigen::Vector3f CA(_v[0].x() - _v[2].x(), _v[0].y() - _v[2].y(), 0);
    //
    // bool sign[3] = {false};
    // sign[0] = pA.cross(AB).z() > 0;
    // sign[1] = pB.cross(BC).z() > 0;
    // sign[2] = pC.cross(CA).z() > 0;
    //
    // return sign[0] == sign[1] && sign[1] == sign[2];

    auto [alpha, beta, gamma] = computeBarycentric2D(x, y, _v);
    return alpha > 0 && alpha < 1 && beta > 0 && beta < 1 && gamma > 0 && gamma < 1;
}

// Bresenham's line drawing algorithm
// Code taken from a stack overflow answer: https://stackoverflow.com/a/16405254
void rst::rasterizer::draw_line(Eigen::Vector3f begin, Eigen::Vector3f end)
{
    auto x1 = begin.x();
    auto y1 = begin.y();
    auto x2 = end.x();
    auto y2 = end.y();

    Eigen::Vector3f line_color = {255, 255, 255};

    int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;

    dx=x2-x1;
    dy=y2-y1;
    dx1=fabs(dx);
    dy1=fabs(dy);
    px=2*dy1-dx1;
    py=2*dx1-dy1;

    if(dy1<=dx1)
    {
        if(dx>=0)
        {
            x=x1;
            y=y1;
            xe=x2;
        }
        else
        {
            x=x2;
            y=y2;
            xe=x1;
        }
        Eigen::Vector3f point = Eigen::Vector3f(x, y, 1.0f);
        set_pixel(point,line_color);
        for(i=0;x<xe;i++)
        {
            x=x+1;
            if(px<0)
            {
                px=px+2*dy1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    y=y+1;
                }
                else
                {
                    y=y-1;
                }
                px=px+2*(dy1-dx1);
            }
//            delay(0);
            Eigen::Vector3f point = Eigen::Vector3f(x, y, 1.0f);
            set_pixel(point,line_color);
        }
    }
    else
    {
        if(dy>=0)
        {
            x=x1;
            y=y1;
            ye=y2;
        }
        else
        {
            x=x2;
            y=y2;
            ye=y1;
        }
        Eigen::Vector3f point = Eigen::Vector3f(x, y, 1.0f);
        set_pixel(point,line_color);
        for(i=0;y<ye;i++)
        {
            y=y+1;
            if(py<=0)
            {
                py=py+2*dx1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    x=x+1;
                }
                else
                {
                    x=x-1;
                }
                py=py+2*(dx1-dy1);
            }
//            delay(0);
            Eigen::Vector3f point = Eigen::Vector3f(x, y, 1.0f);
            set_pixel(point,line_color);
        }
    }
}

void rst::rasterizer::drawWireFrame(rst::pos_buf_id pos_buffer, rst::ind_buf_id ind_buffer, rst::Primitive type)
{
    if (type != rst::Primitive::Triangle)
    {
        throw std::runtime_error("Drawing primitives other than triangle is not implemented yet!");
    }
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];

    float f1 = (100 - 0.1) / 2.0;
    float f2 = (100 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        RasTriangle t;

        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]],1.f),
                mvp * to_vec4(buf[i[1]],1.f),
                mvp * to_vec4(buf[i[2]],1.f)
        };

        for (auto& vec : v) {
            vec /= vec.w();
        }

        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i]);
            t.setVertex(i, v[i]);
            t.setVertex(i, v[i]);
        }

        t.setColor(0, 255.0,  0.0,  0.0);
        t.setColor(1, 0.0  ,255.0,  0.0);
        t.setColor(2, 0.0  ,  0.0,255.0);

        rasterize_wireframe(t);
    }
}

void rst::rasterizer::rasterize_wireframe(const RasTriangle& t)
{
    draw_line(t.c(), t.a());
    draw_line(t.c(), t.b());
    draw_line(t.b(), t.a());
}

void rst::rasterizer::drawColoredTriangle(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type, bool SSAA = false)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        RasTriangle t;

        // 对三角形三个点进行MVP变换
        Eigen::Vector4f v[] = {
            mvp * to_vec4(buf[i[0]],1.f),
            mvp * to_vec4(buf[i[1]],1.f),
            mvp * to_vec4(buf[i[2]],1.f)
        };

        // Homogeneous division
        // 齐次坐标归一化
        for (auto& vec : v) {
            vec /= vec.w();
        }

        // Viewport transformation
        // 视口变换，将MVP变换后的近平面移至屏幕
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i]);
            t.setVertex(i, v[i]);
            t.setVertex(i, v[i]);
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t, SSAA);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const RasTriangle& t, bool SSAA) {
    auto v = t.toVector4();

    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle
    float xLeft = v[0].x(), yBottom = v[0].y();
    float xRight = xLeft, yTop = yBottom;

    for (int i = 0; i < 3; ++i) {
        xLeft = std::min(xLeft, v[i].x());
        xRight = std::max(xRight, v[i].x());
        yBottom = std::min(yBottom, v[i].y());
        yTop = std::max(yTop, v[i].y());
    }

    if (SSAA) {

        std::vector<std::pair<float,float>> offset = {{0.25,0.25}, {0.25, 0.75}, {0.75, 0.25}, {0.75, 0.75}};
        for (int y = (int)(floor(yBottom)); y < (int)(ceil(yTop)); ++y) {
            for (int x = (int)(floor(xLeft)); x < (int)(ceil(xRight)); ++x) {
                int ras = 0;

                Eigen::Vector3f color (0.0, 0.0, 0.0);
                for (int i = 0; i < 4; ++i) {
                    float xi = (float)x + offset[i].first, yi = (float)y + offset[i].second;
                    if (insideTriangle(xi, yi, t.v)) {
                        int index = get_index(x, y) * 4 + i;   // index计算要翻4倍
                        auto[alpha, beta, gamma] = computeBarycentric2D((float)x + offset[i].first, (float)y+offset[i].second, t.v);
                        float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                        float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                        z_interpolated *= w_reciprocal;


                        if (depth_buf_SSAA[index] < z_interpolated) {

                            ras++;
                            depth_buf_SSAA[index] = z_interpolated;
                            color += t.getColor();
                        }
                    }
                }
                if (ras) {
                    set_pixel(Vector3f(x, y, 0), color / 4.0 + frame_buf[get_index(x, y)]);  // 综合多轮渲染的结果
                }
            }
        }
    } else {
        for (int y = (int)(floor(yBottom)); y < (int)(ceil(yTop)); ++y) {
            for (int x = (int)(floor(xLeft)); x < (int)(ceil(xRight)); ++x) {
                if (insideTriangle((float)x+0.5, (float)y+0.5, t.v)) {

                    int index = get_index(x, y);
                    auto[alpha, beta, gamma] = computeBarycentric2D((float)x+0.5, (float)y+0.5, t.v);
                    float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    // std::cout << 1 << std::endl;

                    if (depth_buf[index] < z_interpolated) {
                        depth_buf[index] = z_interpolated;
                        set_pixel(Vector3f(x, y, 0), t.getColor());
                        // std::cout << 1 << std::endl;
                    }
                }
            }
        }
    }
    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
}

static Eigen::Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Eigen::Vector3f& vert3, float weight)
{
    return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

static Eigen::Vector2f interpolate(float alpha, float beta, float gamma, const Eigen::Vector2f& vert1, const Eigen::Vector2f& vert2, const Eigen::Vector2f& vert3, float weight)
{
    auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
    auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);

    u /= weight;
    v /= weight;

    return Eigen::Vector2f(u, v);
}

void rst::rasterizer::rasterize_triangle_obj(const RasTriangle& t, const std::array<Eigen::Vector3f, 3>& view_pos, bool SSAA)
{
    // TODO: From your HW3, get the triangle rasterization code.
    auto v = t.toVector4();
    float xLeft = v[0].x(), yBottom = v[0].y();
    float xRight = xLeft, yTop = yBottom;

    for (int i = 0; i < 3; ++i) {
        xLeft = std::min(xLeft, v[i].x());
        xRight = std::max(xRight, v[i].x());
        yBottom = std::min(yBottom, v[i].y());
        yTop = std::max(yTop, v[i].y());
    }

    if (SSAA) {
        std::vector<std::pair<float,float>> offset = {{0.25,0.25}, {0.25, 0.75}, {0.75, 0.25}, {0.75, 0.75}};
        for (int y = (int)(floor(yBottom)); y < (int)(ceil(yTop)); ++y) {
            for (int x = (int)(floor(xLeft)); x < (int)(ceil(xRight)); ++x) {
                int ras = 0;
                Eigen::Vector3f color (0.0, 0.0, 0.0);
                for (int i = 0; i < 4; ++i) {
                    float xi = (float)x + offset[i].first, yi = (float)y + offset[i].second;
                    if (insideTriangle(xi, yi, t.v)) {
                        int index = get_index(x, y)*4 + i;   // index计算要翻4倍

                        auto[alpha, beta, gamma] = computeBarycentric2D((float)x + offset[i].first, (float)y+offset[i].second, t.v);
                        float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                        float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                        z_interpolated *= w_reciprocal;

                        if (depth_buf_SSAA[index] < z_interpolated) {
                            auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
                            auto interpolated_normal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1);
                            auto interpolated_texcoords = interpolate(alpha, beta, gamma, t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], 1);
                            auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);

                            fragment_shader_payload payload( interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ? &*texture : nullptr);
                            payload.view_pos = interpolated_shadingcoords;
                            auto pixel_color = fragment_shader(payload);

                            ras++;
                            depth_buf_SSAA[index] = z_interpolated;
                            //set_pixel_obj(Vector2i(x, y), pixel_color);
                            color += pixel_color;
                        }
                    }
                }
                if (ras == 4) {
                    set_pixel_obj(Vector2i(x, y), color / 4.0 /* + frame_buf[get_index(x, y)]*/);  // 综合多轮渲染的结果
                } else if (ras >= 1) {
                    set_pixel_obj(Vector2i(x, y), color / 4.0 + frame_buf[get_index(x, y)] * (4-ras/4));  // 综合多轮渲染的结果
                }
            }
        }
    } else {
        for (int y = static_cast<int>(floor(yBottom)); y < static_cast<int>(ceil(yTop)); ++y) {
            for (int x = static_cast<int>(floor(xLeft)); x < static_cast<int>(ceil(xRight)); ++x) {
                int index = get_index(x, y);
                if (insideTriangle((float)x+0.5, (float)y+0.5, t.v)) {

                    auto[alpha, beta, gamma] = computeBarycentric2D((float)x+0.5, (float)y+0.5, t.v);
                    float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;

                    if (depth_buf[index] < z_interpolated) {

                        auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
                        auto interpolated_normal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1);
                        auto interpolated_texcoords = interpolate(alpha, beta, gamma, t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], 1);
                        auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);

                        depth_buf[index] = z_interpolated;

                        fragment_shader_payload payload( interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ? &*texture : nullptr);
                        payload.view_pos = interpolated_shadingcoords;
                        auto pixel_color = fragment_shader(payload);
                        set_pixel_obj(Vector2i(x, y), pixel_color);
                    }
                }
            }
        }
    }

    // auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    // float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    // float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    // z_interpolated *= w_reciprocal;

    // TODO: Interpolate the attributes:
    // auto interpolated_color
    // auto interpolated_normal
    // auto interpolated_texcoords
    // auto interpolated_shadingcoords

    // Use: fragment_shader_payload payload( interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ? &*texture : nullptr);
    // Use: payload.view_pos = interpolated_shadingcoords;
    // Use: Instead of passing the triangle's color directly to the frame buffer, pass the color to the shaders first to get the final color;
    // Use: auto pixel_color = fragment_shader(payload);

}


void rst::rasterizer::drawObj(std::vector<RasTriangle *> &TriangleList, bool SSAA = false) {

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (const auto& t:TriangleList)
    {
        RasTriangle newtri = *t;

        std::array<Eigen::Vector4f, 3> mm {
                (view * model * t->v[0]),
                (view * model * t->v[1]),
                (view * model * t->v[2])
        };

        std::array<Eigen::Vector3f, 3> viewspace_pos;

        std::transform(mm.begin(), mm.end(), viewspace_pos.begin(), [](auto& v) {
            return v.template head<3>();
        });

        Eigen::Vector4f v[] = {
                mvp * t->v[0],
                mvp * t->v[1],
                mvp * t->v[2]
        };
        //Homogeneous division
        // for (auto& vec : v) {
        //     vec.x()/=vec.w();
        //     vec.y()/=vec.w();
        //     vec.z()/=vec.w();
        // }

        Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
        Eigen::Vector4f n[] = {
                inv_trans * to_vec4(t->normal[0], 0.0f),
                inv_trans * to_vec4(t->normal[1], 0.0f),
                inv_trans * to_vec4(t->normal[2], 0.0f)
        };

        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            //screen space coordinates
            newtri.setVertex(i, v[i]);
        }

        for (int i = 0; i < 3; ++i)
        {
            //view space normal
            newtri.setNormal(i, n[i].head<3>());
        }

        newtri.setColor(0, 148,121.0,92.0);
        newtri.setColor(1, 148,121.0,92.0);
        newtri.setColor(2, 148,121.0,92.0);

        // Also pass view space vertice position
        rasterize_triangle_obj(newtri, viewspace_pos, SSAA);
    }
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), -std::numeric_limits<float>::infinity());
        std::fill(depth_buf_SSAA.begin(), depth_buf_SSAA.end(), -std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    depth_buf_SSAA.resize(w * h * 4);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    if (point.x() < 0 || point.x() >= width ||
        point.y() < 0 || point.y() >= height) return;
    auto ind = (height-point.y())*width + point.x();
    frame_buf[ind] = color;
}

void rst::rasterizer::set_pixel_obj(const Vector2i &point, const Eigen::Vector3f &color)
{
    //old index: auto ind = point.y() + point.x() * width;
    int ind = (height-point.y())*width + point.x();
    frame_buf[ind] = color;
}

void rst::rasterizer::set_vertex_shader(std::function<Eigen::Vector3f(vertex_shader_payload)> vert_shader)
{
    vertex_shader = vert_shader;
}

void rst::rasterizer::set_fragment_shader(std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader)
{
    fragment_shader = frag_shader;
}


