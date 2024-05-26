// Database.cpp

#include<functional>
#include<map>
#include<tuple>
#include<array>
#include<vector>
#include<queue>
#include<string>
#include<set>

#include<iostream>
#include<format>

#include "core/Database.hpp"

using namespace Layouts;

////////////////////////////////////
// TEMPLATE METAPROGRAMMING STUFF //
////////////////////////////////////

// https://stackoverflow.com/questions/26169198/how-to-get-the-index-of-a-type-in-a-variadic-type-pack

template <typename T, typename ... Ts>
struct Index;

template <typename T, typename ... Ts>
struct Index<T, T, Ts ...> : std::integral_constant<std::size_t, 0> {};

template <typename T, typename U, typename... Ts>
struct Index<T, U, Ts ...> : std::integral_constant<std::size_t, 1 + Index<T, Ts ...>::value> {};

template <typename T, typename... Ts>
constexpr std::size_t IndexOf = Index<T, Ts...>::value;


////////////////////
// DATABASE STUFF //
////////////////////

template<typename State>
using ObserverMethod = std::function< void( Key observer, State const& )>;

template<typename State>
class ObserverRegistry
{
public:
	struct ObserverEntry
	{
		Key observer;
		ObserverMethod<State> callback;
	};

private:
	std::map<Key, std::vector<ObserverEntry>> m_Registry;

public:
	template<typename State>
	void Add( Key observer, Key observed, ObserverMethod<State> callback )
	{
		m_Registry[ observed ].push_back( ObserverEntry{ .observer = observer, .callback = callback } );
	}

	bool Exists( Key observed ) const
	{
		return m_Registry.contains( observed );
	}

	std::vector<ObserverEntry> const& Get( Key observed ) const
	{
		return m_Registry.at( observed );
	}
};

template<typename State>
using SetStateMethod  = std::function< void( State& ) >;

using WidgetPredicate = std::function< bool( Key ) >;

template<typename ... States>
class Database
{
private:
	using PureMethod = std::function< void() >;

private:
	// set during app (incl. widget) initialisation
	// doesn't change thereafter (ideally?)
	// not accessed by widgets or their lambdas
	std::map<Key, Widget> m_WidgetRegistry;
	std::map<std::string, Key> m_TagRegistry;
	std::tuple<ObserverRegistry<States>...> m_ObserverRegistries;
	
	// changes depending on user interaction
	// only m_States can be accessed by widgets and their lambdas
	// (via 'this')
	std::tuple<std::map<Key, States>...> m_States;
	bool m_RebuildLayout = true;
	std::vector<Key> m_HitTree;

	// internal data, not accessed by widgets or their lambdas
	std::set<Key> m_Dirty;
	std::array<std::set<Key>, sizeof...( States ) > m_DirtyByState;
	std::queue<PureMethod> m_Callbacks;
	
public:
	Key CreateWidget(
		std::string const& tag,
		InitStateMethod initMethod,
		BuildLayoutMethod build,
		HitTestMethod hitTest,
		RenderWidgetMethod renderWidget,
		std::initializer_list<Key> children )
	{
		auto key = NewKey();
		m_WidgetRegistry.insert( { key,
			Widget
			{
				.tag = tag,
				.self = key,
				.initState = initMethod,
				.buildLayout = build,
				.runHitTest = hitTest,
				.renderWidget = renderWidget,
				.children = children
			}
		});

		if( tag.size() )
		{
			m_TagRegistry.insert( { tag, key } );
		}
		return key;
	}

	Key CreateChildWidget( Key parent, Key child )
	{
		m_WidgetRegistry.at( parent ).children.push_back( child );
		return child;
	}

	void InitWidgetTree( Key childNode, Key parentNode )
	{
		auto& child =  m_WidgetRegistry.at( childNode );
		child.parent = parentNode;
		child.initState( childNode );
		for( auto const widget : child.children )
		{
			InitWidgetTree( widget, childNode );
		}
	}

	LayoutBuilder BuildLayoutTree( Key root )
	{
		auto const& widget = m_WidgetRegistry.at( root );
		auto layout = widget.buildLayout( root );
		layout.SetKey( root );
		for( auto const child : widget.children )
		{
			layout.AddChild( BuildLayoutTree( child ) );
		}
		return layout;
	}

