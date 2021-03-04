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

using Boundaries = std::pair<Arg, Arg>;

struct FunctionContext
{
    FunctionContext(Function func, std::string &&name) : func(func), name(name) {}
    Function func;
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

    std::vector<FunctionContext> functions; //< функции для отрисовки
    auto boundaries = Boundaries{ip.left_boundary, ip.right_boundary};
    // добавляем исходную функцию
    std::function<Val(Arg)> f_original = InputFunc;
    {
        FunctionParameters fp{ip.plot_points_num};
        functions.emplace_back(Function(f_original, boundaries, fp), "original");
    }
    auto original_id = functions.size() - 1;
    auto current_id = functions.size() - 1;
    // добавляем интерполяцию
    std::function<Val(Arg)> f_inter =
        !ip.use_cubic ? create_linear(f_original, boundaries, ip.interpolation_points_num)
                      : create_cubic(f_original, boundaries, ip.interpolation_points_num, ip.left_derivative, ip.right_derivative);
    {
        FunctionParameters fp{ip.plot_points_num, sf::Color::Red};
        functions.emplace_back(Function(f_inter, boundaries, fp), "interpolation");
    }
    auto interpolation_id = functions.size() - 1;
    // добавляем ошибку
    std::function<Val(Arg)> f_error = [&](Arg x) { return f_original(x) - f_inter(x); };
    {
        FunctionParameters fp{ip.plot_points_num, sf::Color::Blue};
        functions.emplace_back(Function(f_error, boundaries, fp), "error");
    }
    auto error_id = functions.size() - 1;

    auto x_axis = Axis({-1000.f, 0}, {1000.f, 0});
    auto y_axis = Axis({0, -1000.f}, {0, 1000.f});

    sf::Clock deltaClock;

    sf::Vector2f prev_position;
    bool moving = false;

    float zoom = functions[original_id].func.fit(view, window);
    float def_zoom = zoom * 100;
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
            ImGui::SliderInt("Segments", &n, 1, ip.plot_points_num / 10);
            ImGui::ColorEdit4("##picker", (float *)&color);
            ImGui::End();
        }

        if (n != ip.interpolation_points_num)
        {
            f_inter =
                !ip.use_cubic ? create_linear(f_original, boundaries, n)
                              : create_cubic(f_original, boundaries, n, ip.left_derivative, ip.right_derivative);
            functions[interpolation_id].func.update_func(f_inter);

            f_error = [&](Arg x) { return f_original(x) - f_inter(x); };
            functions[error_id].func.update_func(f_error);

            ip.interpolation_points_num = n;
        }

        if (color != prev_color)
            functions[current_id].func.set_color(transform_color(color));

        window.clear();
        window.setView(view);
        window.draw(x_axis);
        window.draw(y_axis);
        for (auto &f : functions)
            window.draw(f.func);
        window.setView(window.getDefaultView());

        ImGui::SFML::Render(window);
        window.display();
    }
}