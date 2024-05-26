// Column.cpp

#include "core/ui/Widgets.hpp"

using namespace Layouts;

Key Column( WidthRequest width, HeightRequest height, std::initializer_list<Key> children )
{
  return CreateWidget( "",
  
    // initState
    [] ( Key self )
    {
      CreateState<WidgetState>( self );
    },

    // buildLayout
    [ width, height ] ( Key self ) -> LayoutBuilder
    {
      return Column( width, height );
    },

    // renderWidget
    [] ( Key self, Rect r )
    {
      return true;
    },

    // hitTest
    DefaultHitTest,

    // children
    children
  );
}