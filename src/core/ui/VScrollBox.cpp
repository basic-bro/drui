// VScrollBox.cpp

#include "core/Database.hpp"
#include "core/Render.hpp"


using namespace Layouts;

static void calcVisibleChildren( Key self, Transform const& transform )
{
	// calculate visible children whenever transform changes
	// NB: this is the slow way, recomputing the whole array
	// on each update
	auto parentRect = GetWidgetRect( self );
	std::vector<Key> visibleChildren;
	for( auto const child : GetChildWidgets( self ) )
	{
		// init calculations
		auto childRect  = GetWidgetRect( child );
		auto childHead  = childRect.y;
		auto childFoot  = childHead + childRect.h;
		auto parentHead = transform.y;
		auto parentFoot = parentHead + parentRect.h;

		// this is how I can make sense of it right now, ok?
		bool childFoot_above_parentHead = childFoot < parentHead;
		bool childHead_below_parentFoot = childHead > parentFoot;
		if( childFoot_above_parentHead )
		{
			continue;
		}
		if( childHead_below_parentFoot )
		{
			break;
		}

		// child is visible
		visibleChildren.push_back( child );
	}

	SetState<VisibleChildren>( self,
		[=] ( VisibleChildren& vc )
		{
			vc.children = visibleChildren;
		}
	);
}

Key VScrollBox(
  WidthRequest wr,
  HeightRequest hr,
  int initialVScroll,
  Custom custom,
  std::initializer_list<Key> children )
{
  return CreateWidget( "",

		// initState
		[ initialVScroll ] ( Key self )
		{
			// default state
			CreateState<WidgetState>( self );

			// VScrollBox state
			CreateState<VisibleChildren>( self );
			CreateState<Transform>( self, Transform{ .y = initialVScroll }, true );
			ObserveState<Transform>( self, self,
				[] ( Key self, Transform const& transform )
				{
					// Console::PrintLn( "\ntransform.y = {}", transform.y );

					// clamp transform.y at the top
					if( transform.y < 0 )
					{	
						SetState<Transform>( self, [] ( Transform& transform ) { transform.y = 0; } );
					}

					// clamp transform.y at the bottom
					auto const& visibleChildren = GetState<VisibleChildren>( self ).children;
					if( visibleChildren.size() )
					{
						auto lastChild = GetChildWidgets( self ).back();
						if( contains( visibleChildren, lastChild ) )
						{
							// Console::Print( "\nLast child is visible." );
							// last child is visible, so check it's LayoutRect
							auto parentRect = GetWidgetRect( self );
							auto childRect = GetWidgetRect( lastChild ).WithTransform( 0, parentRect.y - transform.y );
							
							// enforce that the last child is not disappearing off the top
							auto delta = childRect.y - parentRect.y;
							// Console::Print( " delta = {}", delta );
							if( delta < 0 )
							{
								SetState<Transform>( self,
									[ delta ] ( Transform& transform )
									{
										transform.y += delta;
									}
								);
							}
						}
					}
					
					calcVisibleChildren( self, transform );
				}
			);
			CreateState<OnBuildLayout>( self );
			ObserveState<OnBuildLayout>( self, self,
				[] ( Key self, OnBuildLayout const& )
				{
					calcVisibleChildren( self, GetState<Transform>( self ) );
				}
			);
		},
			
		// buildLayout
		[ wr, hr ] ( Key self ) -> LayoutBuilder
		{
			SetState<OnBuildLayout>( self, [] ( OnBuildLayout& ) {} );
			return VerticalScrollView( wr, hr );
		},

		// renderWidget
		[] ( Key self, Rect r ) -> bool
		{
			auto const& visibleChildren = GetState<VisibleChildren>( self );
			auto const& transform = GetState<Transform>( self );
			Render::SetClipRect( r );
			// Render::DrawRect( r, White );

			// render children myself
			for( auto const child : visibleChildren.children )
			{
				RenderLayoutTree( child, 0, r.y - transform.y );
			}
			Render::ResetClipRect();
			return false;
		},

		// hitTest
		[] ( LayoutBuilder const& l, int x, int y, std::vector<Key>& hitTree ) -> bool
		{
			// is (x, y) within my hit box?
			auto r = l.GetRect();
			if( r.x <= x && x < r.x + r.w
			 && r.y <= y && y < r.y + r.h )
			{
				// yes, so test my (visible) children,
				// adjusting for Transform
				auto self = l.GetKey();
				hitTree.push_back( self );
				auto numHits = hitTree.size();
				auto const& transform = GetState<Transform>( self );
				for( auto const child : GetState<VisibleChildren>( self ).children )
				{
					RunHitTests( child, x + transform.x - r.x, y + transform.y - r.y, hitTree );
					if( hitTree.size() != numHits )
					{
						return false;
					}
				}
			}

			// no
			return false;
		},

    // custom
    custom,

		// children
		children
	);
}