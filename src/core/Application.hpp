// Application.hpp

#ifndef CORE_APPLICATION_HPP_INCLUDED
#define CORE_APPLICATION_HPP_INCLUDED

#include "core/GuiRuntime.hpp"
#include "core/Key.hpp"

class Application
{
private:
  GuiRuntime m_Gui;
  Key        m_App;

public:
  Application( int w, int h, Key root );
  void Run();
};

#endif