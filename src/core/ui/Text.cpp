// Text.cpp

#include "core/Database.hpp"
#include "core/Render.hpp"

using namespace Layouts;

Key Text( WidthRequest width, HeightRequest height, std::string const& text, Font font )
{
	return CreateWidget( "",

		// initState
		[ height ] ( Key self )
		{
			CreateState<WidgetState>( self );

			// does the height need to be calculated from the width?
			if( height.requestType == Intervals::RequestType::AtLeast )
			{
				// yes, so create state to allow width-height communication during Layout dimensioning
				CreateState<TextState>( self );
			}
		},

		// buildLayout
		[ width, height, &text ] ( Key self )
		{
			// does the height need to be calculated from the width?
			if( height.requestType == Intervals::RequestType::AtLeast )
			{
				// yes, so create a custom Layout that enables width-height communication
				// during Layout dimensioning
				auto& actualWidth = const_cast<int&>( GetState<TextState>( self ).width );
				return LayoutBuilder
				{
					Intervals::IntervalBuilder
					{
						Intervals::IntervalProps
						{
							.extentRequest = width,
							.extentNotifier = [ &, self ] ( int width )
							{
								actualWidth = width;
								// Console::Print( "\nWidget {} setting TextState::width to {}.", self, actualWidth );
							}
						}
					},
					Intervals::IntervalBuilder
					{
						Intervals::IntervalProps
						{
							.extentRequest = height,
							.deduceExtent = [ &, self ] ( Intervals::IntervalBuilder const& )
							{
								auto h = Render::CalcTextHeight( Rect{ .w = actualWidth }, text );
								// Console::Print( "\nWidget {} with text '{}' returning a calculated height of {}.", self, text, h );
								return h;
							}
						}
					}
				};
			}

			// no, so return default Layout
			return Box( width, height );
		},

		// renderWidget
		[ text, font ] ( Key self, Rect r ) -> bool
		{
			// Console::Print( "\nWidget {} has render height {}.", self, r.h );
			// Render::DrawRect( r, White );
			Render::DrawText( r, text );
			return true;
		},

		// hitTest
		RefuseHitTest,

		// children
		{}
	);
}