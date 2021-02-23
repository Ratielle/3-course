#pragma once
#include "types.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/View.hpp>

#include <functional>

#define DEF_PROP_SETTERS                   \
public:                                    \
    auto &set_type(sf::PrimitiveType type) \
    {                                      \
        Drawable::set_type(type);          \
        return *this;                      \
    }                                      \
    auto &set_color(sf::Color color)       \
    {                                      \
        Drawable::set_color(color);        \
        return *this;                      \
    }

class Drawable
{
public:
    Drawable(sf::PrimitiveType type, size_t n) : points(type, n) {}
    float fit(sf::View &view, sf::Window &window) const;
    std::pair<Arg, Arg> hboundaries() const;
    std::pair<Arg, Arg> vboundaries() const;
    sf::PrimitiveType type() const { return points.getPrimitiveType(); }
    sf::Color color() const { return points[0].color; }
    operator const sf::VertexArray() const;

protected:
    size_t size() const { return points.getVertexCount(); }
    Drawable &set_type(sf::PrimitiveType type);
    Drawable &set_color(sf::Color color);
    sf::VertexArray points;
};

struct AxisSettings
{
    struct
    {
        float lenght = 0.15f;
        float width = 0.07f;
    } head;
    sf::Color color = sf::Color::White;
};

class Axis : public Drawable
{
    DEF_PROP_SETTERS
public:
    Axis(sf::Vector2f begin, sf::Vector2f end, const AxisSettings &settings = AxisSettings());

private:
    float norm(const sf::Vector2f &vec) const;
    sf::Vector2f normalize(const sf::Vector2f &vec) const;
};

struct FunctionParameters
{
    int N = 101;
    sf::Color color = sf::Color::Green;
    sf::PrimitiveType type = sf::LinesStrip;
};

class Function : public Drawable
{
    DEF_PROP_SETTERS
public:
    Function(std::function<Val(Arg)> func, std::pair<Arg, Arg> boundaries, const FunctionParameters &params = FunctionParameters());
    Function &set_split(int n);
    int splits() const { return size() - 1; }
    void update_func(std::function<Val(Arg)> new_func);

private:
    void update_points(std::pair<Arg, Arg> boundaries);

    std::function<Val(Arg)> func;
};