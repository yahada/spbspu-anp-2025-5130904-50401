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
    Polygon(point_t * tops, size_t length, point_t pos);
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
    Triangle(point_t a, point_t b, point_t c, point_t pos);
    double getArea() const override;
    rectangle_t getFrameRect() const override;
    void move(point_t p) override;
    void move(double dx, double dy) override;
    void scale(double k) override;

    private:
    point_t a_, b_, c_, pos_;
  };


  Rectangle::Rectangle(double width, double height, point_t pos):
    width_(width),
    height_(height),
    pos_(pos)
  {

  }

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

  Polygon::Polygon(point_t * tops, size_t length, point_t pos):
    length_(length),
    tops_(length_ ? new point_t[length_] : nullptr),
    pos_(pos)
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
  }

  double  Polygon::getArea() const 
  {
    double area = 0;

    for (size_t i = 0; i < length_; ++i) {
      size_t j = (i + 1) % length_;
      double tmp_area = (tops_[i].x * tops_[j].y) - (tops_[j].x * tops_[i].y);
      area += tmp_area;
    }
    
    return (area > 0 ? area : -area) / 2.0;
  }

  rectangle_t Polygon::getFrameRect() const
  {
    return generalGetFrameRect(tops_, length_);
  }
  
  void Polygon::move(point_t p)
  {
    pos_ = p;
  }

  void Polygon::move(double dx, double dy)
  {
    pos_.x += dx;
    pos_.y += dy;
  }

  void Polygon::scale(double k)
  {
    for (size_t i = 0; i < length_; ++i) {
      tops_[i].x *= k;
      tops_[i].y *= k;
    }
  }

  Triangle::Triangle(point_t a, point_t b, point_t c, point_t pos):
    a_(a),
    b_(b),
    c_(c),
    pos_(pos)
  {

  }

  double Triangle::getArea() const 
  {
    double area = (a_.x * (b_.y - c_.y)) + (b_.x * (c_.y - a_.y)) + (c_.x * (a_.y - b_.y));
    return (area > 0 ? area : -area) / 2.0;
  }

  rectangle_t Triangle::getFrameRect() const
  {
    point_t * tops = new point_t[3]{a_, b_, c_};
    rectangle_t res = generalGetFrameRect(tops, 3);
    delete[] tops;
    return res;
  }

  void Triangle::move(point_t p)
  {
    pos_ = p;
  }

  void Triangle::move(double dx, double dy)
  {
    pos_.x += dx;
    pos_.y += dy;
  }

  void Triangle::scale(double k)
  {
    a_.x *= k;
    a_.y *= k;
    b_.x *= k;
    b_.y *= k;
    c_.x *= k;
    c_.y *= k;

  }

  rectangle_t generalGetFrameRect(const point_t const * tops, size_t len)
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
    delete *oldTops;
    *oldTops = newTops;

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

  void showInfo(const Shape * const * figures, size_t len)
  {
    std::cout << "INFO ABOUT FIGURES\n";
    size_t fullSum = 0;
    size_t topsLen = 0;
    point_t * tops = new point_t[topsLen];

    for (size_t i = 0; i < len; ++i) {
      size_t figureArea = figures[i] -> getArea();
      rectangle_t figureFrameRect = figures[i] -> getFrameRect(); 
      point_t * frameRectCords = getCordsOfFrame(figureFrameRect);
      extend(&tops, frameRectCords, topsLen, 4);
      fullSum += figureArea;
      std::cout << "Площадь фигуры №" << i + 1 << ":" << figureArea << "\n";
      std::cout << "Ограничивающий прямиоугольник фигуры №" << i + 1 << " имеет следующие координаты:\n";
      for (size_t i = 0; i < 4; ++i) {
        std::cout << "\t(" << frameRectCords[i].x << "," << frameRectCords[i].y << ")\n";
      }
      delete[] frameRectCords;
    }
    std::cout << "\n";
    std::cout << "Суммарная площадь всех фигур равна: " << fullSum << "\n";
    rectangle_t allFigguresFrame = generalGetFrameRect(tops, topsLen);
    point_t * allFiguresFrameRectCords = getCordsOfFrame(allFigguresFrame);
    std::cout << "Ограничивающий прямиоугольник всех фигур имеет следующие координаты:\n";
    for (size_t i = 0; i < 4; ++i) {
      std::cout << "\t(" << allFiguresFrameRectCords[i].x << "," << allFiguresFrameRectCords[i].y << ")\n";
    }
  }
}


int main()
{
  std::cout << "Initial commit\n";
}
