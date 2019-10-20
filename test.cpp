#include <string>
#include <iostream>

#include "result.hpp"


auto triple(int val) -> result<int, std::string>
{
  if (val < 100) {
    return success{val * 3};
  } else {
    return failure{"can't go over 100!"};
  }
}

auto convert(int val) -> result<float, std::string>
{
  return success{(float)val};
}

auto wrong(int val) -> float
{
  return (float)val;
}

void print_success(float val)
{
  std::cout << "success! value is " << val << std::endl;
}

void print_failure(std::string val)
{
  std::cout << "reason for error = " << val << std::endl;
}

int
main(int argc, char* argv[])
{
  auto s = success{3};
  auto f = failure{"fail"};

  auto r1 = s >> triple >> convert;
  auto r10 = s >> wrong;
  r1(print_success, print_failure);
}
