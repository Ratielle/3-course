#pragma once
#include "types.h"

#include <cmath>

namespace Constants
{
    const float pi = std::atan(1.f) * 4;
    const float e = std::exp(1.f);
} // namespace Constants

inline Val InputFunc(Arg x)
{
    return std::sin(x * x * x) + std::cos(3 * x) + 2.;
}

struct InputParameters
{
    InputParameters(int argc, char *argv[]);
    int plot_points_num;
    int interpolation_points_num;
    Arg left_boundary;
    Arg right_boundary;
    Val left_derivative;
    Val right_derivative;
    bool use_cubic;
};