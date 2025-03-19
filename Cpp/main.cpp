#include <iostream>
#include <vector>
#include <functional>
#include <math.h>
#include <utility> 

namespace ias
{
    class koef
    {
    public:
        std::vector<double> m_c;
        std::vector<std::vector<double>> m_a;
        std::vector<double> m_b;
        int s;

        koef(const std::vector<double> &c,
             const std::vector<std::vector<double>> &a,
             const std::vector<double> &b) : m_c(c), m_a(a), m_b(b), s(m_c.size()) {}
    };

    [[nodiscard]] std::vector<std::pair<double, double>> runge_kutta(
        std::function<double(double, double)> func,
        double x0, double y0,
        double x_end,
        double h,
        const koef &table)
    {
        std::vector<std::pair<double, double>> solution;
        std::pair<double, double> current = {x0, y0};
        solution.push_back(current);

        int steps = static_cast<int>((x_end - x0) / h);

        for (int n = 0; n < steps; n++)
        {
            std::vector<double> k(table.s);

            for (int i = 0; i < table.s; i++)
            {
                double arg_x = current.first + table.m_c[i] * h;
                double arg_y = current.second;

                for (int m = 0; m < i; m++)
                {
                    arg_y += h * table.m_a[i][m] * k[m];
                }

                k[i] = func(arg_x, arg_y);
            }

            double y_next = current.second;
            for (int i = 0; i < table.s; i++)
            {
                y_next += h * table.m_b[i] * k[i];
            }

            double x_next = current.first + h;
            current = {x_next, y_next};
            solution.push_back(current);
        }

        return solution;
    }
}

int main()
{
    auto func = [](double x, double y)
    { return std::exp(x); };

    double x0 = 0;
    double y0 = 1;
    double x_end = 2;
    double h = 0.05;

    std::vector<double> c = {0.0, 0.5, 0.5, 1.0};
    std::vector<std::vector<double>> a = {
        {0, 0, 0, 0},
        {0.5, 0, 0, 0},
        {0, 0.5, 0, 0},
        {0, 0, 1, 0}};
    std::vector<double> b = {1.0 / 6, 1.0 / 3, 1.0 / 3, 1.0 / 6};
    ias::koef table(c, a, b);

    auto solution = ias::runge_kutta(func, x0, y0, x_end, h, table);
    std::cout << "x\t\ty" << std::endl;
    for (const auto &point : solution)
    {
        printf("X: %10f, Y: %10f\n", point.first, point.second);
    }

    return 0;
}