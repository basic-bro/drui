// RoundedBox.cpp

#include "core/Database.hpp"
#include "core/Render.hpp"

using namespace Layouts;

Key RoundedBox( WidthRequest width, HeightRequest height, RoundedBoxFormat format, HitTestMethod hitTest, Key child )
{
	return CreateWidget( "",

		// initState
		[=] ( Key self )
		{
			CreateState<WidgetState>( self );
		},

		// buildLayout
		[ width, height ] ( Key self )
		{
			return Box( width, height );
		},

		// renderWidget
		[ format ] ( Key self, Rect r ) -> bool
		{
			auto const& mouseState = GetState<WidgetState>( self );
			auto c = format.colour;
			if( mouseState.isHovered )
			{
				c = { 255, 255, 0, 128 };
			}
			
			Render::DrawRoundedBox( r, format.radius, c );
			return true;
		},

		// hitTest
		hitTest,

		// children
		{
			child
		}
	);
}