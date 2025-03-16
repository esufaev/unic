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
    // Generate full set of points for reference
    std::vector<double> full_x;
    for (int i = 0; i <= 21; i += 1) // Finer grid for more accurate error calculation
    {
        full_x.push_back(i / 10.0);
    }
    std::vector<double> full_y = erf(full_x);

    // Vectors to store number of nodes and corresponding maximum errors
    std::vector<double> num_nodes;
    std::vector<double> max_errors;

    // Test different numbers of interpolation nodes (from 3 to 11)
    for (int nodes = 3; nodes <= 11; nodes += 2)
    {
        // Generate interpolation points
        std::vector<double> interp_x;
        double step = 2.1 / (nodes - 1); // Evenly spaced points from 0 to 2.1
        for (int i = 0; i < nodes; i++)
        {
            interp_x.push_back(i * step);
        }
        std::vector<double> interp_y = erf(interp_x);

        // Calculate Lagrange interpolation for all points
        std::vector<double> lagrange_y = lagrange(interp_x, interp_y, full_x);

        // Calculate maximum error
        double max_error = 0.0;
        for (size_t i = 0; i < full_x.size(); i++)
        {
            double error = std::abs(full_y[i] - lagrange_y[i]);
            max_error = std::max(max_error, error);
        }

        num_nodes.push_back(nodes);
        max_errors.push_back(max_error);

        std::printf("Nodes: %d, Maximum Error: %.15f\n", nodes, max_error);
    }

    // Create graph of nodes vs error
    bench::graph_tt gr_error;
    for (size_t i = 0; i < num_nodes.size(); i++)
    {
        gr_error.add_point(num_nodes[i], max_errors[i]);
    }
    gr_error.set_title("Error vs Number of Nodes");

    gr_error.plot();

    return 0;
}