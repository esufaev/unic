#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <array>
#include <set>
#include <math.h>

#include "graph.h"

double erf(double x)
{
    const double eps = 1e-6;
    double a0 = (2 * x) / std::sqrt(M_PI);
    double sum_series = a0;
    double an = a0;
    int n = 0;

    while (std::abs(an) > eps)
    {
        double qn = -((x * x) * (2 * n + 1)) / ((2 * n + 3) * (n + 1));
        an *= qn;
        sum_series += an;
        ++n;
    }

    return sum_series;
}

std::vector<double> erf(const std::vector<double>& listx)
{
    std::vector<double> result;
    for (double x : listx)
    {
        result.push_back(erf(x));
    }
    return result;
}

double lagrange(const std::vector<double>& x_list, const std::vector<double>& y_list, double x)
{
    double result = 0.0;

    for (size_t i = 0; i < x_list.size(); i += 2)
    {
        double Li = 1.0;
        for (size_t j = 0; j < x_list.size(); j += 2)
        {
            if (i != j)
            {
                Li *= (x - x_list[j]) / (x_list[i] - x_list[j]);
            }
        }
        result += Li * y_list[i];
    }
    return result;
}

std::vector<double> lagrange(const std::vector<double>& x_list, const std::vector<double>& y_list, const std::vector<double>& x_points)
{
    std::vector<double> lagrange_results;
    for (double x : x_points)
    {
        lagrange_results.push_back(lagrange(x_list, y_list, x));
    }
    return lagrange_results;
}

std::vector<double> chebyshev_nodes(double a, double b, int n)
{
    std::vector<double> nodes(n);
    for (int i = 0; i < n; i++)
    {
        nodes[i] = 0.5 * (a + b) + 0.5 * (b - a) * cos((2 * i + 1) * M_PI / (2 * n + 2));
    }
    return nodes;
}
void exp(int n)
{
    std::vector<double> list_x_1;
    for (int i = 0; i < n; i++)
    {
        list_x_1.push_back(2.0 * i / (n - 1));
    }

    std::vector<double> list_y_1 = erf(list_x_1);

    bench::graph_tt gr_erf;
    for (size_t i = 0; i < list_x_1.size(); ++i)
    {
        gr_erf.add_point(list_x_1[i], list_y_1[i]);
        printf("x = %.1f, y = %.6f\n", list_x_1[i], list_y_1[i]);
    }

    gr_erf.set_title("erf(x)");
    // gr_erf.plot();

    std::vector<double> lagrange_values = lagrange(chebyshev_nodes(0, 2, n), erf(chebyshev_nodes(0, 2, n)), list_x_1);

    std::printf("-------------------------------------------------------\n");
    std::printf("x       erf(x)            Lagrange(x)       Difference\n");
    std::printf("-------------------------------------------------------\n");

    for (size_t i = 0; i < list_x_1.size(); i++)
    {

        double diff = std::abs(list_y_1[i] - lagrange_values[i]);
    }

    bench::graph_tt gr_error;
    gr_error.add_point(0, 0.000036283433603);
    std::printf("%.1f   %.15f   %.15f   %.15f\n", list_x_1[0], list_y_1[0], lagrange_values[0], 0.000036283433603);

    for (size_t i = 1; i < list_x_1.size(); i++)
    {
        double diff = std::abs(list_y_1[i] - lagrange_values[i]);
        std::printf("%.1f   %.15f   %.15f   %.15f\n", list_x_1[i], list_y_1[i], lagrange_values[i], diff);
        gr_error.add_point(list_x_1[i], diff);
    }

    gr_error.set_title("Error");
    gr_error.plot();
}

int main()
{
    exp(11);
    // exp(70);

    return 0;
}