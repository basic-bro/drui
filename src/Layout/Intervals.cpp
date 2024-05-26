// Interval.cpp

#include "Intervals.hpp"

#include "distrib.hpp"

#include<stdexcept>
#include<cassert>

#include<Console/Console.hpp>

namespace Intervals
{

  IntervalBuilder::IntervalBuilder( IntervalProps props )
    : m_ExtentRequest{ props.extentRequest },
      m_DimChildren{ props.dimChildren },
      m_PosChildren{ props.posChildren },
      m_DeduceExtent{ props.deduceExtent },
      m_ExtentNotifier{ props.extentNotifier }
  { }


  Key IntervalBuilder::GetKey() const
  {
    return m_Key;
  }

  void IntervalBuilder::SetKey( Key key )
  {
    m_Key = key;
  }

  bool IntervalBuilder::IsFlexible() const
  {
    return m_ExtentRequest.requestType == RequestType::AtLeast;
  }

  int IntervalBuilder::GetRequestedExtent() const
  {
    assert( m_ExtentRequest.request.index() == 0 );
    return std::get<0>( m_ExtentRequest.request );
  }

  int IntervalBuilder::GetActualExtent() const
  {
    return m_ActualExtent;
  }

  void IntervalBuilder::SetActualExtent( int actualExtent )
  {
    if( GetActualExtent() != -1 )
    {
      Console::Print( "\nNote: m_ActualExtent about to be overriden. Was {}, changing to {}.", GetActualExtent(), actualExtent );
    }
    m_ActualExtent = actualExtent;

    // notify
    if( m_ExtentNotifier )
    {
      m_ExtentNotifier( m_ActualExtent );
    }
  }

  int IntervalBuilder::GetPosition() const
  {
    return m_Position;
  }

  void IntervalBuilder::SetPosition( int position )
  {
    if( GetPosition() != -1 )
    {
      Console::Print( "\nNote: m_Position about to be overriden. Was {}, changing to {}.", GetPosition(), position );
    }
    m_Position = position;
  }

  void IntervalBuilder::ForEachChild( std::function< void( IntervalBuilder& child ) > f )
  {
    for( auto& child : m_Children )
    {
      f( child );
    }
  }

  void IntervalBuilder::ForEachChild( std::function< void( IntervalBuilder const& child ) > f ) const
  {
    for( auto const& child : m_Children )
    {
      f( child );
    }
  }

  Interval IntervalBuilder::GetInterval() const
  {
    return Interval
    {
      .position = GetPosition(),
      .extent = GetActualExtent()
    };
  }

  int IntervalBuilder::DeduceActualExtent()
  {
    // best-case scenario: I'm requesting an actual extent
    if( m_ExtentRequest.requestType == RequestType::Actual && m_ExtentRequest.request.index() == 0 )
    {
      return std::get<0>( m_ExtentRequest.request );
    }

    // fallback: deduce from my children
    if( m_DeduceExtent )
    {
      auto extent = m_DeduceExtent( *this );
      if( extent >= 0 )
      {
        m_ExtentRequest = RequestExactly( extent );
      }
      return extent;
    }

    // error
    return -1;
  }

  void IntervalBuilder::DimChildren()
  {
    // I 'should' already have an actualExtent,
    // but just in case I don't, try to deduce it
    if( GetActualExtent() == -1 )
    {
      auto extent = DeduceActualExtent();
      if( extent >= 0 )
      {
        SetActualExtent( extent );
        // Console::Print( "\nNote: Deduced actual extent: {}", GetActualExtent() );
      }
      else
      {
        throw std::runtime_error{ "Could not deduce extent." };
      }
    }

    // early exit: no children
    if( m_Children.empty() )
    {
      return;
    }

    // Prepare children for partitioning
    for( auto& child : m_Children )
    {
      // convert proportional requests
      // using my actualExtent
      child.CalcRequest( GetActualExtent() );    
    }

    // A parent only partitions their own children
    // (not, e.g., their children's children)
    if( m_DimChildren )
    {
      m_DimChildren( *this );
    }

    // continue to walk the tree
    for( auto& child : m_Children )
    {
      child.DimChildren();
    }
  }

  void IntervalBuilder::PosChildren()
  {
    if( GetPosition() == -1 )
    {
      throw std::runtime_error{ "Cannot position children if I myself have no position!" };
    }

    if( m_PosChildren )
    {
      m_PosChildren( *this );
    }

    for( auto& child : m_Children )
    {
      child.PosChildren();
    }
  }
                
  void IntervalBuilder::CalcRequest( int parentExtent )
  {
    if( m_ExtentRequest.request.index() == 1 )
    {
      m_ExtentRequest.request = static_cast<int>( parentExtent * std::get<1>( m_ExtentRequest.request ) );
      assert( m_ExtentRequest.request.index() == 0 );
    }
  }

