#include <iostream>
#include <vector>
#include <functional>
#include <math.h>
#include <utility>

namespace ias
{
    class coef
    {
    public:
        std::vector<double> m_c;
        std::vector<double> m_a;
        std::vector<double> m_b;
        int s;

        coef(const std::vector<double> &c,
             const std::vector<std::vector<double>> &a,
             const std::vector<double> &b) : m_c(c), m_b(b), s(c.size())
        {
            m_a.reserve((s * (s - 1)) / 2);
            for (int i = 1; i < s; i++)
            {
                for (int j = 0; j < i; j++)
                {
                    m_a.push_back(a[i][j]);
                }
            }
        }

        double get_a(int i, int j) const
        {
            if (j >= i) return 0.0;
            int index = (i * (i - 1)) / 2 + j;
            return m_a[index];
        }
    };

    [[nodiscard]] std::vector<std::pair<double, double>> runge_kutta(
        std::function<double(double, double)> func,
        double x0, double y0,
        double x_end,
        double h,
        const coef &table)
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
                    arg_y += h * table.get_a(i, m) * k[m]; 
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
    ias::coef table(c, a, b);

    std::vector<double> vec_res;

    for (double h = 0.00001; h < 2.0; h += 0.00004)
    {
        double max_res = 0.0;
        auto solution = ias::runge_kutta(func, x0, y0, x_end, h, table);
        for (auto && point : solution)
            max_res = std::max(max_res, std::abs(point.second - std::exp(point.first)) / (h * h * h * h));
        vec_res.push_back(max_res);
    }
    for (auto && point : vec_res)
        printf("%lf\n", point);


    return 0;
}