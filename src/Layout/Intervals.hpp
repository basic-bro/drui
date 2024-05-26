// Intervals.hpp

#ifndef INTERVALS_HPP_INCLUDED
#define INTERVALS_HPP_INCLUDED

#include<variant>
#include<vector>
#include<functional>

namespace Intervals
{
  using Key = uint64_t;

  enum class RequestType
  {
    Actual, AtLeast
  };
  
  struct ExtentRequest
  {
    RequestType requestType;
    std::variant<int, float> request;
  };

  inline ExtentRequest RequestExactly( int extent )
  {
    return ExtentRequest
    {
      .requestType = RequestType::Actual,
      .request = extent
    };
  }

  inline ExtentRequest RequestExactly( float fractionOfParent )
  {
    return ExtentRequest
    {
      .requestType = RequestType::Actual,
      .request = fractionOfParent
    };
  }

  inline ExtentRequest RequestAtLeast( int extent )
  {
    return ExtentRequest
    {
      .requestType = RequestType::AtLeast,
      .request = extent
    };
  }

  inline ExtentRequest RequestAtLeast( float fractionOfParent )
  {
    return ExtentRequest
    {
      .requestType = RequestType::AtLeast,
      .request = fractionOfParent
    };
  }

  class IntervalBuilder;
  using DimChildrenMethod  = std::function< void( IntervalBuilder& ) >;
  using PosChildrenMethod  = std::function< void( IntervalBuilder& ) >;
  using DeduceExtentMethod = std::function< int ( IntervalBuilder& ) >;
  using ExtentNotifier     = std::function< void( int extent ) >;

  struct Notifiers
  {
    ExtentNotifier extentNotifier;
  };

  struct Interval
  {
    int position = 0;
    int extent = 0;
  };

  struct IntervalProps
  {
    ExtentRequest extentRequest;
    DimChildrenMethod dimChildren;
    PosChildrenMethod posChildren;
    DeduceExtentMethod deduceExtent;
    ExtentNotifier extentNotifier;
  };

  class IntervalBuilder
  {
  private:
    ExtentRequest m_ExtentRequest;
    
    Key m_Key;
    int m_ActualExtent = -1;
    int m_Position = -1;

    DimChildrenMethod m_DimChildren;
    PosChildrenMethod m_PosChildren;
    DeduceExtentMethod m_DeduceExtent;
    ExtentNotifier m_ExtentNotifier;

    std::vector<IntervalBuilder> m_Children;

  public:
    IntervalBuilder() = default;

    IntervalBuilder( IntervalProps props );

    void CalcRequest( int parentExtent );

    int GetRequestedExtent() const;
    bool IsFlexible() const;

    Key GetKey() const;
    void SetKey( Key key );

    int  GetActualExtent() const;
    void SetActualExtent( int actualExtent );

    int  GetPosition() const;
    void SetPosition( int position );

    void PosChildren();
    void DimChildren();

    Interval GetInterval() const;

    int DeduceActualExtent();
    int TotalRequestedByChildren() const;
    int CountFlexibleChildren() const;

    void ForEachChild( std::function< void( IntervalBuilder& child ) > f );
    void ForEachChild( std::function< void( IntervalBuilder const& child ) > f ) const;

    void AddChild( IntervalBuilder child );
    IntervalBuilder WithoutChildren() const;
    std::vector<IntervalBuilder> const& GetChildren() const;

    void DebugPrint( int depth ) const;
  };

  //////////////////////////
  // Dimensioning Methods //
  //////////////////////////

  void DimChildrenCopyParent( IntervalBuilder& parent );
  void DimChildrenEvenly( IntervalBuilder& parent );
  DimChildrenMethod DimChildrenInset( int before, int after );

  //////////////////////
  // Position Methods //
  //////////////////////

  void PosChildrenCopyParent( IntervalBuilder& parent );
  // when the childrens' coordinate space is different from the parent's
  // e.g. the coordinate space of a scroll view is internally different from 
  // the surrounding layouts in the tree
  PosChildrenMethod PosChildrenSequentiallyStartingAt( int start = 0 );
  PosChildrenMethod PosChildrenSequentiallyWithInitialOffsetFromParent( int offsetFromParent = 0 );
  PosChildrenMethod PosChildrenAllAt( int position );

  /////////////////////
  // Convenience API //
  /////////////////////

  IntervalBuilder ChildlessIntervalBuilder( ExtentRequest req, Notifiers = {} );
  IntervalBuilder SingleChildIntervalBuilder( ExtentRequest req, Notifiers = {} );
  IntervalBuilder RowWidthIntervalBuilder( ExtentRequest req, Notifiers = {} );
  IntervalBuilder RowHeightIntervalBuilder( ExtentRequest req, Notifiers = {} );
  IntervalBuilder ColumnWidthIntervalBuilder( ExtentRequest req, Notifiers = {} );
  IntervalBuilder ColumnHeightIntervalBuilder( ExtentRequest req, Notifiers = {} );
  IntervalBuilder ScrollWidthIntervalBuilder( ExtentRequest req, Notifiers = {} );
  IntervalBuilder ScrollHeightIntervalBuilder( ExtentRequest req, Notifiers = {} );
  IntervalBuilder InsetIntervalBuilder( ExtentRequest req, int before, int after, Notifiers = {} );


} // namespace Intervals

#endif