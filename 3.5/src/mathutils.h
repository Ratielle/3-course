#pragma once
#include <array>
#include <cmath>

template <class T, size_t N>
struct Vector
{
    std::array<T, N> v;

    Vector &operator+=(const Vector<T> &other)
    {
        for (int i = 0; i < N; ++i)
            v[i] += other.v[i];
        return *this;
    }

    Vector &operator-=(const Vector<T> &other)
    {
        for (int i = 0; i < N; ++i)
            v[i] -= other.v[i];
        return *this;
    }

    Vector operator-() const
    {
        Vector out;
        for (int i = 0; i < N; ++i)
            out.v[i] = -v[i];
        return out;
    }

    T norm() const
    {
        T out = T(0);
        for (int i = 0; i < N; ++i)
            out += v[i] * v[i];
        return std::sqrt(out);
    }

    Vector &normalize()
    {
        scale(T(1) / norm());
    }

    Vector &scale(const Vector<T> &other)
    {
        for (int i = 0; i < N; ++i)
            v[i] *= other.v[i];
        return *this;
    }

    Vector &scale(const T &factor)
    {
        for (int i = 0; i < N; ++i)
            v[i] *= factor;
        return *this;
    }

    static T dot(const Vector &a, const Vector &b)
    {
        T out = T(0);
        for (int i = 0; i < N; ++i)
            out += a.v[i] * b.v[i];
        return out;
    }

    static Vector scale(const Vector &a, const Vector &b)
    {
        Vector out = a;
        return out.scale(b);
    }

    static Vector scale(const Vector &a, const T &factor)
    {
        Vector out = a;
        return out.scale(factor);
    }

    static Vector normalize(const Vector &a)
    {
        Vector out = a;
        return out.normalize();
    }
};

template <class T, size_t N>
Vector<T, N> operator+(const Vector<T, N> &a, const Vector<T, N> &b)
{
    Vector<T, N> out = a;
    return out += b;
}

template <class T, size_t N>
Vector<T, N> operator-(const Vector<T, N> &a, const Vector<T, N> &b)
{
    Vector<T, N> out = a;
    return out -= b;
}
