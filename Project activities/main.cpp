#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

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
            if (j >= i)
                return 0.0;
            int index = (i * (i - 1)) / 2 + j;
            return m_a[index];
        }
    };

    std::vector<std::pair<double, std::vector<double>>> runge_kutta(
        std::function<std::vector<double>(double, const std::vector<double> &)> func,
        double x0, const std::vector<double> &y0,
        double x_end,
        double h,
        const coef &table)
    {
        std::vector<std::pair<double, std::vector<double>>> solution;
        std::pair<double, std::vector<double>> current = {x0, y0};
        solution.push_back(current);

        int steps = static_cast<int>((x_end - x0) / h);

        for (int n = 0; n < steps; n++)
        {
            int s = table.s;
            int dim = y0.size();
            std::vector<std::vector<double>> k(s, std::vector<double>(dim));

            for (int i = 0; i < s; i++)
            {
                double arg_x = current.first + table.m_c[i] * h;
                std::vector<double> arg_y = current.second;

                for (int m = 0; m < i; m++)
                {
                    for (int d = 0; d < dim; d++)
                    {
                        arg_y[d] += h * table.get_a(i, m) * k[m][d];
                    }
                }

                k[i] = func(arg_x, arg_y);
            }

            std::vector<double> y_next = current.second;
            for (int d = 0; d < dim; d++)
            {
                for (int i = 0; i < s; i++)
                {
                    y_next[d] += h * table.m_b[i] * k[i][d];
                }
            }

            double x_next = current.first + h;
            current = {x_next, y_next};
            solution.push_back(current);
        }

        return solution;
    }
}

std::vector<double> func(double x, const std::vector<double> &y)
{
    double y1 = y[0];
    double y2 = y[1];
    return {exp(2 * x) * y2, -exp(-2 * x) * y1};
}

std::vector<double> exact_solution(double x)
{
    return {exp(x), exp(-x)};
}

// int main()
// {
//     std::vector<double> c = {0.0, 0.5, 1.0};
//     std::vector<std::vector<double>> a = {{}, {0.5}, {-1.0, 2.0}};
//     std::vector<double> b = {1.0 / 6.0, 2.0 / 3.0, 1.0 / 6.0};
//     ias::coef table(c, a, b);

//     std::vector<double> y0 = {1.0, 1.0};
//     double x0 = 0.0;
//     double x_end = 4.0;

//     for (int n = 1; n <= 300; n++)
//     {
//         double h = (x_end - x0) / n;
//         auto solution = ias::runge_kutta(func, x0, y0, x_end, h, table);

//         double max_error = 0.0;
//         for (const auto &point : solution)
//         {
//             double x = point.first;
//             std::vector<double> y_exact = exact_solution(x);
//             double error_y1 = std::abs(point.second[0] - y_exact[0]);
//             double error_y2 = std::abs(point.second[1] - y_exact[1]);
//             double local_error = std::max(error_y1, error_y2);
//             max_error = std::max(max_error, local_error);
//         }

//         double ratio = max_error / (h * h * h);
//         std::cout << n << "\t" << ratio << std::endl;
//     }

//     return 0;
// }

