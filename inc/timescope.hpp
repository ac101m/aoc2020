#ifndef TIMESCOPE_INCLUDED
#define TIMESCOPE_INCLUDED

#include <chrono>
#include <iostream>


class TimeScope
{
private:
  std::string const name;

  std::chrono::high_resolution_clock::time_point t_start;
  std::chrono::high_resolution_clock::time_point t_end;




public:
  TimeScope(std::string const name) :
    name(name),
    t_start(std::chrono::high_resolution_clock::now())
  {}

  ~TimeScope()
  {
    t_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seconds = t_end - t_start;
    std::cout << name << ": " << seconds.count() << "s" << std::endl;
  }
};


#endif // TIMESCOPE_INCLUDED
