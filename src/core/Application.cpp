// Application.cpp

#include "Application.hpp"
#include "Database.hpp"
#include "Render.hpp"
#include "Timer.hpp"

#include<SDL2/SDL.h>

Application::Application( int w, int h, Key root )
  : m_Gui{ w, h },
		m_App{ root }
{ }

void Application::Run()
{
	// show Window
	m_Gui.ShowWindow();

  // init m_App
	InitWidgetTree( m_App, NullKey );

  // init main loop
	int mouseX;
	int mouseY;
	SDL_GetMouseState( &mouseX, &mouseY );
  Key prev_hovered = 0;
	bool finished = false;
	Timer<60> timer;
	while( !finished )
	{ 
		// rebuild?
		if( ShouldRebuildLayoutTree() )
		{
			RebuildLayoutTree( m_App );
		}

		int mouseWheelDelta = 0;

		SDL_Event e;
		while( SDL_PollEvent( &e ) )
		{
			switch( e.type )
			{
				case SDL_QUIT:
				{
					finished = true;
					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEMOTION:
				{
					auto buttons = SDL_GetMouseState( &mouseX, &mouseY );
					break;
				}

				case SDL_MOUSEWHEEL:
				{
					mouseWheelDelta = e.wheel.y;
					break;
				}
			}
		}

		// compare new and old hit trees
		auto const prevHitTree = GetHitTree();
		ClearHitTree();
		RunHitTests( m_App, mouseX, mouseY );
		auto const& currHitTree = GetHitTree();

		// turn off widgets that have left the hit tree
		for( auto const widget : prevHitTree )
		{
			if( !contains( currHitTree, widget ) )
			{
				SetState<WidgetState>( widget,
					[] ( WidgetState& widgetState )
					{
						widgetState.isInHitTree = false;
					}
				);
			}
		}

		// turn on widgets that are new to the tree
		for( auto const widget : currHitTree )
		{
			if( !contains( prevHitTree, widget ) )
			{
				SetState<WidgetState>( widget,
					[] ( WidgetState& widgetState )
					{
						widgetState.isInHitTree = true;
					}
				);
			}
		}

		// update hovered widget
		auto hovered = currHitTree.size() ? currHitTree.back() : 0;
		if( hovered != prev_hovered )
		{
			if( hovered )
			{
				SetState<WidgetState>( hovered,
					[] ( WidgetState& widgetState )
					{
						widgetState.isHovered = true;
					}
				);
			}

			if( prev_hovered )
			{
				SetState<WidgetState>( prev_hovered,
					[] ( WidgetState& widgetState )
					{
						widgetState.isHovered = false;
					}
				);
			}

			prev_hovered = hovered;
		}

		// handle mouse wheel
		if( mouseWheelDelta )
		{
			auto const& hitTree = GetHitTree();
			for( auto it = hitTree.crbegin(); it != hitTree.crend(); it++ )
			{
				if( HasState<Transform>( *it ) )
				{
					SetState<Transform>( *it,
						[ mouseWheelDelta ] ( Transform& transform )
						{
							transform.y -= mouseWheelDelta * 10;
						}
					);
					break;
				}
			}
		}

		// update other things ...
		

		// process changes
		FlushCallbacks();

		// rebuild?
		if( ShouldRebuildLayoutTree() )
		{
			RebuildLayoutTree( m_App );
		}

		// render
		Render::ClearScreen();
		RenderLayoutTree( m_App );
		Render::Present();

		// wait for next frame
		timer.Sleep();
	}

}