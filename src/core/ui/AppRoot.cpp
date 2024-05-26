// AppRoot.cpp

#include "core/Database.hpp"
#include "core/Render.hpp"

static constexpr auto NO_TAG = "";

using namespace Layouts;

Key AppRoot( int w, int h, Key Main )
{
	return CreateWidget( "AppRoot",

		// initState
		[] ( Key self )
		{
			CreateState<WidgetState>( self );
		},
		
		// buildLayout
		[ w, h ] ( Key self ) -> LayoutBuilder
		{
			return Scaffold( w, h );
		},

		// renderLayout
		[] ( Key self, Rect r ) -> bool
		{
			// auto const& mouseState = GetState<WidgetState>( self );
			// if( mouseState.isHovered )
			// {
			// 	Render::DrawRect( r, { 255, 0, 0, 255 } );
			// }
			return true;
		},

		// hitTest
		DefaultHitTest,

		// children
		{
			Main
		}
	);
}