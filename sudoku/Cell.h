#pragma once
#include <set>
#include <boost/assign/list_of.hpp>
#include <memory>

class Cell
{
public:

  Cell(void) : m_candidates()
  {
    m_candidates.insert( 0 );
  }
  explicit Cell( int value ) : m_candidates()
  {
    if ( value != 0 )
    {
      m_candidates.clear();
      m_candidates.insert( value );
    } 
  }
  explicit Cell( const Cell& rhs )
  {
    m_candidates = rhs.m_candidates;
  }

  Cell& 
  operator=( int value )
  {
    if ( value != 0 )
    {
      m_candidates.clear();
      m_candidates.insert( value );
    }
    return *this;
  }

  bool 
  operator==( const Cell& rhs )
  {
    if ( this != &rhs )
    {
      return this->m_candidates == rhs.m_candidates;
    }
    return true;
  }

  const std::set<int>& 
  get_candidates() const 
  { 
    return m_candidates; 
  }

  int 
  get_solution() const 
  { 
    if ( solved() ) 
    {
      return *(m_candidates.begin());
    }
    else
    {
      return 0;
    }
  }

  void 
  add_candidate( int value )
  {
    m_candidates.insert( value );
  }

  void 
  remove_candidate( int value )
  {
    m_candidates.erase( value );
  }

  // take all of the candidates of one cell and copy them into one in the grid
  void 
  add_candidates( const std::set<std::shared_ptr<Cell>>& candidates)
  {
    if ( empty() && candidates.size() > 0 )
    {
      remove_candidate( 0 );
    }
   
    m_candidates.clear();
    for( const auto i : candidates )
    {
      add_candidate( i->get_solution());
    } 
  }
  
  bool 
  solved() const
  {
    return ( m_candidates.size() == 1 && 
             *m_candidates.begin() != 0 );
  }
  

  bool 
  empty() const
  {
    return ( m_candidates.size() == 1 &&
             *m_candidates.begin() == 0  );
  }

  ~Cell(void)
  {
  }

private:
  std::set<int> m_candidates;
  friend std::ostream& operator<<( std::ostream& out, Cell& cell );

};

std::ostream& operator<<( std::ostream& out, Cell& cell )
{
  std::for_each( cell.m_candidates.begin(), cell.m_candidates.end(), 
    [&]( int candidate ){ out << candidate << ", "; });
  return out;
}

