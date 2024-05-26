// Align.cpp

#include "core/Database.hpp"
#include "core/Render.hpp"

#include<format>

using namespace Layouts;

Key HSpace()
{
  return CreateWidget( "",
    [] ( Key ) {},
    [] ( Key ) -> LayoutBuilder
    {
      return Layouts::HSpace();
    },
    [] ( Key self, Rect r ) -> bool
    {
			// Render::DrawRect( r, Blue );
      return false;
    },
		RefuseHitTest,
    {}
  );
}

Key AlignLeft( WidthRequest width, HeightRequest height, Key child )
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
			return AlignLeft( width, height );
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
			child,
      ::HSpace()
		}
  );
}

Key AlignRight( WidthRequest width, HeightRequest height, Key child )
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
			return AlignRight( width, height );
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
      ::HSpace(),
      child
		}
  );
}