int main()
{
    double C = 0.2;
    double rho = 1.29;
    double S = 0.25;
    double g = 9.81;
    double v0 = 50.0;
    double mu = 3.75;      // 15 kg / 4 s
    double T_thrust = 5.0; // N
    double m0 = 30.0;      // kg
    double t_burn = 4.0;   // s

    std::vector<double> c = {0.0, 0.5, 0.5, 1.0};
    std::vector<std::vector<double>> a = {
        {0.0, 0.0, 0.0, 0.0},
        {0.5, 0.0, 0.0, 0.0},
        {0.0, 0.5, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0}};
    std::vector<double> b = {1.0 / 6.0, 1.0 / 3.0, 1.0 / 3.0, 1.0 / 6.0};
    ias::coef table(c, a, b);

    double max_range = 0.0;
    double best_theta0 = 0.0;

    for (int theta0_deg = 30; theta0_deg <= 80; theta0_deg++)
    {
        double theta0 = theta0_deg * M_PI / 180.0;

        auto func = [theta0, C, rho, S, g, mu, T_thrust, m0, t_burn](double t, const std::vector<double> &u)
        {
            double v = std::sqrt(u[1] * u[1] + u[3] * u[3]);
            double m, m_dot, T;

            if (t <= t_burn)
            {
                m = m0 - mu * t;
                m_dot = -mu;
                T = T_thrust;
            }
            else
            {
                m = m0 - mu * t_burn;
                m_dot = 0.0;
                T = 0.0;
            }

            double drag_term = (C * rho * S * v * v) / 2.0;
            double common = (T - drag_term) / m;
            double u2p = common * std::cos(theta0) - (m_dot * u[1]) / m;
            double u4p = common * std::sin(theta0) - (m_dot * u[3]) / m - g;

            return std::vector<double>{u[1], u2p, u[3], u4p};
        };

        std::vector<double> y0 = {0.0, v0 * std::cos(theta0), 0.0, v0 * std::sin(theta0)};
        double h = 0.01;
        double x0 = 0.0;
        double x_end = 100.0; 

        auto solution = ias::runge_kutta(func, x0, y0, x_end, h, table);

        size_t n = 0;
        for (n = 1; n < solution.size(); n++)
        {
            if (solution[n].second[2] <= 0.0)
                break;
        }

        if (n < solution.size() && n > 1)
        {
            double range = solution[n - 1].second[0];
            if (range > max_range)
            {
                max_range = range;
                best_theta0 = theta0_deg;
            }
        }
    }

    std::cout << "Optimal theta0: " << best_theta0 << " degrees\n";
    std::cout << "Maximum range: " << max_range << " meters\n";

    double theta0 = best_theta0 * M_PI / 180.0;
    auto func = [theta0, C, rho, S, g, mu, T_thrust, m0, t_burn](double t, const std::vector<double> &u)
    {
        double v = std::sqrt(u[1] * u[1] + u[3] * u[3]);
        double m, m_dot, T;

        if (t <= t_burn)
        {
            m = m0 - mu * t;
            m_dot = -mu;
            T = T_thrust;
        }
        else
        {
            m = m0 - mu * t_burn;
            m_dot = 0.0;
            T = 0.0;
        }

        double drag_term = (C * rho * S * v * v) / 2.0;
        double common = (T - drag_term) / m;
        double u2p = common * std::cos(theta0) - (m_dot * u[1]) / m;
        double u4p = common * std::sin(theta0) - (m_dot * u[3]) / m - g;

        return std::vector<double>{u[1], u2p, u[3], u4p};
    };

    std::vector<double> y0 = {0.0, v0 * std::cos(theta0), 0.0, v0 * std::sin(theta0)};
    double h = 0.01;
    double x0 = 0.0;
    double x_end = 100.0;
    auto solution = ias::runge_kutta(func, x0, y0, x_end, h, table);

    std::cout << "\nTrajectory (t, x, y):\n";
    size_t n = 0;
    for (n = 1; n < solution.size(); n++)
    {
        if (solution[n].second[2] <= 0.0)
            break;
    }

    for (size_t i = 0; i < n; i++)
    {
        double t = solution[i].first;
        double x = solution[i].second[0];
        double y = solution[i].second[2];
        // std::cout << "t = " << t << " s, x = " << x << " m, y = " << y << " m\n";
        std::cout << x << "\t" << y << std::endl;
    }

    if (n < solution.size())
    {
        double t_prev = solution[n - 1].first;
        double x_prev = solution[n - 1].second[0];
        double y_prev = solution[n - 1].second[2];
        double t_next = solution[n].first;
        double x_next = solution[n].second[0];
        double y_next = solution[n].second[2];

        double t_land = t_prev + (0.0 - y_prev) * (t_next - t_prev) / (y_next - y_prev);
        double x_land = x_prev + (x_next - x_prev) * (t_land - t_prev) / (t_next - t_prev);
        std::cout << "Landing: t = " << t_land << " s, x = " << x_land << " m, y = 0 m\n";
    }

    return 0;
}