	void RenderLayoutTree( Key root, int x_adjust, int y_adjust )
	{
		// init
		auto const& widget = m_WidgetRegistry.at( root );
		auto rect = widget.layout.GetRect( x_adjust, y_adjust );

		// early return: widget is invisible
		if( rect.w == 0 || rect.h == 0 )
		{
			return;
		}

		// proceed with render
		auto renderChildren = widget.renderWidget( root, rect );

		// early exit: not rendering children
		if( !renderChildren )
		{
			return; 
		}

		// render children
		for( auto const child : widget.children )
		{
			RenderLayoutTree( child, x_adjust, y_adjust );
		}
	}

	void SetRebuildLayoutTree()
	{
		m_RebuildLayout = true;
	}

	bool ShouldRebuildLayoutTree() const
	{
		return m_RebuildLayout;
	}

	void RebuildLayoutTree( Key root )
	{
		if( !m_RebuildLayout )
		{
			throw std::runtime_error{ "RebuildLayoutTree() called when m_RebuildLayout is false." };
		}

		// rebuild tree
		auto tree = BuildLayoutTree( root );
		tree.DimChildren();
		tree.PosChildren( 0, 0 );
		// tree.DebugPrint();

		// push changes to widget tree
		pushLayouts( tree.GetWidthBuilder(), tree.GetHeightBuilder() );

		FlushCallbacks();
		m_RebuildLayout = false;
	}

	std::vector<Key> const& GetHitTree() const
	{
		return m_HitTree;
	}

	void ClearHitTree()
	{
		m_HitTree.clear();
	}

	bool TrueOfAny( Key root, WidgetPredicate predicate )
	{
		if( predicate( root ) )
		{
			return true;
		}

		for( auto const& child : GetChildWidgets( root ) )
		{
			if( TrueOfAny( child, predicate ) )
			{
				return true;
			}
		}
		return false;
	}

	void RunHitTests( Key root, int x, int y )
	{
		runHitTests( root, x, y, m_HitTree );
	}

	// begin at the layout tree node corresponding to the given key 'widget'
	void RunHitTests( Key widget, int x, int y, std::vector<Key>& hitTree )
	{
		runHitTests( widget, x, y, hitTree );
	}

	Rect GetRect( Key widget )
	{
		if( m_WidgetRegistry.contains( widget ) )
		{
			return m_WidgetRegistry.at( widget ).layout.GetRect();
		}
		return Rect{};
	}

	Key GetParentWidget( Key child, unsigned int nest )
	{
		auto parent = m_WidgetRegistry.at( child ).parent;
		if( nest == 0 )
		{
			return parent;
		}
		return GetParentWidget( parent, nest - 1 );
	}

	std::vector<Key> const& GetSiblingWidgets( Key child )
	{
		return GetChildWidgets( GetParentWidget( child, 0 ) );
	}

	std::vector<Key> const& GetChildWidgets( Key parent )
	{
		return m_WidgetRegistry.at( parent ).children;
	}

	template<typename State>
	void CreateState( Key widget, State state, bool markAsDirty = false )
	{
		std::get<std::map<Key, State>>( m_States ).insert( { widget, state } );
		if( markAsDirty )
		{
			m_Dirty.insert( widget );
			m_DirtyByState.at( IndexOf<State, States...> ).insert( widget );
		}
	}

	template<typename State>
	State const& GetState( Key widget ) const
	{
		if( !HasState<State>( widget ) )
		{
			throw std::runtime_error{ std::format( "Widget {} does not have {}", widget, State::AsString ) };
		}
		return std::get<std::map<Key, State>>( m_States ).at( widget );
	}

	template<typename State>
	State const& ObserveState( Key observer, Key observed, ObserverMethod<State> callback )
	{
		std::get<ObserverRegistry<State>>( m_ObserverRegistries ).Add( observer, observed, callback );
		return GetState<State>( observed );
	}

	template<typename State>
	State const& ObserveState( Key observer, std::string const& tag, ObserverMethod<State> callback )
	{
		// throw on unknown tag
		if( !m_TagRegistry.contains( tag ) )
		{
			throw std::runtime_error{ std::format( "Unknown tag: '{}'", tag ) };
		}
		Key observed = m_TagRegistry.at( tag );
		std::get<ObserverRegistry<State>>( m_ObserverRegistries ).Add( observer, observed, callback );
		return GetState<State>( observed );
	}

	template<typename State>
	void SetState( Key widget, SetStateMethod<State> setState )
	{
		m_Callbacks.push( [=] () { setState( getState<State>( widget ) ); } );
		addDirty<State>( widget );
	}

