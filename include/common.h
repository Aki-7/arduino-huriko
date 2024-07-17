#pragma once

template<class T, class Min, class Max>
auto clamp(const T& a, const Min& min, const Max& max)
  -> decltype(a < min ? min : a > max ? max : a)
{
  return (a < min ? min : a > max ? max : a);
}