  int IntervalBuilder::TotalRequestedByChildren() const
  {
    int total = 0;
    for( auto const& child : m_Children )
    {
      total += child.GetRequestedExtent();
    }
    return total;
  }

  int IntervalBuilder::CountFlexibleChildren() const
  {
    auto count = 0;
    for( auto const& child : m_Children )
    {
      if( child.IsFlexible() )
      {
        ++count;
      }
    }
    return count;
  }

  void IntervalBuilder::AddChild( IntervalBuilder child )
  {
    m_Children.push_back( child );
  }

  IntervalBuilder IntervalBuilder::WithoutChildren() const
  {
    auto p = IntervalBuilder
    {
      IntervalProps
      {
        m_ExtentRequest, m_DimChildren,
        m_PosChildren, m_DeduceExtent, m_ExtentNotifier
      }
    };
    p.SetKey( GetKey() );
    p.m_ActualExtent = GetActualExtent();   // avoid calling notifiers needlessly
    p.m_Position = GetPosition();           // avoid calling notifiers needlessly
    return p;
  }

  std::vector<IntervalBuilder> const& IntervalBuilder::GetChildren() const
  {
    return m_Children;
  }

  void IntervalBuilder::DebugPrint( int depth ) const
  {
    std::string indent( depth, ' ' );
    Console::Print( "\n{} [{}] extent {} position {}", indent, m_Key, GetActualExtent(), GetPosition() );
    for( auto const& child : m_Children )
    {
      child.DebugPrint( depth + 1 );
    }
  }

  //////////////////////////
  // Dimensioning Methods //
  //////////////////////////

  void DimChildrenCopyParent( IntervalBuilder& parent )
  {
    parent.ForEachChild(
      [&] ( IntervalBuilder& child )
      {
        auto actualExtent = parent.GetActualExtent();

        // all visible children must be flexible for this policy,
        // (unless parent and child agree, in which case it doesn't matter)
        if( !child.IsFlexible() && child.GetRequestedExtent() > 0 && actualExtent != child.GetRequestedExtent() )
        {
          Console::Print( "\nNote: Inflexible child being overriden by parent. Child wants {}, parent will give {}.",
            child.GetRequestedExtent(),
            actualExtent
          );
        }

        // set child extent
        child.SetActualExtent( actualExtent );
      }
    );
  }

  void DimChildrenEvenly( IntervalBuilder& parent )
  {
    int leftover = parent.GetActualExtent() - parent.TotalRequestedByChildren();
    int adjustable_count = parent.CountFlexibleChildren();
    if( adjustable_count == 0 )
    {
      adjustable_count = 1;
    }
        
    if( leftover ) {
      // set definite widths
      // 'distrib' is reversed because I want to pop elements
      // from it as I go, but std::vector only has pop_back()
      auto distrib = distrib_middle( adjustable_count, leftover );
      std::reverse( distrib.begin(), distrib.end() );
      parent.ForEachChild(
        [ &distrib ] ( IntervalBuilder& child )
        {
          // inflexible children get what they asked for
          if( !child.IsFlexible() )
          {
            child.SetActualExtent( child.GetRequestedExtent() );
            return;
          }

          // flexible children get more than what they asked for
          child.SetActualExtent( child.GetRequestedExtent() + distrib.back() );
          distrib.pop_back();
        }
      );
    }
  }

  DimChildrenMethod DimChildrenInset( int before, int after )
  {
    return [=] ( IntervalBuilder& parent )
    {
      parent.ForEachChild(
        [=] ( IntervalBuilder& child )
        {
          child.SetActualExtent( parent.GetActualExtent() - before - after );
        }
      );
    };
  }

  //////////////////////
  // Position Methods //
  /////////////////////

  void PosChildrenCopyParent( IntervalBuilder& parent )
  {
    parent.ForEachChild(
      [&] ( IntervalBuilder& child ) {
        child.SetPosition( parent.GetPosition() );
      }
    );
  }

  // when the childrens' coordinate space is different from the parent's
  // e.g. the coordinate space of a scroll view is internally different from 
  // the surrounding layouts in the tree
  PosChildrenMethod PosChildrenSequentiallyStartingAt( int start )
  {
    return [=] ( IntervalBuilder& parent )
    {
      int pos = start;
      parent.ForEachChild
      (
        [&] ( IntervalBuilder& child )
        {
          child.SetPosition( pos );
          pos += child.GetActualExtent();
        }
      );
    };
  }


  PosChildrenMethod PosChildrenSequentiallyWithInitialOffsetFromParent( int offsetFromParent )
  {
    return [=] ( IntervalBuilder& parent )
    {
      int pos = parent.GetPosition() + offsetFromParent;
      parent.ForEachChild
      (
        [&] ( IntervalBuilder& child )
        {
          child.SetPosition( pos );
          pos += child.GetActualExtent();
        }
      );
    };
  }

