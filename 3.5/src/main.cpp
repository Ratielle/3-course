#include "drawable.h"
#include "input.h"
#include "interpolation.h"

#include <imgui-sfml/imgui-SFML.h>
#include <imgui/imgui.h>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <functional>
#include <string>
#include <vector>

constexpr int WINDOW_H = 480;
constexpr int WINDOW_W = 720;

using Boundaries = Vector2D;

struct FunctionContext
{
    FunctionContext(Object3D &&func, std::string &&name) : func(func), name(name) {}
    Object3D func;
    std::string name = "not specified";
};

ImColor transform_color(sf::Color c)
{
    return ImColor(static_cast<int>(c.r), static_cast<int>(c.g), static_cast<int>(c.b), static_cast<int>(c.a));
}

sf::Color transform_color(ImColor c)
{
    auto v = c.Value;
    const auto f = 255.f;
    return sf::Color(static_cast<sf::Uint8>(v.x * f), static_cast<sf::Uint8>(v.y * f),
                     static_cast<sf::Uint8>(v.z * f), static_cast<sf::Uint8>(v.w * f));
}

int main(int argc, char *argv[])
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "Interpolation plotter");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    sf::View view({0, 0}, {WINDOW_W * 1.0, WINDOW_H * 1.0});

    InputParameters ip(argc, argv);

    auto original_func = [](Vector2D r) { return InputFunc3D(r[0], r[1]); };

    Boundaries lb = {ip.left_boundary, ip.left_boundary};
    Boundaries rt = {ip.right_boundary, ip.right_boundary};
    std::vector<FunctionContext> functions; //< функции для отрисовки
    {
        functions.emplace_back(Object3D(original_func, lb, rt), "original");
        functions.back().func({100, 0, 0}); //< Заранее сделаем проекцию
        functions.back().func.set_color(sf::Color::Green);
    }
    int original_id = functions.size() - 1;
    {
        auto func = Interpolation3D(original_func, lb, rt, ip.interpolation_points_num);
        functions.emplace_back(Object3D(func, lb, rt), "interpolation");
        functions.back().func({100, 0, 0}); //< Заранее сделаем проекцию
        functions.back().func.set_color(sf::Color::Red);
    }
    int interpolation_id = functions.size() - 1;

    auto x_axis = Axis({-10, 0}, {10, 0});
    auto y_axis = Axis({0, -10}, {0, 10});

    sf::Clock deltaClock;

    sf::Vector2f prev_position;
    bool moving = false;

    float zoom = functions[original_id].func.fit(view, window);
    float def_zoom = zoom * 100;
    Vector3D cam = {10, 0, 0};
    int current_id = original_id;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Up)
                    cam[2] += .01;
                if (event.key.code == sf::Keyboard::Down)
                    cam[2] -= .01;
                if (event.key.code == sf::Keyboard::Left)
                    cam[1] += .01;
                if (event.key.code == sf::Keyboard::Right)
                    cam[1] -= .01;
            }
            if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
            {
                if (event.mouseWheelScroll.delta < 0)
                {
                    zoom *= 1.1f;
                    view.zoom(1.1f);
                }
                else if ((event.mouseWheelScroll.delta > 0))
                {
                    zoom /= 1.1f;
                    view.zoom(1.0f / 1.1f);
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
            {
                auto &mouse = event.mouseButton;
                prev_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                moving = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
            {
                moving = false;
            }
            else if (moving && event.type == sf::Event::MouseMoved)
            {
                auto &mouse = event.mouseButton;
                auto delta = prev_position - window.mapPixelToCoords(sf::Mouse::getPosition(window));
                view.move(delta * zoom);
                prev_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            }
        }

        int n = ip.interpolation_points_num;
        int dist = cam.v[0];
        ImColor color, prev_color;
        color = prev_color = transform_color(functions[current_id].func.color());
        ImGui::SFML::Update(window, deltaClock.restart());

        if (ImGui::Begin("Plotter settings"))
        {
            ImGui::Text("Pan: mouse right button");
            ImGui::Text("Scale: mouse wheel up/down");
            if (ImGui::Button("Fit"))
                zoom = functions[original_id].func.fit(view, window);
            ImGui::SameLine();
            ImGui::Text("Zoom: %.2f%%", def_zoom / zoom);
            ImGui::SameLine();
            ImGui::Text("Center: {%.2f,%.2f}", view.getCenter().x, -view.getCenter().y);
            if (ImGui::ListBoxHeader("Functions", functions.size()))
            {
                for (int i = 0; i < functions.size(); ++i)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, transform_color(functions[i].func.color()).Value);
                    if (ImGui::Selectable(functions[i].name.c_str(), i == current_id))
                        current_id = i;
                    ImGui::PopStyleColor();
                }
                ImGui::ListBoxFooter();
            }
            ImGui::SliderInt("Points num", &n, 2, ip.plot_points_num / 10);
            ImGui::SliderInt("Cam distance", &dist, 1, 1000);
            ImGui::ColorEdit4("##picker", (float *)&color);
            ImGui::End();
        }
        bool cam_changed = static_cast<int>(cam.v[0]) != dist;
        cam.v[0] = dist;

        if (n != ip.interpolation_points_num)
        {
            auto func = Interpolation3D(original_func, lb, rt, n);
            functions[interpolation_id].func.update_func(func);
        }

        if (color != prev_color)
            functions[current_id].func.set_color(transform_color(color));

        window.clear();
        window.setView(view);
        window.draw(x_axis(cam));
        window.draw(y_axis(cam));
        window.draw(functions[original_id].func(cam, cam_changed));
        window.draw(functions[interpolation_id].func(cam, cam_changed || n != ip.interpolation_points_num));
        window.setView(window.getDefaultView());

        ImGui::SFML::Render(window);
        window.display();
        cam.v[0] = dist;
        ip.interpolation_points_num = n;
    }
}
