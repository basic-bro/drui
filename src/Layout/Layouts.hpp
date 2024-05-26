// Layouts.hpp
// - Layouts, now with Partitions!

#ifndef LAYOUT_HPP_INCLUDED
#define LAYOUT_HPP_INCLUDED

#include "Intervals.hpp"

namespace Layouts
{

  struct Rect
  {
    int x = 0, y = 0, w = 0, h = 0;

    Rect WithTransform( int x_adjust, int y_adjust )
    {
      return Rect
      {
        .x = x + x_adjust,
        .y = y + y_adjust,
        .w = w,
        .h = h
      };
    }
  };

  //////////////////
  // class Layout //
  //////////////////

  class LayoutBuilder
  {
  private:
    Intervals::IntervalBuilder m_WidthBuilder;
    Intervals::IntervalBuilder m_HeightBuilder;

  public:
    LayoutBuilder() = default;
    LayoutBuilder( Intervals::IntervalBuilder horizontal, Intervals::IntervalBuilder vertical );

    void SetKey( Intervals::Key key );
    Intervals::Key GetKey() const;

    void AddChild( LayoutBuilder child );

    void DimChildren();
    void PosChildren( int x, int y );

    Rect GetRect( int x_adjust = 0, int y_adjust = 0 ) const;

    Intervals::IntervalBuilder const& GetWidthBuilder() const;
    Intervals::IntervalBuilder const& GetHeightBuilder() const;

    void DebugPrint() const;
  };

  //////////////////
  // WidthRequest //
  //////////////////

  struct WidthRequest
  {
    Intervals::RequestType requestType;
    std::variant<int, float> request;

    operator Intervals::ExtentRequest() const;
  };

  constexpr WidthRequest WidthExactly( int width )
  {
    return WidthRequest
    {
      .requestType = Intervals::RequestType::Actual,
      .request = width
    };
  }

  constexpr WidthRequest WidthExactly( float fractionOfParent )
  {
    return WidthRequest
    {
      .requestType = Intervals::RequestType::Actual,
      .request = fractionOfParent
    };
  }

  constexpr WidthRequest WidthAtLeast( int widthAtLeast )
  {
    return WidthRequest
    {
      .requestType = Intervals::RequestType::AtLeast,
      .request = widthAtLeast
    };
  }

  constexpr WidthRequest WidthAtLeast( float fractionOfParentAtLeast )
  {
    return WidthRequest
    {
      .requestType = Intervals::RequestType::AtLeast,
      .request = fractionOfParentAtLeast
    };
  }

  ///////////////////
  // HeightRequest //
  ///////////////////

  struct HeightRequest
  {
    Intervals::RequestType requestType;
    std::variant<int, float> request;

    operator Intervals::ExtentRequest() const;
  };

  constexpr HeightRequest HeightExactly( int height )
  {
    return HeightRequest
    {
      .requestType = Intervals::RequestType::Actual,
      .request = height
    };
  }

  constexpr HeightRequest HeightExactly( float fractionOfParent )
  {
    return HeightRequest
    {
      .requestType = Intervals::RequestType::Actual,
      .request = fractionOfParent
    };
  }

  constexpr HeightRequest HeightAtLeast( int heightAtLeast )
  {
    return HeightRequest
    {
      .requestType = Intervals::RequestType::AtLeast,
      .request = heightAtLeast
    };
  }

  constexpr HeightRequest HeightAtLeast( float fractionOfParentAtLeast )
  {
    return HeightRequest
    {
      .requestType = Intervals::RequestType::AtLeast,
      .request = fractionOfParentAtLeast
    };
  }

  ///////////////////////////
  // AutoWidth, AutoHeight //
  ///////////////////////////

  constexpr auto AutoWidth  = WidthAtLeast( 1 );
  constexpr auto AutoHeight = HeightAtLeast( 1 );

  /////////////////////
  // Convenience API //
  /////////////////////

  LayoutBuilder Scaffold( int width, int height, Intervals::Notifiers widthNotifiers = {}, Intervals::Notifiers = {} );

  LayoutBuilder Box( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers = {}, Intervals::Notifiers heightNotifiers = {} );

  LayoutBuilder Row( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers = {}, Intervals::Notifiers heightNotifiers = {} );

  LayoutBuilder Column( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers = {}, Intervals::Notifiers heightNotifiers = {} );

  LayoutBuilder VerticalScrollView( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers = {}, Intervals::Notifiers heightNotifiers = {} );
    
  LayoutBuilder HSpace();

  LayoutBuilder AlignLeft( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers = {}, Intervals::Notifiers heightNotifiers = {} );

  LayoutBuilder AlignRight( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers = {}, Intervals::Notifiers heightNotifiers = {} );
  
  LayoutBuilder Padding( WidthRequest wr, HeightRequest hr, int left = 0, int right = 0, int top = 0, int bottom = 0 );

} // namespace Layouts

#endif