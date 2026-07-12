#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>
#include <string>

struct quad_res
{
    double x1, x2;
    int count;
};

class math
{
public:
    static quad_res quadratic(double a, double b, double c)
    {
        double d = b * b - 4 * a * c;
        if (d < 0)
        {
            return {0, 0, 0};
        }
        if (d == 0)
        {
            return {-b / (2 * a), -b / (2 * a), 1};
        }
        return {(-b + std::sqrt(d)) / (2 * a), (-b - std::sqrt(d)) / (2 * a), 2};
    }

    static double arith_prog(double a1, double d, int n)
    {
        return a1 + (n - 1) * d;
    }

    static double arith_prog_sum(double a1, double d, int n)
    {
        return (2 * a1 + (n - 1) * d)/2 * n;
    }

    static double geom_prog(double b1, double q, int n)
    {
        return b1 * std::pow(q, n - 1);
    }

    static double geom_prog_sum(double b1, double q, int n)
    {
        if (q == 1)
        {
            return b1 * n;
        }
        return (b1 * (std::pow(q, n) - 1))/(q - 1);
    }

    static bool is_triangle_exists(double a, double b, double c)
    {
        return (a + b > c) && (a + c > b) && (b + c > a);
    }

    static double triangle_area(double a, double b, double c)
    {
        double p = (a + b + c) / 2.0;
        return std::sqrt(p * (p - a) * (p - b) * (p - c));
    }

    static std::string triangle_type(double a, double b, double c)
    {
        if (a == b && b == c)
        {
            return "равносторонний";
        }
        if (a == b || b == c || a == c)
        {
            return "равнобедренный";
        }
        return "разносторонний";
    }
    
    static double perimeter(double a, double b, double c)
    {
        return a + b + c;
    }
};

#endif
