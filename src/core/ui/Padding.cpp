// Padding.cpp

#include "core/ui/Widgets.hpp"

Key Padding( WidthRequest wr, HeightRequest hr, int left, int right, int top, int bottom, Key child )
{
  return CreateWidget( "",
  
    // initState
    [] ( Key self )
    {
      CreateState<WidgetState>( self );
    },

    // buildLayout
    [=] ( Key self ) -> LayoutBuilder
    {
      return Padding( wr, hr, left, right, top, bottom );
    },

    // renderWidget
    [] ( Key self, Rect r ) -> bool
    {
      return true;
    },

    // hitTest
    DefaultHitTest,

    // children
    {
      child
    }
  );
}