	template<typename State>
	bool HasState( Key widget ) const
	{
		return std::get<std::map<Key, State>>( m_States ).contains( widget );
	}

	void FlushCallbacks()
	{
		// Console::Print( "\nFlush" );
		// int iteration = 0;
		do
		{
			// Console::Print( "\n\n\nIteration {}, {} dirty widgets, {} callbacks.", iteration, m_Dirty.size(), m_Callbacks.size() );

			// schedule dirty widgets to be observed
			for( auto widget : m_Dirty )
			{
				( runObservers<States>( widget ), ... );
			}
			m_Dirty.clear();

			// Console::Print( "\n\n\t{} callbacks.", m_Callbacks.size() );
		
			// drain the callbacks
			while( !m_Callbacks.empty() )
			{
				m_Callbacks.front()();
				m_Callbacks.pop();
			}
			// iteration++;
		} while( m_Dirty.size() );
	}

private:
	template<typename State>
	State& getState( Key widget )
	{
		if( !HasState<State>( widget ) )
		{
			throw std::runtime_error{ std::format( "Widget {} does not have {}", widget, State::AsString ) };
		}
		return std::get<std::map<Key, State>>( m_States ).at( widget );
	}

	template<typename State>
	void addDirty( Key widget )
	{
		if( std::get<ObserverRegistry<State>>( m_ObserverRegistries ).Exists( widget ) )
		{
			m_Dirty.insert( widget );
			m_DirtyByState.at( IndexOf<State, States...> ).insert( widget );
		}
	}

	template<typename State>
	void runObservers( Key widget )
	{
		auto const& registry = std::get<ObserverRegistry<State>>( m_ObserverRegistries );
		if( !registry.Exists( widget ) )
		{
			return;
		}
		
		auto const& entries = registry.Get( widget );
		// Console::Print( "\n\t{}, widget {}, {} observers.", State::AsString, widget, entries.size() );
		for( auto const& entry : entries )
		{
			m_Callbacks.push( [ &, widget ] () { entry.callback( entry.observer, GetState<State>( widget ) ); } );
		}
		m_DirtyByState.at( IndexOf<State, States...> ).clear();
	}

	void pushLayouts( Intervals::IntervalBuilder const& widthBuilder, Intervals::IntervalBuilder const& heightBuilder )
	{
		LayoutBuilder layout{ widthBuilder.WithoutChildren(), heightBuilder.WithoutChildren() };
		m_WidgetRegistry.at( layout.GetKey() ).layout = layout;

		auto const& widthChildren = widthBuilder.GetChildren();
		auto const& heightChildren = heightBuilder.GetChildren();

		for( auto i = 0; i < widthChildren.size(); ++i )
		{
			pushLayouts( widthChildren.at( i ), heightChildren.at( i ) );
		}
	}

	// begin at the given node in the layout tree
	void runHitTests( Key tgt, int x, int y, std::vector<Key>& hitTree )
	{
		// early return: hit target does not have a WidgetState
		if( !HasState<WidgetState>( tgt ) )
		{
			return;
		}

		// test current layout
		auto const& widget = m_WidgetRegistry.at( tgt );
		auto runChildren = widget.runHitTest( widget.layout, x, y, hitTree );
			
		// test children?
		if( !runChildren )
		{
			return;
		}

		// test children: exit on first child to add to the hit tree
		auto numHits = hitTree.size();
		for( auto const child : widget.children )
		{
			runHitTests( child, x, y, hitTree );
			if( hitTree.size() != numHits )
			{
				return;
			}
		}
	}
};

using AppDatabase = Database<
	#include "core/Config.def"
	#include "user/Config.def"
>;

static AppDatabase db;

HitTestMethod DefaultHitTest = 
	[] ( LayoutBuilder const& l, int x, int y, std::vector<Key>& hitTree ) -> bool
	{
		auto r = l.GetRect();
		if( r.x <= x && x < r.x + r.w
		 && r.y <= y && y < r.y + r.h )
		{
			hitTree.push_back( l.GetKey() );
		}
		return true;
	};

HitTestMethod RefuseHitTest =
	[] ( LayoutBuilder const&, int, int, std::vector<Key>& ) -> bool
	{
		return false;
	};

void InitWidgetTree( Key childNode, Key parentNode )
{
	db.InitWidgetTree( childNode, parentNode );
}

LayoutBuilder BuildLayoutTree( Key root )
{
	return db.BuildLayoutTree( root );
}

