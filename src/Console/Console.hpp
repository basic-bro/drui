// Console.hpp

#ifndef CONSOLE_HPP_INCLUDED
#define CONSOLE_HPP_INCLUDED

#include<iostream>
#include<format>

namespace Console
{
  template<typename ... Ts>
  void Print( std::format_string<Ts...> const fmt, Ts&& ... args )
  {
    std::cout << std::format( fmt, std::forward<Ts&&>( args ) ... );
  }

  template<typename ... Ts>
  void PrintLn( std::format_string<Ts...> const fmt, Ts&& ... args )
  {
    std::cout << std::format( fmt, std::forward<Ts&&>( args ) ... ) << '\n';
  }

  template<typename ... Ts>
  void Error( std::format_string<Ts...> const fmt, Ts&& ... args )
  {
    std::cerr << std::format( fmt, std::forward<Ts&&>( args ) ... );
  }

  template<typename ... Ts>
  void ErrorLn( std::format_string<Ts...> const fmt, Ts&& ... args )
  {
    std::cerr << std::format( fmt, std::forward<Ts&&>( args ) ... ) << '\n';
  }
} // namespace Console

#endif