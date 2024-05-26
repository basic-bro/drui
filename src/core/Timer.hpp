// Timer.hpp

// https://stackoverflow.com/questions/77744136/achieving-stable-60fps-with-sdl2

#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <type_traits>

template<std::intmax_t FPS>
class Timer
{
public:
  static constexpr std::chrono::duration<double, std::ratio<1, FPS>> time_between_frames{ 1 };

private:
  std::chrono::time_point<std::chrono::steady_clock, std::remove_const_t<decltype( time_between_frames )>> tp;

public:
  Timer() : tp{ std::chrono::steady_clock::now() } {}

  void Sleep()
  {
      tp += time_between_frames;
      std::this_thread::sleep_until(tp - std::chrono::microseconds(100));
      // while(std::chrono::steady_clock::now() < tp) {}
  }
};

#endif