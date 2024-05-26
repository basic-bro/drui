// Render.cpp

#include "Render.hpp"
#include "FontCache.hpp"

#include<SDL_FontCache/SDL_FontCache.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL2_gfxPrimitives.h>

#include<stdexcept>
#include<map>
#include<array>

#include<iostream>

namespace Render
{
  static SDL_Renderer* s_Renderer = nullptr;  
  static FontCache s_FontCache{ s_Renderer };

  struct FontLoadSpecs
  {
    std::string_view face;
    std::string_view file;
    int pointSize;
    rgba32 colour;
    int ttfStyle;
  };

  static constexpr std::array s_FontLoadSpecs = 
  {
    FontLoadSpecs{ "Roboto", "./Fonts/Roboto-Black.ttf", 12, rgba32{ .r = 255, .g = 255, .b = 255, .a = 255 }, TTF_STYLE_NORMAL },
  };

  FC_Font* DefaultFont()
  {
    return s_FontCache.DefaultFont();
  }

  void Init( SDL_Renderer* renderer )
  {
    s_Renderer = renderer;
    SDL_SetRenderDrawBlendMode( s_Renderer, SDL_BLENDMODE_BLEND );
    
    for( auto i = 0; i < s_FontLoadSpecs.size(); ++i )
    {
      auto const& specs = s_FontLoadSpecs.at( i );
      s_FontCache.AddFont( specs.face, specs.file, specs.pointSize, specs.colour, specs.ttfStyle, i == 0 );
    }
  }

  void DrawRect( Rect r, rgba32 colour )
  {
    SDL_Rect dst = { .x = r.x, .y = r.y, .w = r.w, .h = r.h };
    SDL_SetRenderDrawColor( s_Renderer, colour.r, colour.g, colour.b, colour.a );
    SDL_RenderDrawRect( s_Renderer, &dst );
  }

  void DrawFilledRect( Rect r, rgba32 colour )
  {
    SDL_Rect dst = { .x = r.x, .y = r.y, .w = r.w, .h = r.h };
    SDL_SetRenderDrawColor( s_Renderer, colour.r, colour.g, colour.b, colour.a );
    SDL_RenderFillRect( s_Renderer, &dst );
  }

  int CalcTextHeight( Rect r, std::string const& s )
  {
    SDL_Rect dst{ .x = r.x, .y = r.y, .w = r.w, .h = r.h };
    return FC_CalcRequiredHeight( DefaultFont(), s_Renderer, dst, FC_ALIGN_LEFT, "%s", s.c_str() );
  }

  void DrawText( Rect r, std::string const& s )
  {
    SDL_Rect dst{ .x = r.x, .y = r.y, .w = r.w, .h = r.h };
    FC_DrawBoxAlign( DefaultFont(), s_Renderer, dst, FC_ALIGN_LEFT, "%s", s.c_str() );
  }

  void DrawRoundedBox( Rect r, int radius, rgba32 colour )
  {
    roundedBoxRGBA( s_Renderer, r.x, r.y, r.x + r.w, r.y + r.h, radius, colour.r, colour.g, colour.b, colour.a );
  }

  void SetClipRect( Rect clip )
  {
    SDL_Rect sdlCLip = { clip.x, clip.y, clip.w, clip.h };
    SDL_RenderSetClipRect( s_Renderer, &sdlCLip );
  }

  void ResetClipRect()
  {
    SDL_RenderSetClipRect( s_Renderer, NULL );
  }

  void ClearScreen()
  {
    SDL_SetRenderDrawColor( s_Renderer, 0, 0, 0, 255 );
    SDL_RenderClear( s_Renderer );
  }

  void Present()
  {
    SDL_RenderPresent( s_Renderer );
  }


} // namespace Render