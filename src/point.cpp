#include "point.hpp"

#include <math.h>

template <typename T, int D>
Point<T, D>::Point()
{
    for (int i=0; i<D; ++i)
        coordinates[i] = 0;
}

template<typename T, int D>
Point<T, D>::Point(const T x, const T y)
{
   coordinates[0] = x;
   coordinates[1] = y;
}

template<typename T, int D>
Point<T, D>::Point(const std::vector<T> &values)
{
   for (int i=0; i<D; ++i)
      coordinates[i] = values[i];
}

template<typename T, int D>
void Point<T, D>::SetX(const T value)
{
   coordinates[0] = value;
}

template<typename T, int D>
void Point<T, D>::SetY(const T value)
{
   coordinates[1] = value;
}

template <typename T, int D>
T Point<T, D>::x() const
{
    return coordinates[0];
}

template <typename T, int D>
T Point<T, D>::y() const
{
    return coordinates[1];
}

template <typename T, int D>
Point<T, D> &Point<T, D>::operator=(const Point<T, D> &param)
{
    for (int i=0; i<D; ++i)
        coordinates[i] = param.coordinates[i];

    return *this;
}

template <typename T, int D>
Point<T, D> Point<T, D>::operator+(const Point<T, D> &param)
{
    Point point;
    for (int i=0; i<D; ++i)
        point.coordinates[i] = coordinates[i] + param.coordinates[i];

    return point;
}

template <typename T, int D>
Point<T, D> Point<T, D>::operator-(const Point<T, D> &param)
{
    Point point;
    for (int i=0; i<D; ++i)
        point.coordinates[i] = coordinates[i] - param.coordinates[i];

    return point;
}

template <typename T, int D>
void Point<T, D>::operator*=(const T val)
{
    for (int i=0; i<D; ++i)
        coordinates[i] *= val;
}

template <typename T, int D>
void Point<T, D>::operator/=(const T val)
{
    for (int i=0; i<D; ++i)
        coordinates[i] /= val;
}

template <typename T, int D>
void Point<T, D>::Normalize()
{
    T norm = Norm();

    for (int i=0; i<D; ++i)
        coordinates[i] = (norm != 0) ? coordinates[i] / norm : 0;
}

template <typename T, int D>
Point<T, D> Point<T, D>::Normalized() const
{
    Point<T, D> point(*this);
    point.Normalize();
    return point;
}

template <typename T, int D>
T Point<T, D>::Norm() const
{
    T sum = 0;
    for (int i=0; i<D; ++i)
        sum += coordinates[i] * coordinates[i];

    return static_cast<T>(sqrt(sum));
}


template class Point<int, 2>;
template class Point<float, 2>;
