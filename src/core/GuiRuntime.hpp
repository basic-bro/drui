// GuiRuntime.hpp

#ifndef GUI_RUNTIME_HPP_INCLUDED
#define GUI_RUNTIME_HPP_INCLUDED

struct SDL_Window;
struct SDL_Renderer;

class GuiRuntime
{
private:
  SDL_Window*   m_Window = nullptr;
  SDL_Renderer* m_Renderer = nullptr;

public:
  GuiRuntime( int w, int h );
  ~GuiRuntime();

  void ShowWindow() const;
};

#endif