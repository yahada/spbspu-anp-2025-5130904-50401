#include <iostream>

namespace malashenko {

  struct point_t {
    double x, y;
  };

  struct rectangle_t {
    double width, height;
    point_t pos;
  };

  struct Shape {
    virtual double getArea() const = 0;
    virtual rectangle_t getFrameRect() const = 0;
    virtual void move(point_t p) = 0;
    virtual void move(double dx, double dy) = 0;
    virtual void scale(double k) = 0;
    virtual ~Shape() = default;
  };

  struct Rectangle : Shape {
    Rectangle(double width, double height, point_t pos);
    double getArea() const override;
    rectangle_t getFrameRect() const override;
    void move(point_t p) override;
    void move(double dx, double dy) override;
    void scale(double k) override;

    private:
    double width_, height_;
    point_t pos_;
  };

  struct Polygon : Shape {
    Polygon(point_t * tops, size_t length);
    double getArea() const override;
    rectangle_t getFrameRect() const override;
    void move(point_t p) override;
    void move(double dx, double dy) override;
    void scale(double k) override;

    private:
    size_t length_;
    point_t * tops_;
    point_t pos_;
  };

  struct Triangle : Shape {
    Triangle(point_t * tops);
    double getArea() const override;
    rectangle_t getFrameRect() const override;
    void move(point_t p) override;
    void move(double dx, double dy) override;
    void scale(double k) override;
    private:
    point_t * tops_;
    point_t pos_;
  };

  rectangle_t generalGetFrameRect(const point_t * tops, size_t len)
  {
    rectangle_t resRect;
    double minX = tops[0].x, minY = tops[0].y;
    double maxX = tops[0].x, maxY = tops[0].y;
    for (size_t i = 1; i < len; ++i) {
      point_t p = tops[i];
      minX = std::min(minX, p.x);
      minY = std::min(minY, p.y);
      maxX = std::max(maxX, p.x);
      maxY = std::max(maxY, p.y);
    }
    resRect.width = maxX - minX;
    resRect.height = maxY - minY;
    resRect.pos = {minX + resRect.width / 2, minY + resRect.height / 2};
    return resRect;
  }

  double generalGetArea(const point_t * tops, size_t len)
  {
    double area = 0;
    for (size_t i = 0; i < len; ++i) {
      size_t j = (i + 1) % len;
      double tmp_area = (tops[i].x * tops[j].y) - (tops[j].x * tops[i].y);
      area += tmp_area;
    }
    return (area > 0 ? area : -area) / 2.0;
  }

  point_t * getCordsOfFrame(const rectangle_t & frame)
  {
    point_t * cords = new point_t[4];
    cords[0] = {frame.pos.x - (frame.width / 2), frame.pos.y - (frame.height / 2)};
    cords[1] = {frame.pos.x - (frame.width / 2), frame.pos.y + (frame.height / 2)};
    cords[2] = {frame.pos.x + (frame.width / 2), frame.pos.y - (frame.height / 2)};
    cords[3] = {frame.pos.x + (frame.width / 2), frame.pos.y + (frame.height / 2)};
    return cords;
  }

  Rectangle::Rectangle(double width, double height, point_t pos):
    width_(width),
    height_(height),
    pos_(pos)
  {}

  double Rectangle::getArea() const
  {
    return width_ * height_;
  }

  rectangle_t Rectangle::getFrameRect() const
  {
    return { width_, height_, pos_ };
  }

  void Rectangle::move(point_t p)
  {
    pos_ = p;
  }

  void Rectangle::move(double dx, double dy)
  {
    pos_.x += dx;
    pos_.y += dy;
  }

  void Rectangle::scale(double k)
  {
    width_ *= k;
    height_ *= k;
  }

  Polygon::Polygon(point_t * tops, size_t length):
    length_(length),
    tops_(length_ ? new point_t[length_] : nullptr)
  {
    if (!tops_) {
      throw std::bad_alloc();
    } else if (length_ < 3) {
      delete[] tops_;
      throw std::invalid_argument("polygon must have at least 3 points");
    }
    for (size_t i = 0; i < length_; ++i) {
      tops_[i] = tops[i];
    }

    double area2 = 0.0, cx = 0.0, cy = 0.0;

    for (size_t i = 0; i < length_; ++i) {
      size_t j = (i + 1) % length_;

      double cross = tops_[i].x * tops_[j].y - tops_[j].x * tops_[i].y;
      area2 += cross;

      cx += (tops_[i].x + tops_[j].x) * cross;
      cy += (tops_[i].y + tops_[j].y) * cross;
    }

    if (area2 == 0.0) {
      pos_.x = 0.0;
      pos_.y = 0.0;
      delete[] tops_;
      throw std::invalid_argument("polygon area must be positive");
    }

    double area = area2 / 2.0;

    cx /= (3.0 * area2);
    cy /= (3.0 * area2);
    pos_.x = cx;
    pos_.y = cy;
  }

