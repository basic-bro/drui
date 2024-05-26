// Widget.hpp

#ifndef WIDGET_HPP_INCLUDED
#define WIDGET_HPP_INCLUDED

#include<functional>
#include<string>
#include<tuple>

#include<Layout/Layouts.hpp>

#include "Key.hpp"

using InitStateMethod    = std::function< void( Key ) >;
using BuildLayoutMethod  = std::function< Layouts::LayoutBuilder( Key ) >;
using RenderWidgetMethod = std::function< bool( Key, Layouts::Rect ) >;
using HitTestMethod      = std::function< bool( Layouts::LayoutBuilder const&, int, int, std::vector<Key>& ) >;

struct Widget
{
	std::string tag;
	Key self;
	Key parent;
	InitStateMethod initState;
	BuildLayoutMethod buildLayout;
	HitTestMethod runHitTest;
	RenderWidgetMethod renderWidget;
	std::vector<Key> children;
	Layouts::LayoutBuilder layout;
};


#endif