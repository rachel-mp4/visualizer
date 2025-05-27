#ifndef SAMPLE_H
#define SAMPLE_H
#include <cmath>

class Sample {
public:
  float left;
  float right;
  Sample(float l = 0.0, float r = NAN);
  Sample operator+(const Sample &other) const;
  Sample operator-(const Sample &other) const;
  Sample operator*(const Sample &other) const;
  Sample operator/(const Sample &other) const;
  Sample operator*(const float f) const;
  friend Sample operator*(float f, const Sample &s);
  Sample operator/(const float f) const;
  Sample &operator+=(const Sample &other);
  Sample &operator-=(const Sample &other);
  Sample &operator*=(const Sample &other);
  Sample &operator/=(const Sample &other);
  Sample &operator*=(float f);
  Sample &operator/=(float f);
};

#endif