  double  Polygon::getArea() const
  {
    return generalGetArea(tops_, length_);
  }

  rectangle_t Polygon::getFrameRect() const
  {
    return generalGetFrameRect(tops_, length_);
  }

  void Polygon::move(point_t p)
  {
    double dx = p.x - pos_.x;
    double dy = p.y - pos_.y;
    for (size_t i = 0; i < length_; ++i) {
      tops_[i].x += dx;
      tops_[i].y += dy;
    }
    pos_ = p;
  }

  void Polygon::move(double dx, double dy)
  {
    for (size_t i = 0; i < length_; ++i) {
      tops_[i].x += dx;
      tops_[i].y += dy;
    }
    pos_.x += dx;
    pos_.y += dy;
  }

  void Polygon::scale(double k)
  {
    for (size_t i = 0; i < length_; ++i) {
      tops_[i].x = pos_.x + (tops_[i].x - pos_.x) * k;
      tops_[i].y = pos_.y + (tops_[i].y - pos_.y) * k;
    }
  }

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

  void scaleByPoint(Shape ** figure, size_t len, point_t scalePoint, double k)
  {
    for (size_t i = 0; i < len; ++i) {
      point_t firstCord = figure[i]->getFrameRect().pos;
      figure[i]->move(scalePoint);
      point_t secondcord = figure[i]->getFrameRect().pos;
      figure[i]->scale(k);

      double xdist = (firstCord.x - secondcord.x) * k;
      double ydist = (firstCord.y - secondcord.y) * k;
      figure[i]->move(xdist, ydist);
    }
  }

  void extend(point_t ** oldTops, const point_t * topsToAdd, size_t & l, size_t k)
  {
    point_t * newTops = new point_t[l + k];
    size_t i = 0;
    for (;i < l; ++i) {
      newTops[i] = (*oldTops)[i];
    }
    for (;i < l + k; ++i) {
      newTops[i] = topsToAdd[i - l];
    }
    l += k;
    delete[] *oldTops;
    *oldTops = newTops;
  }


  void showInfo(const Shape * const * figures, size_t len)
  {
    std::cout << "INFO ABOUT FIGURES:\n";
    double fullSum = 0;
    size_t topsLen = 0;
    point_t * tops = new point_t[topsLen];
    for (size_t i = 0; i < len; ++i) {
      double figureArea = figures[i] -> getArea();
      rectangle_t figureFrameRect = figures[i] -> getFrameRect();
      point_t * frameRectCords = getCordsOfFrame(figureFrameRect);
      extend(&tops, frameRectCords, topsLen, 4);
      fullSum += figureArea;
      std::cout << "Площадь фигуры №" << i + 1 << ":" << figureArea << "\n";
      std::cout << "Кординаты ограничивающего прямиоугольника фигуры №" << i + 1 << ":\n";
      for (size_t i = 0; i < 4; ++i) {
        std::cout << "\t(" << frameRectCords[i].x << ", " << frameRectCords[i].y << ")\n";
      }
      delete[] frameRectCords;
    }
    std::cout << "\n";
    std::cout << "Суммарная площадь всех фигур равна: " << fullSum << "\n";
    rectangle_t figgureFrame = generalGetFrameRect(tops, topsLen);
    point_t * figuresFrameCords = getCordsOfFrame(figgureFrame);
    std::cout << "Ограничивающий прямиоугольник всех фигур имеет следующие координаты:\n";
    for (size_t i = 0; i < 4; ++i) {
      std::cout << "\t(" << figuresFrameCords[i].x << ", " << figuresFrameCords[i].y << ")\n";
    }
    delete[] figuresFrameCords;
    delete[] tops;
  }
}


int main()
{
  namespace mal = malashenko;
  double k = 0.0;
  mal::point_t scalePoint= {0.0, 0.0};
  std::cout << "ENTER SCALE COEFFICIENT AND SCALE POINT: ";
  if (!(std::cin >> k >> scalePoint.x >> scalePoint.y) || k < 0){
    std::cerr << "Input problems\n";
    return 1;
  }

  mal::Shape * figures[3] = {nullptr, nullptr, nullptr};
  mal::point_t triangleTops[3] = {{0, 0}, {3, 3}, {0, 3}};
  mal::point_t * tops = nullptr;
  try {
    figures[0] = new mal::Rectangle(4, 6, {10, 10});
    figures[1] = new mal::Triangle(triangleTops);
    mal::point_t * tops = new mal::point_t[3]{{0, 0}, {3, 3}, {1, 2}};
    figures[2] = new mal::Polygon(tops, 3);
  } catch (const std::exception & e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  mal::showInfo(figures, 3);
  mal::scaleByPoint(figures, 3, {10, 10}, 2);
  mal::showInfo(figures, 3);

  delete figures[0];
  delete figures[1];
  delete figures[2];

  delete[] tops;
}
