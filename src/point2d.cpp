#include "point2d.h"

#include <math.h>

Point2D::Point2D()
    : x(0.0), y(0.0)
{
}

Point2D::Point2D(const float _x, const float _y)
    : x(_x), y(_y)
{
}

Point2D::Point2D(const Point2D &other)
  : x(other.x), y(other.y)
{
}

Point2D &Point2D::operator=(const Point2D &param)
{
   x = param.x;
   y = param.y;
   return *this;
}

Point2D Point2D::operator+(const Point2D &param)
{
   return Point2D(x+param.x, y+param.y);
}

Point2D Point2D::operator-(const Point2D &param)
{
   return Point2D(x-param.x, y-param.y);
}

void Point2D::operator*=(const float val)
{
   x *= val;
   y *= val;
}

void Point2D::operator/=(const float val)
{
   x /= val;
   y /= val;
}

void Point2D::Normalize()
{
   const float norm = Norm();
   x = (norm != 0.0f) ? x / norm : 0.0f;
   y = (norm != 0.0f) ? y / norm : 0.0f;
}

Point2D Point2D::Normalized() const
{
   Point2D normalized(*this);
   normalized.Normalize();
   return normalized;
}

float Point2D::Norm() const
{
   return sqrtf(x*x + y*y);
}