void RenderLayoutTree( Key root, int x_adjust, int y_adjust )
{
	db.RenderLayoutTree( root, x_adjust, y_adjust );
}

void SetRebuildLayoutTree()
{
	db.SetRebuildLayoutTree();
}

bool ShouldRebuildLayoutTree()
{
	return db.ShouldRebuildLayoutTree();
}

void RebuildLayoutTree( Key root )
{
	db.RebuildLayoutTree( root );
}

void FlushCallbacks()
{
	db.FlushCallbacks();
}

void RunHitTests( Key root, int x, int y )
{
	db.RunHitTests( root, x, y );
}

void RunHitTests( Key widget, int x, int y, std::vector<Key>& hitTree )
{
	db.RunHitTests( widget, x, y, hitTree );
}

Rect GetWidgetRect( Key widget )
{
	return db.GetRect( widget );
}

Key CreateWidget( std::string const& tag,
	InitStateMethod initMethod,
	BuildLayoutMethod build,
	RenderWidgetMethod renderWidget,
	HitTestMethod hitTest,
	std::initializer_list<Key> children )
{
	return db.CreateWidget( tag, initMethod, build, hitTest, renderWidget, children );
}

Key CreateWidget( std::string const& tag,
	InitStateMethod initMethod,
	BuildLayoutMethod build,
	RenderWidgetMethod renderWidget,
	HitTestMethod hitTest,
	Custom custom,
	std::initializer_list<Key> children )
{
	return CreateWidget( tag,

		// initState
		// customisation allows composition of original initState
		// with user-provided one
		custom.extraInitState
			? [ initMethod, custom ] ( Key widget )
				{
					initMethod( widget );
					custom.extraInitState( widget );
				}
			: initMethod,

		// buildLayout
		// customisation allows to override the original buildLayout
		custom.overrideBuildLayout
			? custom.overrideBuildLayout
			: build,

		// renderWidget
		// customisation allows to override the original renderWidget
		custom.overrideRenderWidget
			? custom.overrideRenderWidget
			: renderWidget,

		// hitTest
		// customisation allows to override the original hitTest
		custom.overrideHitTest
			? custom.overrideHitTest
			: hitTest,

		// children
		children
	);
}

bool TrueOfAny( Key root, WidgetPredicate predicate )
{
	return db.TrueOfAny( root, predicate );
}

Key CreateChildWidget( Key parent, Key child )
{
	return db.CreateChildWidget( parent, child );
}

Key GetParentWidget( Key child, unsigned int generation )
{
	return db.GetParentWidget( child, generation );
}

std::vector<Key> const& GetChildWidgets( Key parent )
{
	return db.GetChildWidgets( parent );
}

std::vector<Key> const& GetSiblingWidgets( Key child )
{
	return db.GetSiblingWidgets( child );
}

std::vector<Key> const& GetHitTree()
{
	return db.GetHitTree();
}

void ClearHitTree()
{
	db.ClearHitTree();
}

template<typename State>
void CreateState( Key widget, State s, bool markAsDirty )
{
	db.CreateState<State>( widget, s, markAsDirty );
}

template<typename State>
State const& GetState( Key widget )
{
	return db.GetState<State>( widget );
}

template<typename State>
State const& ObserveState( Key observer, Key observed, ObserverMethod<State> callback )
{
	return db.ObserveState<State>( observer, observed, callback );
}

template<typename State>
void SetState( Key widget, SetStateMethod<State> setState )
{
	db.SetState<State>( widget, setState );
}

template<typename State>
bool HasState( Key widget )
{
	return db.HasState<State>( widget );
}


#define INSTANTIATE_FUNCTION_TEMPLATES( STATE ) \
template void CreateState( Key widget, STATE s, bool markAsDirty ); \
template STATE const& GetState( Key widget ); \
template STATE const& ObserveState( Key observer, Key observed, ObserverMethod<STATE> callback ); \
template void SetState( Key widget, SetStateMethod<STATE> setState ); \
template bool HasState<STATE>( Key widget );

INSTANTIATE_FUNCTION_TEMPLATES( WidgetState );
INSTANTIATE_FUNCTION_TEMPLATES( TextState );
INSTANTIATE_FUNCTION_TEMPLATES( Transform );
INSTANTIATE_FUNCTION_TEMPLATES( VisibleChildren );
INSTANTIATE_FUNCTION_TEMPLATES( OnBuildLayout );
