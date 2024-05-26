// FontCache.cph

#include "FontCache.hpp"

#include<SDL_FontCache/SDL_FontCache.h>

#include<format>
#include<stdexcept>

FontCache::FontCache( SDL_Renderer* const& renderer )
  : m_Renderer{ renderer }
{ }

FC_Font* FontCache::DefaultFont() const
{
  return m_DefaultFont;
}

bool FontCache::exists( std::string_view face, Uint32 pointSize, rgba32 colour, int ttfStyle )
{
  // early exit - not even the font face exists
  if( !m_Cache.contains( face ) )
  {
    return false;
  }

  // slower path: look through each FontDescriptor corresponding to the given face
  auto const& fds = m_Cache.at( face );
  auto it = std::find_if( fds.cbegin(), fds.cend(),
    [=] ( FontDescriptor const& font )
    {
      return font.pointSize == pointSize && font.colour == colour && font.ttf_style == ttfStyle;
    }
  );

  return it != fds.cend();
}

void FontCache::AddFont(
  std::string_view face,
  std::filesystem::path file,
  Uint32 pointSize,
  rgba32 colour,
  int ttfStyle,
  bool setAsDefault )
{
  // early exit: font already exists in cache
  if( exists( face, pointSize, colour, ttfStyle ) )
  {
    return;
  }

  // load font
  auto font = FC_CreateFont();
  SDL_Color color = { colour.r, colour.g, colour.b, colour.a };
  auto result = FC_LoadFont( font, m_Renderer, file.string().c_str(), pointSize, color, ttfStyle );
  if( result == 0 )
  {
    throw std::runtime_error{ std::format( "Error while loading font '{}' from file '{}'.", face, file.string() ) };
  }

  // default font?
  if( setAsDefault )
  {
    m_DefaultFont = font;
  }

  // add to cache
  m_Cache[ face ].push_back( FontDescriptor{ .pointSize = pointSize, .colour = colour, .ttf_style = ttfStyle, .ptr = font } );
}