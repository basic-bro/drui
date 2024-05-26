// ChatBubbles.cpp

#include "core/ui/Widgets.hpp"

#include<format>

Key ChatBubbles( WidthRequest width, HeightRequest height, rgba32 colour, ChatEntry const& chatEntry )
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
		[] ( Key self, Rect r ) -> bool
		{
			// Render::DrawRect( r, Green );
			// Console::Print( "\n{} x = {}, width = {}", self, r.x, r.w );
			return true;
		},

		// hitTest
		DefaultHitTest,

		// children
		{
			// Padding( AutoWidth, AutoHeight, 5, 5, 10, 10, 

				// child
				RoundedBox(
					AutoWidth, AutoHeight,
					RoundedBoxFormat
					{
						.colour = colour,
						.radius = 10
					},
					DefaultHitTest,

					// child
					Padding( AutoWidth, AutoHeight, 5, 5, 5, 5,

						// child
						Text(
							AutoWidth, AutoHeight,
							chatEntry.speech,
							Font
							{
								.face = "Roboto",
								.colour = rgba32{ 255, 255, 255, 255 },
								.pointSize = 12,
								.isBold = false,
								.isItalic = false
							}
						)
					)
				// )
			),
			Hiding<WidgetState>(

				// initialVisibility
				false,

				// getTriggeringKey
				[] ( Key self ) -> Key
				{
					return GetParentWidget( self, 1 );
				},

				// visibilityTest
				[] ( Key self, WidgetState const& widgetState )
				{
					return widgetState.isInHitTree;
				},

				// child
				Padding( AutoWidth, AutoHeight, 5, 5, 10, 10,

					// child
					RoundedBox(
						AutoWidth, AutoHeight,
						RoundedBoxFormat
						{
							.colour = colour,
							.radius = 10
						},
						DefaultHitTest,

						// child
						Padding( AutoWidth, AutoHeight, 5, 5, 5, 5,

							// child
							Text(
								AutoWidth, AutoHeight,
								chatEntry.author,
								Font
								{
									.face = "Roboto",
									.colour = rgba32{ 255, 255, 255, 255 },
									.pointSize = 12,
									.isBold = false,
									.isItalic = false
								}
							)
						)
					)
				)
			)
		}
	);
}