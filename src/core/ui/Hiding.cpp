// Hiding.cpp

#include "core/Database.hpp"
#include "core/Render.hpp"

using namespace Layouts;

template<typename TriggeringState>
using VisibilityTest = std::function< bool( Key self, TriggeringState const& triggeringState ) >;

template<typename TriggeringState>
Key Hiding( bool initialVisibility, Key triggeringKey, VisibilityTest<TriggeringState> visibilityTest, Key child )
{
	return CreateWidget( "",

		// initState
		[=] ( Key self )
		{
			CreateState<WidgetState>( self, WidgetState{ .isVisible = initialVisibility } );
			ObserveState<TriggeringState>( self, triggeringKey,
				[] ( Key self, TriggeringState const& triggeringState )
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