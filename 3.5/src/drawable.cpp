#include "drawable.h"

double Object3D::fit(sf::View &view, sf::Window &window) const
{
    auto rect = projected.getBounds();
    auto aspect = static_cast<double>(window.getSize().y) / static_cast<double>(window.getSize().x);

    view.setSize(rect.width, rect.width * aspect);
    view.setCenter(sf::Vector2f(rect.left + 0.5 * rect.width, rect.top + 0.5 * rect.height));
    return rect.width / static_cast<double>(window.getSize().x);
}

Object3D::Object3D(std::function<double(Vector2D)> func, const Vector2D &bound_lb, const Vector2D &bound_rt, int N)
    : func(func), N(N), data(N * N), projected(sf::PrimitiveType::Points, N * N)
{
    update_data(bound_lb, bound_rt);
}

void Object3D::update_data(const Vector2D &bound_lb, const Vector2D &bound_rt)
{
    double step_x = (bound_rt[0] - bound_lb[0]) / (N - 1);
    double step_y = (bound_rt[1] - bound_lb[1]) / (N - 1);

    double x = bound_lb[0];
    for (int i = 0; i < N; ++i, x += step_x)
    {
        double y = bound_lb[1];
        for (int j = 0; j < N; ++j, y += step_y)
            data[i * N + j] = {x, y, func({x, y})};
    }
}

void Object3D::update_func(std::function<double(Vector2D)> f)
{
    func = f;
    auto [from, to] = xy_boundaries();
    update_data(from, to);
}

std::pair<Vector2D, Vector2D> Object3D::xy_boundaries() const
{
    auto lb = data.front();
    auto rt = data.back();
    return {{lb[0], lb[1]}, {rt[0], rt[1]}};
}
void Object3D::set_primitive(sf::PrimitiveType type)
{
    projected.setPrimitiveType(type);
}

const sf::VertexArray &Object3D::operator()(const SphericalCoords &cam_coords, bool force) const
{
    auto [r, fi, th] = cam_coords.v;
    if (force || (angles - Vector2D({fi, th})).norm() > 1e-6)
    {
        angles = {fi, th};
        Vector3D y_axis = {-std::cos(fi) * std::cos(th), -std::sin(fi) * std::cos(th), std::sin(th)};
        Vector3D x_axis = {-std::sin(fi), std::cos(fi), 0};
        Vector3D dir = {-std::cos(fi) * std::sin(th), -std::sin(fi) * std::sin(th), -std::cos(th)};
        Vector3D cam = Vector3D::scale(dir, -r);
        auto project = [&](const Vector3D &p) {
            auto v = cam - p;
            auto l = Vector3D::scale(v, cam.norm() / Vector3D::dot(v, dir)) + cam;
            return sf::Vector2f(Vector3D::dot(l, x_axis), Vector3D::dot(l, y_axis));
        };
        for (int i = 0; i < N * N; ++i)
            projected[i].position = project(data[i]);
    }
    return projected;
}

void Object3D::set_color(sf::Color color)
{
    for (int i = 0; i < N * N; ++i)
        projected[i].color = color;
}

sf::Color Object3D::color() const
{
    return projected[0].color;
}
