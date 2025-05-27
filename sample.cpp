#include "sample.h"

Sample::Sample(float l, float r) : left(l), right(std::isnan(r) ? l : r) {}

Sample Sample::operator+(const Sample &other) const {
  return Sample(left + other.left, right + other.right);
}
Sample Sample::operator-(const Sample &other) const {
  return Sample(left - other.left, right - other.right);
}
Sample Sample::operator*(const Sample &other) const {
  return Sample(left * other.left, right * other.right);
}
Sample Sample::operator/(const Sample &other) const {
  return Sample(left / other.left, right / other.right);
}
Sample Sample::operator*(const float f) const {
  return Sample(left * f, right * f);
}
Sample operator*(float f, const Sample &s) { return s * f; }
Sample &Sample::operator+=(const Sample &other) {
  left += other.left;
  right += other.right;
  return *this;
}
Sample &Sample::operator-=(const Sample &other) {
  left -= other.left;
  right -= other.right;
  return *this;
}
Sample &Sample::operator*=(const Sample &other) {
  left *= other.left;
  right *= other.right;
  return *this;
}
Sample &Sample::operator/=(const Sample &other) {
  left /= other.left;
  right /= other.right;
  return *this;
}
Sample &Sample::operator*=(float f) {
  left *= f;
  right *= f;
  return *this;
}

Sample Sample::operator/(const float f) const {
  return Sample(left / f, right / f);
}
