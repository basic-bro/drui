// Database.hpp

#ifndef DATABASE_HPP_INCLUDED
#define DATABASE_HPP_INCLUDED

#include "core/Widget.hpp"
#include "core/Props.hpp"

#include "core/State.hpp"
#include "user/State.hpp"

#include<Console/Console.hpp>

template<typename State>
using ObserverMethod = std::function< void( Key observer, State const& )>;

template<typename State>
using SetStateMethod  = std::function< void( State& ) >;

using WidgetPredicate = std::function< bool( Key ) >;

extern HitTestMethod DefaultHitTest;
extern HitTestMethod RefuseHitTest;


template<typename State>
void CreateState( Key widget, State s = {}, bool markAsDirty = false );

template<typename State>
State const& GetState( Key widget );

template<typename State>
State const& ObserveState( Key observer, Key observed, ObserverMethod<State> callback );

template<typename State>
void SetState( Key widget, SetStateMethod<State> setState );

template<typename State>
bool HasState( Key widget );

#define DECLARE_FUNCTION_TEMPLATES( STATE ) \
extern template void CreateState( Key widget, STATE s, bool markAsDirty ); \
extern template STATE const& GetState( Key widget ); \
extern template STATE const& ObserveState( Key observer, Key observed, ObserverMethod<STATE> callback ); \
extern template void SetState( Key widget, SetStateMethod<STATE> setState ); \
extern template bool HasState<STATE>( Key widget );

DECLARE_FUNCTION_TEMPLATES( WidgetState );
DECLARE_FUNCTION_TEMPLATES( TextState );
DECLARE_FUNCTION_TEMPLATES( Transform );
DECLARE_FUNCTION_TEMPLATES( VisibleChildren );
DECLARE_FUNCTION_TEMPLATES( OnBuildLayout );

Key CreateWidget( std::string const& tag,
	InitStateMethod initMethod,
	BuildLayoutMethod build,
	RenderWidgetMethod renderWidget,
	HitTestMethod hitTest,
	std::initializer_list<Key> children
);

Key CreateWidget( std::string const& tag,
	InitStateMethod initMethod,
	BuildLayoutMethod build,
	RenderWidgetMethod renderWidget,
	HitTestMethod hitTest,
	Custom customisation,
	std::initializer_list<Key> children
);


bool TrueOfAny( Key root, WidgetPredicate predicate );

Key CreateChildWidget( Key parent, Key child );

Layouts::Rect GetWidgetRect( Key widget );

Key GetParentWidget( Key child, unsigned int generation = 0 );

std::vector<Key> const& GetChildWidgets( Key parent );

std::vector<Key> const& GetSiblingWidgets( Key child );

void InitWidgetTree( Key childNode, Key parentNode );

Layouts::LayoutBuilder BuildLayoutTree( Key root );

void RenderLayoutTree( Key root, int x_adjust = 0, int y_adjust = 0 );

void SetRebuildLayoutTree();

bool ShouldRebuildLayoutTree();

void RebuildLayoutTree( Key root );

void FlushCallbacks();

// begin at the layout tree node corresponding to the given key 'widget'
void RunHitTests( Key widget, int x, int y, std::vector<Key>& hitTree );

void RunHitTests( Key root, int x, int y );

std::vector<Key> const& GetHitTree();

void ClearHitTree();

/////////////////////
// utility methods //
/////////////////////

template<typename Container, typename Value>
bool contains( Container const& container, Value v )
{
	return std::find( container.cbegin(), container.cend(), v ) != container.cend();
}

#endif