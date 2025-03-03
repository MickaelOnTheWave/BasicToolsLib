#ifndef POINT_H
#define POINT_H

template<typename T, int D = 2>
class Point
{
public:
    Point();

    void SetX(const T value);
    void SetY(const T value);
    void SetZ(const T value);

    T x() const;
    T y() const;
    T z() const;

    Point<T, D>& operator=(const Point<T, D>& param);
    Point<T, D> operator+(const Point<T, D>& param);
    Point<T, D> operator-(const Point<T, D>& param);

    void operator*=(const T val);
    void operator/=(const T val);

    void Normalize();
    Point<T, D> Normalized() const;

    T Norm() const;

private:
    T coordinates[D];
};

#endif // POINT_H
