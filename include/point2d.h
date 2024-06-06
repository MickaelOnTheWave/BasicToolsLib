#ifndef POINT2D_H
#define POINT2D_H

// TODO : make a template point class, with dimension and type
// as template parameters. However there should be some testing to
// make sure results are OK, and maybe some performance too.
class Point2D
{
public:
    Point2D();
    Point2D(const float _x, const float _y);

    Point2D& operator=(const Point2D& param);
    Point2D operator+(const Point2D& param);
    Point2D operator-(const Point2D& param);

    void operator*=(const float val);
    void operator/=(const float val);

    void Normalize();
    Point2D Normalized() const;

    float Norm() const;


    float x;
    float y;

};

#endif // POINT2D_H
