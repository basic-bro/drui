// Render.hpp

#ifndef RENDER_HPP_INCLUDED
#define RENDER_HPP_INCLUDED

#include "rgba32.hpp"

#include<Layout/Layouts.hpp>

#include<string>

struct SDL_Renderer;

namespace Render
{
  using namespace Layouts;

  void Init( SDL_Renderer* renderer );

  void DrawRect( Rect r, rgba32 colour );
  void DrawFilledRect( Rect r, rgba32 colour );
  void DrawRoundedBox( Rect r, int radius, rgba32 colour );
  void DrawText( Rect r, std::string const& s );

  int CalcTextHeight( Rect r, std::string const& s );

  void ClearScreen();
  void Present();

  void SetClipRect( Rect clip );
  void ResetClipRect();

} // namespace Render

#endif