  PosChildrenMethod PosChildrenAllAt( int position )
  {
    return [ position ] ( IntervalBuilder& parent )
    {
      parent.ForEachChild
      (
        [ position ] ( IntervalBuilder& child )
        {
          child.SetPosition( position );
        }
      );
    };
  }

  ///////////////////////
  // Deduction Methods //
  ///////////////////////

  int DeduceExtentSumChildren( IntervalBuilder& parent )
  {
    auto totalExtent = 0;
    parent.ForEachChild(
      [&] ( IntervalBuilder& child )
      {
        auto extent = child.DeduceActualExtent();
        if( extent == -1 )
        {
          totalExtent = -1;
          return;
        }
        totalExtent += extent;
      }
    );
    return totalExtent;
  }

  int DeduceExtentMaxChild( IntervalBuilder& parent )
  {
    auto max = 0;
    parent.ForEachChild(
      [&] ( IntervalBuilder& child )
      {
        auto extent = child.DeduceActualExtent();
        if( extent > max )
        {
          max = extent;
        }
      }
    );
    return max;
  }

  DeduceExtentMethod DeduceExtentWithInset( DeduceExtentMethod deduceExtent, int before, int after )
  {
    return [=] ( IntervalBuilder& parent )
    {
      return deduceExtent( parent ) + before + after;
    };
  }

  /////////////////////
  // Convenience API //
  /////////////////////

  IntervalBuilder ChildlessIntervalBuilder( ExtentRequest req, Notifiers notifiers )
  {
    return IntervalBuilder
    { 
      IntervalProps
      {
        .extentRequest = req,
        .extentNotifier = notifiers.extentNotifier
      }
    };
  }

  IntervalBuilder SingleChildIntervalBuilder( ExtentRequest req, Notifiers notifiers )
  {
    return IntervalBuilder
    {
      IntervalProps
      {
        .extentRequest = req,
        .dimChildren= DimChildrenCopyParent,
        .posChildren = PosChildrenCopyParent,
        .deduceExtent = DeduceExtentSumChildren,
        .extentNotifier = notifiers.extentNotifier
      }
    };
  }

  IntervalBuilder InsetIntervalBuilder( ExtentRequest req, int before, int after, Notifiers notifiers )
  {
    return IntervalBuilder
    {
      IntervalProps
      {
        .extentRequest = req,
        .dimChildren = DimChildrenInset( before, after ),
        .posChildren = PosChildrenSequentiallyWithInitialOffsetFromParent( before ),
        .deduceExtent = DeduceExtentWithInset( DeduceExtentSumChildren, before, after ),
        .extentNotifier = notifiers.extentNotifier
      }
    };
  }

  IntervalBuilder RowWidthIntervalBuilder( ExtentRequest req, Notifiers notifiers )
  {
    return IntervalBuilder
    {
      IntervalProps
      {
        .extentRequest = req,
        .dimChildren = DimChildrenEvenly,
        .posChildren = PosChildrenSequentiallyWithInitialOffsetFromParent( 0 ),
        .deduceExtent = DeduceExtentSumChildren,
        .extentNotifier = notifiers.extentNotifier
      }
    };
  }

  IntervalBuilder RowHeightIntervalBuilder( ExtentRequest req, Notifiers notifiers )
  {
    return IntervalBuilder
    {
      IntervalProps
      {
        .extentRequest = req,
        .dimChildren = DimChildrenCopyParent,
        .posChildren = PosChildrenCopyParent,
        .deduceExtent = DeduceExtentMaxChild,
        .extentNotifier = notifiers.extentNotifier
      }
    };
  }

  IntervalBuilder ColumnWidthIntervalBuilder( ExtentRequest req, Notifiers notifiers )
  {
    return RowHeightIntervalBuilder( req, notifiers );
  }

  IntervalBuilder ColumnHeightIntervalBuilder( ExtentRequest req, Notifiers notifiers )
  {
    return RowWidthIntervalBuilder( req, notifiers );
  }

  IntervalBuilder ScrollWidthIntervalBuilder( ExtentRequest req, Notifiers notifiers )
  {
    return IntervalBuilder
    {
      IntervalProps
      {
        .extentRequest = req,
        .posChildren = PosChildrenSequentiallyStartingAt( 0 ),
        .deduceExtent = DeduceExtentSumChildren,
        .extentNotifier = notifiers.extentNotifier
      }
    };
  }

  IntervalBuilder ScrollHeightIntervalBuilder( ExtentRequest req, Notifiers notifiers )
  {
    return IntervalBuilder
    {
      IntervalProps
      {
        .extentRequest = req,
        .posChildren = PosChildrenSequentiallyStartingAt( 0 ),
        .deduceExtent = DeduceExtentSumChildren,
        .extentNotifier = notifiers.extentNotifier
      }
    };
  }

} // namespace Intervals