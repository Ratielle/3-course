#include "input.h"

#include <argh/argh.h>

InputParameters::InputParameters(int argc, char *argv[])
{
    argh::parser cmdl(argc, argv);
    cmdl({"-NP", "--num_plot_points"}, 101) >> plot_points_num;

    cmdl({"-NI", "--num_inter_points"}, 11) >> interpolation_points_num;

    auto parse_number = [](const std::string &s) {
        if (s == "pi")
            return Constants::pi;
        if (s == "e")
            return Constants::e;
        throw std::runtime_error("Unknown boundary");
    };

    if (!(cmdl({"-LB", "--left_boundary"}, -1) >> left_boundary))
    {
        auto v = cmdl({"-LB", "--left_boundary"}, -1).str();
        if (v.front() == '-')
            left_boundary = -parse_number(v.substr(1));
        else
            left_boundary = parse_number(v);
    }

    if (!(cmdl({"-RB", "--right_boundary"}, 1) >> right_boundary))
    {
        auto v = cmdl({"-RB", "--right_boundary"}, 1).str();
        if (v.front() == '-')
            right_boundary = -parse_number(v.substr(1));
        else
            right_boundary = parse_number(v);
    }

    if (!(cmdl({"-LD", "--left_derivative"}, 0) >> left_derivative))
    {
        auto v = cmdl({"-LD", "--left_derivative"}, 0).str();
        if (v.front() == '-')
            left_derivative = -parse_number(v.substr(1));
        else
            left_derivative = parse_number(v);
    }

    if (!(cmdl({"-RD", "--right_derivative"}, 0) >> right_derivative))
    {
        auto v = cmdl({"-RD", "--right_derivative"}, 0).str();
        if (v.front() == '-')
            right_derivative = -parse_number(v.substr(1));
        else
            right_derivative = parse_number(v);
    }

    use_cubic = cmdl[{"-C", "--cubic"}];
}