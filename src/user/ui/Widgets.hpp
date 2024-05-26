// user/ui/Widgets.hpp

#ifndef USER_UI_WIDGETS_HPP_INCLUDED
#define USER_UI_WIDGETS_HPP_INCLUDED

#include "core/Database.hpp"

using namespace Layouts;

Key ChatApp();

Key ChatBubbles( WidthRequest wr, HeightRequest hr, rgba32 colour, ChatEntry const& chatEntry );

#endif