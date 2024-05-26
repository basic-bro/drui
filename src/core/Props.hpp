// Props.hpp

#ifndef PROPS_HPP_INCLUDED
#define PROPS_HPP_INCLUDED

#include "core/rgba32.hpp"
#include "core/Widget.hpp"

#include<string_view>

///////////
// Props //
///////////

struct Custom
{
  InitStateMethod extraInitState;
  BuildLayoutMethod overrideBuildLayout;
  RenderWidgetMethod overrideRenderWidget;
  HitTestMethod overrideHitTest;
};

struct RoundedBoxFormat
{
	rgba32 colour = White;
	int radius = 0;
};

struct Font
{
	std::string_view face;
	rgba32 colour;
	int pointSize;
	bool isBold;
	bool isItalic;
};


#endif