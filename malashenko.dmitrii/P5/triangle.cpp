#include "triangle.hpp"
#include "generalFuncs.hpp"
namespace malashenko {
  Triangle::Triangle(point_t * tops):
  tops_(new point_t[3])
  {
    if (!tops) {
      delete[] tops_;
      std::bad_alloc();
    }
    for (size_t i = 0; i < 3; ++i) {
      tops_[i] = tops[i];
    }
    pos_.x = (tops_[0].x + tops_[1].x + tops_[2].x) / 3;
    pos_.y = (tops_[0].y + tops_[1].y + tops_[2].y) / 3;
  }

  double Triangle::getArea() const
  {
    double res = generalGetArea(tops_, 3);
    return res;
  }

  rectangle_t Triangle::getFrameRect() const
  {
    rectangle_t res = generalGetFrameRect(tops_, 3);
    return res;
  }

  void Triangle::move(point_t p)
  {
    double dx = p.x - pos_.x;
    double dy = p.y - pos_.y;
    for (size_t i = 0; i < 3; ++i)
    {
      tops_[i].x += dx;
      tops_[i].y += dy;
    }

    pos_ = p;
  }

  void Triangle::move(double dx, double dy)
  {
    for (size_t i = 0; i < 3; ++i)
    {
      tops_[i].x += dx;
      tops_[i].y += dy;
    }

    pos_.x += dx;
    pos_.y += dy;
  }

  void Triangle::scale(double k)
  {
    for (size_t i = 0; i < 3; ++i)
    {
      tops_[i].x = pos_.x + (tops_[i].x - pos_.x) * k;
      tops_[i].y = pos_.y + (tops_[i].y - pos_.y) * k;
    }

  }
}
