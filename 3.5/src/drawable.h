#pragma once

#include "mathutils.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/View.hpp>

#include <functional>
#include <vector>

using SphericalCoords = Vector<double, 3>;

class Object3D
{
public:
    Object3D(std::function<double(Vector2D)> func, const Vector2D &bound_lb, const Vector2D &bound_rt, int N = 101);
    void reinterpolate(int m);
    double fit(sf::View &view, sf::Window &window) const;
    void update_func(std::function<double(Vector2D)> func);
    void set_color(sf::Color color);
    const sf::VertexArray &operator()(const SphericalCoords &cam_coords, bool force = false) const;
    sf::Color color() const;

protected:
    void set_primitive(sf::PrimitiveType type);

private:
    std::pair<Vector2D, Vector2D> xy_boundaries() const;
    void update_data(const Vector2D &bound_lb, const Vector2D &bound_rt);
    std::function<double(Vector2D)> func;
    std::vector<Vector3D> data;
    mutable sf::VertexArray projected;
    mutable Vector2D angles;
    const int N;
};

class Axis : public Object3D
{
public:
    Axis(const Vector2D &bound_lb, const Vector2D &bound_rt)
        : Object3D([](Vector2D x) { return 0.; }, bound_lb, bound_rt, 2)
    {
        set_primitive(sf::PrimitiveType::LineStrip);
    }
};