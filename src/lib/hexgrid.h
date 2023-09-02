// Generated code -- CC0 -- No Rights Reserved --
// http://www.redblobgames.com/grids/hexagons/
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <sstream>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace hex {

using std::abs;
using std::max;
using std::vector;

struct Point {
  const double x;
  const double y;
  Point(double x_, double y_) : x(x_), y(y_) {}
};

struct Hex {
  int q;
  int r;
  int s;
  // Hex(int q_, int r_, int s_) : q(q_), r(r_), s(s_) {
  //   if (q + r + s != 0)
  //     throw "q + r + s must be 0";
  // }

  friend std::strong_ordering operator<=>(const Hex &l, const Hex &r) = default;

  std::string str() const & {
    std::stringstream ss{};
    ss << "[" << q << "," << r << "," << s << "]";

    return ss.str();
  }
};

struct FractionalHex {
  const double q;
  const double r;
  const double s;
  FractionalHex(double q_, double r_, double s_) : q(q_), r(r_), s(s_) {
    if (round(q + r + s) != 0)
      throw "q + r + s must be 0";
  }
};

struct OffsetCoord {
  const int col;
  const int row;
  OffsetCoord(int col_, int row_) : col(col_), row(row_) {}
};

struct DoubledCoord {
  const int col;
  const int row;
  DoubledCoord(int col_, int row_) : col(col_), row(row_) {}
};

struct Orientation {
  const double f0;
  const double f1;
  const double f2;
  const double f3;
  const double b0;
  const double b1;
  const double b2;
  const double b3;
  const double start_angle;
  Orientation(double f0_, double f1_, double f2_, double f3_, double b0_,
              double b1_, double b2_, double b3_, double start_angle_)
      : f0(f0_), f1(f1_), f2(f2_), f3(f3_), b0(b0_), b1(b1_), b2(b2_), b3(b3_),
        start_angle(start_angle_) {}
};

struct Layout {
  const Orientation orientation;
  const Point size;
  const Point origin;
  Layout(Orientation orientation_, Point size_, Point origin_)
      : orientation(orientation_), size(size_), origin(origin_) {}
};

Hex hex_add(Hex a, Hex b);

Hex hex_subtract(Hex a, Hex b);

Hex hex_scale(Hex a, int k);

Hex hex_rotate_left(Hex a);

Hex hex_rotate_right(Hex a);

const vector<Hex> hex_directions = {Hex(1, 0, -1), Hex(1, -1, 0),
                                    Hex(0, -1, 1), Hex(-1, 0, 1),
                                    Hex(-1, 1, 0), Hex(0, 1, -1)};
Hex hex_direction(int direction);

Hex hex_neighbor(Hex hex, int direction);

const vector<Hex> hex_diagonals = {Hex(2, -1, -1), Hex(1, -2, 1),
                                   Hex(-1, -1, 2), Hex(-2, 1, 1),
                                   Hex(-1, 2, -1), Hex(1, 1, -2)};
Hex hex_diagonal_neighbor(Hex hex, int direction);

int hex_length(Hex hex);

int hex_distance(Hex a, Hex b);

Hex hex_round(FractionalHex h);

FractionalHex hex_lerp(FractionalHex a, FractionalHex b, double t);

vector<Hex> hex_linedraw(Hex a, Hex b);

const int EVEN = 1;
const int ODD = -1;
OffsetCoord qoffset_from_cube(int offset, Hex h);

Hex qoffset_to_cube(int offset, OffsetCoord h);

OffsetCoord roffset_from_cube(int offset, Hex h);

Hex roffset_to_cube(int offset, OffsetCoord h);

DoubledCoord qdoubled_from_cube(Hex h);

Hex qdoubled_to_cube(DoubledCoord h);

DoubledCoord rdoubled_from_cube(Hex h);

Hex rdoubled_to_cube(DoubledCoord h);

const Orientation layout_pointy =
    Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0, sqrt(3.0) / 3.0,
                -1.0 / 3.0, 0.0, 2.0 / 3.0, 0.5);
const Orientation layout_flat =
    Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0,
                -1.0 / 3.0, sqrt(3.0) / 3.0, 0.0);
Point hex_to_pixel(Layout layout, Hex h);

FractionalHex pixel_to_hex(Layout layout, Point p);

Point hex_corner_offset(Layout layout, int corner);

vector<Point> polygon_corners(Layout layout, Hex h);

Hex cube_round(FractionalHex frac);

// Tests
} // namespace hex

