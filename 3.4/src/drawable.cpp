#include "drawable.h"

float Drawable::fit(sf::View &view, sf::Window &window) const
{
    auto rect = points.getBounds();
    auto aspect = static_cast<float>(window.getSize().y) / static_cast<float>(window.getSize().x);

    view.setSize(rect.width, rect.width * aspect);
    view.setCenter(sf::Vector2f(rect.left + 0.5f * rect.width, rect.top + 0.5f * rect.height));
    return rect.width / static_cast<float>(window.getSize().x);
}

std::pair<Arg, Arg> Drawable::hboundaries() const
{
    auto rect = points.getBounds();
    return {rect.left, rect.left + rect.width};
}

std::pair<Arg, Arg> Drawable::vboundaries() const
{
    auto rect = points.getBounds();
    return {rect.top - rect.height, rect.top};
}

Drawable::operator const sf::VertexArray() const
{
    return points;
}

Drawable &Drawable::set_type(sf::PrimitiveType type)
{
    points.setPrimitiveType(type);
    return *this;
}
Drawable &Drawable::set_color(sf::Color color)
{
    for (int i = 0; i < size(); ++i)
        points[i].color = color;
    return *this;
}

Axis::Axis(sf::Vector2f begin, sf::Vector2f end, const AxisSettings &settings) : Drawable(sf::LineStrip, 5)
{
    begin.y = -begin.y;
    end.y = -end.y;
    points[0].position = begin;
    points[1].position = end;
    auto dir = normalize(end - begin);
    auto head_end = end - dir * static_cast<float>(settings.head.lenght);
    dir = {-dir.y, dir.x};
    dir *= static_cast<float>(settings.head.width * 0.5);
    points[2].position = head_end + dir;
    points[3].position = head_end - dir;
    points[4].position = end;

    set_color(settings.color);
}

float Axis::norm(const sf::Vector2f &vec) const
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

sf::Vector2f Axis::normalize(const sf::Vector2f &vec) const
{
    auto len = norm(vec);
    if (len > 1e-7)
        return vec * 1.f / norm(vec);
    else
        return {0, 0};
}

Function::Function(std::function<Val(Arg)> func, std::pair<Arg, Arg> boundaries, const FunctionParameters &params)
    : Drawable(params.type, params.N), func(func)
{
    update_points(boundaries);
    set_color(params.color);
}
Function &Function::set_split(int n)
{
    if (n + 1 != size())
    {
        auto boundaries = hboundaries();
        auto old_size = size();
        points.resize(n + 1);
        update_points(boundaries);

        if (old_size < size())
            set_color(points[0].color);
    }
    return *this;
}

void Function::update_points(std::pair<Arg, Arg> boundaries)
{
    auto [x1, x2] = boundaries;
    auto step = (x2 - x1) / (size() - 1);
    for (int i = 0; i < size(); ++i)
    {
        auto x = x1 + step * i;
        points[i].position = sf::Vector2f(x, -func(x));
    }
}

void Function::update_func(std::function<Val(Arg)> new_func)
{
    func = new_func;
    auto boundaries = hboundaries();
    update_points(boundaries);
}
