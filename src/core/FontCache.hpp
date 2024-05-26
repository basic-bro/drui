// FontCache.hpp

#ifndef CORE_FONT_CACHE_HPP_INCLUDED
#define CORE_FONT_CACHE_HPP_INCLUDED

#include "rgba32.hpp"

#include<SDL2/SDL_stdinc.h>

#include<map>
#include<string_view>
#include<vector>
#include<filesystem>

struct SDL_Renderer;
struct FC_Font;

struct FontDescriptor
{
  Uint32 pointSize;
  rgba32 colour;
  int ttf_style;
  FC_Font* ptr = nullptr;
};

class FontCache
{
private:
  SDL_Renderer* const& m_Renderer;
  std::map<std::string_view, std::vector<FontDescriptor>> m_Cache;
  FC_Font* m_DefaultFont;

public:
  FontCache( SDL_Renderer* const& renderer );

  void AddFont(
    std::string_view face,
    std::filesystem::path file,
    Uint32 pointSize,
    rgba32 colour,
    int ttfStyle,
    bool setAsDefault = false
  );

  FC_Font* DefaultFont() const;

private:
  bool exists( std::string_view face, Uint32 pointSize, rgba32 colour, int ttfStyle );

};


#endif