#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "bench.h"

double erf(double x)
{
    const double eps = 1e-6;
    double a0 = (2 * x) / std::sqrt(M_PI);
    double sum_series = a0;
    double an = a0;
    int n = 0;

    while (std::abs(an) >= eps)
    {
        double qn = -((x * x) * (2 * n + 1)) / ((2 * n + 3) * (n + 1));
        an *= qn;
        sum_series += an;
        ++n;
    }

    return sum_series;
}

std::vector<double> erf(const std::vector<double> &listx)
{
    std::vector<double> listy;
    for (double x : listx)
    {
        listy.push_back(erf(x));
    }
    return listy;
}

double lagrange(const std::vector<double> &x_list, const std::vector<double> &y_list, double x)
{
    double result = 0.0;

    for (size_t i = 0; i < x_list.size(); ++i)
    {
        double Li = 1.0;
        for (size_t j = 0; j < x_list.size(); ++j)
        {
            if (i != j)
            {
                Li *= (x - x_list[j]) / (x_list[i] - x_list[j]);
            }
        }
        result += y_list[i] * Li;
    }

    return result;
}

std::vector<double> lagrange(const std::vector<double> &x_list, const std::vector<double> &y_list, const std::vector<double> &x_points)
{
    std::vector<double> lagrange_results;
    for (double x : x_points)
    {
        lagrange_results.push_back(lagrange(x_list, y_list, x));
    }
    return lagrange_results;
}

int main()
{
    std::vector<double> list_x_1;
    for (int i = 0; i <= 21; i += 2)
    {
        list_x_1.push_back(i / 10.0);
    }

    std::vector<double> list_y_1 = erf(list_x_1);

    bench::graph_tt gr_erf;
    for (size_t i = 0; i < list_x_1.size(); ++i)
    {
        gr_erf.add_point(list_x_1[i], list_y_1[i]);
        std::printf("x = %.1f, erf(x) = %.15f\n", list_x_1[i], list_y_1[i]);
    }
    gr_erf.set_title("erf(x)");
    gr_erf.plot();

    std::vector<double> lagrange_values = lagrange(list_x_1, list_y_1, list_x_1);

    std::printf("-------------------------------------------------------\n");
    std::printf("x       erf(x)            Lagrange(x)       Difference\n");
    std::printf("-------------------------------------------------------\n");
    for (size_t i = 0; i < list_x_1.size(); ++i)
    {
        if (i % 2 == 0)
            double diff = std::abs(list_y_1[i] - lagrange_values[i]);
        std::printf("%.1f   %.15f   %.15f   %.15f\n", list_x_1[i], list_y_1[i], lagrange_values[i], diff);
    }

    return 0;
}
