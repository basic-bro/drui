// LayoutBuilder.cpp

#include<cassert>
#include<iostream>

#include "Layouts.hpp"

#include<Console/Console.hpp>

namespace Layouts
{

  /////////////////////////
  // class LayoutBuilder //
  /////////////////////////

  LayoutBuilder::LayoutBuilder( Intervals::IntervalBuilder widthBuilder, Intervals::IntervalBuilder heightBuilder )
    : m_WidthBuilder{ widthBuilder }, m_HeightBuilder{ heightBuilder }
    { }

  void LayoutBuilder::SetKey( Intervals::Key key )
  {
    m_WidthBuilder.SetKey( key );
    m_HeightBuilder.SetKey( key );
  }

  Intervals::Key LayoutBuilder::GetKey() const
  {
    auto key = m_WidthBuilder.GetKey();
    assert( key == m_HeightBuilder.GetKey() );
    return m_WidthBuilder.GetKey();
  }

  void LayoutBuilder::AddChild( LayoutBuilder child )
  {
    m_WidthBuilder.AddChild( child.m_WidthBuilder );
    m_HeightBuilder.AddChild( child.m_HeightBuilder );
  }

  void LayoutBuilder::DimChildren()
  {
    m_WidthBuilder.DimChildren();
    m_HeightBuilder.DimChildren();
  }

  void LayoutBuilder::PosChildren( int x, int y )
  {
    m_WidthBuilder.SetPosition( x );
    m_WidthBuilder.PosChildren();
    m_HeightBuilder.SetPosition( y );
    m_HeightBuilder.PosChildren();
  }
    
  Rect LayoutBuilder::GetRect( int x_adjust, int y_adjust ) const
  {
    auto width = m_WidthBuilder.GetInterval();
    auto height = m_HeightBuilder.GetInterval();
    return Rect
    {
      .x = width.position + x_adjust,
      .y = height.position + y_adjust,
      .w = width.extent,
      .h = height.extent
    };
  }

  Intervals::IntervalBuilder const& LayoutBuilder::GetWidthBuilder() const
  {
    return m_WidthBuilder;
  }

  Intervals::IntervalBuilder const& LayoutBuilder::GetHeightBuilder() const
  {
    return m_HeightBuilder;
  }

  void LayoutBuilder::DebugPrint() const
  {
    Console::Print( "\nLayoutBuilder widths:" );
    m_WidthBuilder.DebugPrint( 0 );
    Console::Print( "\n\nLayoutBuilder heights:" );
    m_HeightBuilder.DebugPrint( 0 );
  }


  //////////////////
  // WidthRequest //
  //////////////////

  WidthRequest::operator Intervals::ExtentRequest() const
  {
    return Intervals::ExtentRequest
    {
      .requestType = requestType,
      .request = request
    };
  }

  ///////////////////
  // HeightRequest //
  ///////////////////

  HeightRequest::operator Intervals::ExtentRequest() const
  {
    return Intervals::ExtentRequest
    {
      .requestType = requestType,
      .request = request
    };
  }

  /////////////////////
  // Convenience API //
  /////////////////////


  LayoutBuilder Scaffold( int width, int height, Intervals::Notifiers widthNotifiers, Intervals::Notifiers heightNotifiers )
  {
    return LayoutBuilder
    {
      Intervals::SingleChildIntervalBuilder( WidthExactly( width ), widthNotifiers ),
      Intervals::SingleChildIntervalBuilder( HeightExactly( height ), heightNotifiers )
    };
  }

  LayoutBuilder Box( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers, Intervals::Notifiers heightNotifiers )
  {
    return LayoutBuilder
    {
      Intervals::SingleChildIntervalBuilder( wr, widthNotifiers ),
      Intervals::SingleChildIntervalBuilder( hr, heightNotifiers )
    };
  }

  LayoutBuilder Row( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers, Intervals::Notifiers heightNotifiers )
  {
    return LayoutBuilder
    {
      Intervals::RowWidthIntervalBuilder( wr, widthNotifiers ),
      Intervals::RowHeightIntervalBuilder( hr, heightNotifiers )
    };
  }

  LayoutBuilder Column( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers, Intervals::Notifiers heightNotifiers )
  {
    return LayoutBuilder
    {
      Intervals::ColumnWidthIntervalBuilder( wr, widthNotifiers ),
      Intervals::ColumnHeightIntervalBuilder( hr, heightNotifiers )
    };
  }

  LayoutBuilder VerticalScrollView( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers, Intervals::Notifiers heightNotifiers )
  {
    return LayoutBuilder
    {
      Intervals::ColumnWidthIntervalBuilder( wr, widthNotifiers ),
      Intervals::ScrollHeightIntervalBuilder( hr, heightNotifiers )
    };
  }

  LayoutBuilder HSpace()
  {
    return LayoutBuilder
    {
      Intervals::ChildlessIntervalBuilder( AutoWidth ),
      Intervals::ChildlessIntervalBuilder( AutoHeight )
    };
  }

  LayoutBuilder AlignLeft( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers, Intervals::Notifiers heightNotifiers )
  {
    return Row( wr, hr, widthNotifiers, heightNotifiers );
  }

  LayoutBuilder AlignRight( WidthRequest wr, HeightRequest hr, Intervals::Notifiers widthNotifiers, Intervals::Notifiers heightNotifiers )
  {
    return AlignLeft( wr, hr, widthNotifiers, heightNotifiers );
  }

  LayoutBuilder Padding( WidthRequest wr, HeightRequest hr, int left, int right, int top, int bottom )
  {
    return LayoutBuilder
    {
      Intervals::InsetIntervalBuilder( wr, left, right ),
      Intervals::InsetIntervalBuilder( hr, top, bottom )
    };
  }

} // namespace Layouts