// Widgets.hpp
// standard library of built-in widgets

#ifndef CORE_UI_WIDGETS_HPP_INCLUDED
#define CORE_UI_WIDGETS_HPP_INCLUDED

#include "core/Database.hpp"
#include "core/Render.hpp"
#include "core/Props.hpp"

using namespace Layouts;

static constexpr auto NO_TAG = "";

Key AppRoot( int w, int h, Key Main );

Key Text( WidthRequest width, HeightRequest height, std::string const& text, Font font );

Key RoundedBox( WidthRequest width, HeightRequest height, RoundedBoxFormat format, HitTestMethod hitTest, Key child );

Key AlignLeft( WidthRequest width, HeightRequest height, Key child );

Key AlignRight( WidthRequest width, HeightRequest height, Key child );

Key VScrollBox( WidthRequest wr, HeightRequest hr, int initialVScroll, Custom custom, std::initializer_list<Key> children );

Key Column( WidthRequest width, HeightRequest height, std::initializer_list<Key> children );

Key Padding( WidthRequest wr, HeightRequest hr, int left, int right, int top, int bottom, Key child );


using KeyFinder = std::function< Key( Key self ) >;

template<typename TriggeringState>
using VisibilityTest = std::function< bool( Key self, TriggeringState const& triggeringState ) >;

template<typename TriggeringState>
Key Hiding( bool initialVisibility, KeyFinder getTriggeringKey, VisibilityTest<TriggeringState> visibilityTest, Key child )
{
	return CreateWidget( "",

		// initState
		[ initialVisibility, getTriggeringKey, visibilityTest ] ( Key self )
		{
			CreateState<WidgetState>( self, WidgetState{ .isVisible = initialVisibility } );
			ObserveState<TriggeringState>( self, getTriggeringKey( self ),
				[ visibilityTest ] ( Key self, TriggeringState const& triggeringState )
				{
          // the triggering state has changed. Determine if
          // I should be visible
					auto iShouldBeVisible = visibilityTest( self, triggeringState );

					// early exit: no need to change
					if( GetState<WidgetState>( self ).isVisible == iShouldBeVisible )
					{
						return;
					}

					// schedule the change
					SetState<WidgetState>( self,
						[=] ( WidgetState& selfState )
						{
							selfState.isVisible = iShouldBeVisible;
							SetRebuildLayoutTree();
						}
					);
				}
			);
		},

		// buildLayout
		[] ( Key self )
		{
			auto const& widgetState = GetState<WidgetState>( self );
			if( widgetState.isVisible )
			{
				return Box( AutoWidth, AutoHeight );
			}
			return Box( WidthExactly( 0 ), HeightExactly( 0 ) );
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

#endif