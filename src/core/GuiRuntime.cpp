// GuiRuntime.cpp

#include "GuiRuntime.hpp"
#include "Render.hpp"

#include<SDL2/SDL.h>

#include<stdexcept>
#include<format>

GuiRuntime::GuiRuntime( int w, int h )
{
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    throw std::runtime_error{ std::format( "Error initialising SDL: {}", SDL_GetError() ) };
  }

  if( SDL_CreateWindowAndRenderer( w, h, SDL_WINDOW_HIDDEN, &m_Window, &m_Renderer ) < 0 )
  {
    m_Window   = nullptr;
    m_Renderer = nullptr;
    throw std::runtime_error{ std::format( "Error creating window: {}", SDL_GetError() ) };
  }

  Render::Init( m_Renderer );
}

GuiRuntime::~GuiRuntime()
{
  if( m_Renderer )
  {
    SDL_DestroyRenderer( m_Renderer );
  }

  if( m_Window )
  {
    SDL_DestroyWindow( m_Window );
  }
  
  SDL_Quit(); // safe to call even if errors in SDL_Init()
}

void GuiRuntime::ShowWindow() const
{
  SDL_ShowWindow( m_Window );
}