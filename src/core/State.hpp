// State.hpp

#ifndef STATE_HPP_INCLUDED
#define STATE_HPP_INCLUDED

#include "rgba32.hpp"

#include<string>

struct WidgetState
{
	static constexpr auto AsString = "WidgetState";
	bool isHovered = false;
	bool isVisible = true;
  bool isInHitTree = false;
};

// when a widget has a different origin to its parent
struct Transform
{
  static constexpr auto AsString = "Transform";

  // parent's origin relative to mine
  int x = 0;
  int y = 0;
};

struct VisibleChildren
{
  static constexpr auto AsString = "VisibleChildren";
  
  std::vector<Key> children;
};

struct TextState
{
	static constexpr auto AsString = "TextState";
	int width;
};

struct OnBuildLayout
{
  static constexpr auto AsString = "OnBuildLayout";
};

#endif