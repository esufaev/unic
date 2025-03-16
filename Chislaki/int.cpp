#include <functional>
#include <iostream>
#include <cmath>
#include <array>
#include <cassert>

namespace ios
{
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

    enum approximation_type
    {
        left,
        middle,
        right,
        trap,
        gauss2,
        simpson
    };

    template <approximation_type mode>
    double approximation(std::function<double(double)> func, double a, double b, int n = 1000)
    {
        assert(b - a >= 0);
        assert(n >= 1);
        double h = (b - a) / n, sum = 0.0;

        if constexpr (mode == approximation_type::trap)
        {
            for (int i = 0; i < n; ++i)
            {
                sum += (func(a + i * h) + func(a + (i + 1) * h)) / 2.0;
            }
            sum *= h;
        }
        else if constexpr (mode == approximation_type::simpson)
        {
            double subinterval_h = (b - a) / n;
            sum = 0.0;
            for (int i = 0; i < n; ++i)
            {
                double x0 = a + i * subinterval_h;
                double x1 = a + (i + 1) * subinterval_h;
                sum += (x1 - x0) / 6.0 * (func(x0) + 4.0 * func((x0 + x1) / 2.0) + func(x1));
            }
        }
        else if constexpr (mode == approximation_type::gauss2)
        {
            const double sqrt3_inv = 1.0 / std::sqrt(3.0);
            double subinterval_h = (b - a) / n;
            sum = 0.0;
            for (int i = 0; i < n; ++i)
            {
                double x0 = a + i * subinterval_h;
                double x1 = a + (i + 1) * subinterval_h;
                double scale = (x1 - x0) / 2.0;
                double shift = (x0 + x1) / 2.0;
                double t1 = scale * (-sqrt3_inv) + shift;
                double t2 = scale * sqrt3_inv + shift;
                sum += scale * (func(t1) + func(t2));
            }
        }
        else
        {
            for (int i = 0; i < n; ++i)
            {
                sum += func(a + (i + static_cast<double>(mode) * 0.5) * h);
            }
            sum *= h;
        }
        return sum;
    }
}

template <ios::approximation_type mode>
int find_n(std::function<double(double)> func, double a, double b, double tolerance)
{
    int n = 1;
    while (true)
    {
        double I_n = ios::approximation<mode>(func, a, b, n);
        double I_2n = ios::approximation<mode>(func, a, b, 2 * n);
        if (std::abs(I_n - I_2n) <= tolerance)
        {
            return n;
        }
        n *= 2;
    }
}

template <ios::approximation_type mode>
void print_table(std::function<double(double)> func,
                 double a, double b, double tolerance = 1e-6)
{
    printf("%10s %15s %15s %15s %10s\n", "x", "erf(x)", "I_n", "|I_n - I_2n|", "n");
    for (double x = a; x <= b; x += 0.2)
    {
        int n = find_n<mode>(func, a, x, tolerance);
        double diff = std::abs(ios::approximation<mode>(func, a, x, n) - ios::erf(x));
        printf("%10.1f %15.8f %15.8f %15.8f %10d\n", x, ios::erf(x), ios::approximation<mode>(func, a, x, n), diff, n);
    }
}

int main()
{
    double a = 0.0;
    double b = 2.0;

    auto func = [](double t)
    { return (2.0 / std::sqrt(M_PI)) * std::exp(-t * t); };

    print_table<ios::approximation_type::left>(func, a, b);
    print_table<ios::approximation_type::right>(func, a, b);
    print_table<ios::approximation_type::middle>(func, a, b);
    print_table<ios::approximation_type::trap>(func, a, b);
    print_table<ios::approximation_type::simpson>(func, a, b);
    print_table<ios::approximation_type::gauss2>(func, a, b);

    return 0;
}