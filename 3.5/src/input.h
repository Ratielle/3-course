#pragma once

#include <cmath>

namespace Constants
{
    const double pi = std::atan(1.f) * 4;
    const double e = std::exp(1.f);
} // namespace Constants

inline double InputFunc3D(double x, double y)
{
    return 1. / (x * x + y * y + 1);
}

struct InputParameters
{
    InputParameters(int argc, char *argv[]);
    int plot_points_num;
    int interpolation_points_num;
    double left_boundary;
    double right_boundary;
    double left_derivative;
    double right_derivative;
    bool use_cubic;
};