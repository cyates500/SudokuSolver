#pragma once
#include "cell.h"
#include <memory>
#include <vector>
#include <fstream>
#include <strstream>
#include <string>
#include <algorithm>
#include <boost/assign/list_of.hpp>
#include <iterator>
#include <iostream>
#include <map>
#include <boost/range/irange.hpp>


// @file
// @author  Chris Yates <cyates50@hotmail.co.uk>
// @version 1.0
//
// @section LICENSE
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details at
// http://www.gnu.org/copyleft/gpl.html
//
// @section DESCRIPTION
//
// The Grid...
// RCS - Row, Column or Subgrid


class Grid
{
public:
  static const int SIZE_SUBGRID = 3;
  static const int SIZE_GRID    = SIZE_SUBGRID * SIZE_SUBGRID;
  static const int NUM_CELLS    = SIZE_GRID * SIZE_GRID;

  struct mless
		: public std::binary_function<std::shared_ptr<Cell>, std::shared_ptr<Cell>, bool>
	{	// functor for operator<
	bool operator()(const std::shared_ptr<Cell>& left, const std::shared_ptr<Cell>& right) const
		{	
      return left->get_candidates() < right->get_candidates();
		}
	};
  typedef std::set<std::shared_ptr<Cell>, mless> SetPtrCellLessT;


  struct cellless
		: public std::binary_function<std::shared_ptr<Cell>, std::shared_ptr<Cell>, bool>
	{	// functor for operator<
	bool operator()(const std::shared_ptr<Cell>& left, const std::shared_ptr<Cell>& right) const
		{	
      return left.get() < right.get();
		}
	};


  typedef std::set<std::shared_ptr<Cell>, cellless> SetPtrCellPtrLessT;


  Grid(void)
  {
    m_cells.resize( SIZE_GRID );

    for ( const auto row : boost::irange( 0, SIZE_GRID ))
    {
      m_cells[row].resize( SIZE_GRID );
      for ( const auto col : boost::irange( 0, SIZE_GRID ))
      {
        Cell* new_cell = new Cell;
        m_cells[row][col] = std::shared_ptr<Cell>(new_cell);
        m_map_cell_to_row_col[ new_cell ] = std::make_pair( row, col );
      }
    }
  }

  Grid( const Grid& rhs )
  {
    m_cells.resize( SIZE_GRID );

    for ( const auto row : boost::irange( 0, SIZE_GRID ))
    {
      m_cells[row].resize( SIZE_GRID );
      for ( const auto col : boost::irange( 0, SIZE_GRID ))
      {
        Cell* new_cell = new Cell( *rhs.m_cells[row][col].get() );
        m_cells[row][col] = std::shared_ptr<Cell>(new_cell);
        m_map_cell_to_row_col[ new_cell ] = std::make_pair( row, col );
      }
    }
  }

  std::set<std::shared_ptr<Cell>, mless> 
  get_solved_RCS( int row, int col )
  {
    std::set<std::shared_ptr<Cell>, mless> solved_rcs_set;
    // rows and columns
    for( int i = 0; i < SIZE_GRID; ++i )
    { 
      std::shared_ptr<Cell>& temp1_cell_ptr = m_cells[row][i];
      if ( temp1_cell_ptr->solved() )
      {
        solved_rcs_set.insert( temp1_cell_ptr );
      }
      std::shared_ptr<Cell>& temp2_cell_ptr = m_cells[i][col];
      if ( temp2_cell_ptr->solved() )
      {
        solved_rcs_set.insert( temp2_cell_ptr );
      }
    }
    //subgrids
    for( int i = 0; i < SIZE_SUBGRID; ++i )
    {
      for( int j = 0; j < SIZE_SUBGRID; ++j )
      {
        std::shared_ptr<Cell>& temp3_cell_ptr = m_cells[((row / SIZE_SUBGRID) * SIZE_SUBGRID ) + i][(( col / SIZE_SUBGRID ) * SIZE_SUBGRID ) + j];
        if ( temp3_cell_ptr->solved() )
        {
          solved_rcs_set.insert( temp3_cell_ptr );
        }
      }
    }
    return solved_rcs_set;
  }

  std::set<std::shared_ptr<Cell>> 
  get_unsolved_RCS( int row, int col )
  {
    std::set<std::shared_ptr<Cell>> rcs_set;
    // rows and columns
    for( int i = 0; i < SIZE_GRID; ++i )
    { 
      std::shared_ptr<Cell>& temp1_cell_ptr = m_cells[row][i];
      if( !temp1_cell_ptr->solved())
      {
        rcs_set.insert( temp1_cell_ptr );
      }
      std::shared_ptr<Cell>& temp2_cell_ptr = m_cells[i][col];
      if( !temp2_cell_ptr->solved())
      {
        rcs_set.insert( temp2_cell_ptr );
      }
      
    }
    //subgrids
    for( int i = 0; i < SIZE_SUBGRID; ++i )
    {
      for( int j = 0; j < SIZE_SUBGRID; ++j )
      {
        std::shared_ptr<Cell>& temp3_cell_ptr = m_cells[((row / SIZE_SUBGRID) * SIZE_SUBGRID ) + i][(( col / SIZE_SUBGRID ) * SIZE_SUBGRID ) + j];
        if( !temp3_cell_ptr->solved())
        {
          rcs_set.insert( temp3_cell_ptr );
        }
      }
    }
    return rcs_set;
  }
  
  std::set<std::shared_ptr<Cell>>
  get_row_cells( int row, int exclude_column )
  {
    std::set<std::shared_ptr<Cell>> row_cells;
    for( int i = 0; i < SIZE_GRID; ++i )
    {
      std::shared_ptr<Cell>& temp_cell_ptr = m_cells[row][i];
      if ( i != exclude_column )
      {
        row_cells.insert( temp_cell_ptr ); 
      }
    }
    return row_cells;
  }

  std::set<std::shared_ptr<Cell>> 
  get_col_cells( int exclude_row, int col )
  {
    std::set<std::shared_ptr<Cell>> col_cells;
    for( int i = 0; i < SIZE_GRID; ++i )
    {
      std::shared_ptr<Cell>& temp_cell_ptr = m_cells[i][col];
      if ( i != exclude_row )
      {
        col_cells.insert( temp_cell_ptr ); 
      }
    }
    return col_cells;
  }

  std::set<std::shared_ptr<Cell>>
  get_subgrid_cells( int row, int col )
  {
    std::set<std::shared_ptr<Cell>> subgrid_cells;

    for( int i = 0; i < SIZE_SUBGRID; ++i )
    {
      for( int j = 0; j < SIZE_SUBGRID; ++j )
      {
        std::shared_ptr<Cell>& temp_cell_ptr = m_cells[((row / SIZE_SUBGRID) * SIZE_SUBGRID ) + i][(( col / SIZE_SUBGRID ) * SIZE_SUBGRID ) + j];
        if ( temp_cell_ptr != m_cells[row][col] )
        {
          subgrid_cells.insert( temp_cell_ptr );
        }
      }
    }
    return subgrid_cells;
  }

  std::set<int>
  get_row_values( int row, int exclude_column )
  {
    std::set<int> row_values;
    for( int i = 0; i < SIZE_GRID; ++i )
    {
      if ( i != exclude_column )
      {
        row_values.insert( m_cells[row][i]->get_candidates().begin(), 
                           m_cells[row][i]->get_candidates().end());
      }
    }
    return row_values;
  }

  std::set<int>
  get_col_values( int exclude_row, int col )
  {
    std::set<int> col_values;
    for( int i = 0; i < SIZE_GRID; ++i )
    {
      if ( i != exclude_row )
      {
        col_values.insert( m_cells[i][col]->get_candidates().begin(), 
                           m_cells[i][col]->get_candidates().end());
      }
    }
    return col_values;
  }
  
  std::set<int>
  get_subgrid_values( int row, int col )
  {
    std::set<int> subgrid_values;

    for( int i = 0; i < SIZE_SUBGRID; ++i )
    {
      for( int j = 0; j < SIZE_SUBGRID; ++j )
      {
        std::shared_ptr<Cell>& temp_cell_ptr = m_cells[((row / SIZE_SUBGRID) * SIZE_SUBGRID ) + i][(( col / SIZE_SUBGRID ) * SIZE_SUBGRID ) + j];
        if ( temp_cell_ptr != m_cells[row][col] )
        {
          subgrid_values.insert( temp_cell_ptr->get_candidates().begin(), 
                                 temp_cell_ptr->get_candidates().end());
        }
      }
    }
    return subgrid_values;
  }

  bool
  solve_for_subgrid()
  {
    for( int row = 0; row < SIZE_GRID; ++row )
    {
      for( int col = 0; col < SIZE_GRID; ++col )
      {
        if( !m_cells[row][col]->solved() )
        {
          std::set<int> subgrid_cell_values      = get_subgrid_values( row, col );
          std::shared_ptr<Cell> analysed_cell    = m_cells[row][col];
          std::set<int> analysed_cell_candidates = m_cells[row][col]->get_candidates();
          if( subgrid_cell_values.size() != SIZE_GRID )
          {
            for(const auto analysed_cell_candidates_value :  analysed_cell_candidates)
            {
               if ( subgrid_cell_values.find( analysed_cell_candidates_value ) == subgrid_cell_values.end())
               {
                 *analysed_cell = analysed_cell_candidates_value;
               }
             }
          }
          if ( m_cells[row][col]->solved() )
          {
            row = 0;
            col = -1;
          }
        }
      }
    }
    return true;
  }
  

  void
  solve_for_col()
  {
    for( int row = 0; row < SIZE_GRID; ++row )
    {
      for( int col = 0; col < SIZE_GRID; ++col )
      {
        if( !m_cells[row][col]->solved() )
        {
          std::set<int> col_cell_values          = get_col_values( row, col );
          std::shared_ptr<Cell> analysed_cell    = m_cells[row][col];
          std::set<int> analysed_cell_candidates = m_cells[row][col]->get_candidates();
          if( col_cell_values.size() != SIZE_GRID )
          {
            for( const auto analysed_cell_candidates_value : analysed_cell_candidates )
            {
              if ( col_cell_values.find( analysed_cell_candidates_value ) == col_cell_values.end())
              {
                *analysed_cell = analysed_cell_candidates_value;
              }
            }
          }
          if ( m_cells[row][col]->solved() )
          {
            row = 0;
            col = -1;
          }
        }
      }
    }
  }
 
  void
  solve_for_row()
  {
    //row
    // for each candidate of each cell go over the rest of the row's cells' candidates to see if it's unique
    for( int row = 0; row < SIZE_GRID; ++row )
    {
      for( int col = 0; col < SIZE_GRID; ++col )
      {
        if( !m_cells[row][col]->solved() )
        {
          std::set<int>         row_cell_values          = get_row_values( row, col );
          std::shared_ptr<Cell> analysed_cell            = m_cells[row][col];
          std::set<int>         analysed_cell_candidates = m_cells[row][col]->get_candidates(); 

          if( row_cell_values.size() != SIZE_GRID )
          {
            for(const auto analysed_cell_candidates_value : analysed_cell_candidates )
            {
              if ( row_cell_values.find( analysed_cell_candidates_value ) == row_cell_values.end())
              {
                *analysed_cell = analysed_cell_candidates_value;
              }
            }
          }
          if ( m_cells[row][col]->solved() )
          {
            row = 0;
            col = -1;
          }
        }
      }
    }
  }
 

  const std::vector<std::vector<std::shared_ptr<Cell>>>&
  get_cells() const 
  {
    return m_cells; 
  }

  ~Grid(void)
  {
  }

  std::vector<int> 
  get_values_from_grid( int number )
  {
    std::vector<int> ret_vec;
    for( int col = 0; col < number; ++col )
    {
      ret_vec.push_back( m_cells[0][col]->get_solution() );
    }
    return ret_vec;
  }

   void
   initialise()
   {
     // get feasible candidates (from rows, columns and subgrids) for each empty cell, if only one fill in / if only one of that number in each rcs, fill in.
     for( int row = 0; row < SIZE_GRID; ++row )
     {
       for( int col = 0; col < SIZE_GRID; ++col )
       {
         const std::set<int>& candidates = m_cells[row][col]->get_candidates();
         typedef std::set<std::shared_ptr<Cell>> SetPtrCellT;
         SetPtrCellT result_set;
         if( !m_cells[row][col]->solved() )
         {
           std::set<std::shared_ptr<Cell>, mless> rcs = get_solved_RCS( row, col);
           for ( int i = 1; i < 10; ++i )
           {
             std::shared_ptr<Cell> val_to_find( new Cell(i) );
             auto iter = rcs.find( val_to_find );
             if ( iter == rcs.end() )
             {
               result_set.insert( val_to_find );
             }
           }
           m_cells[row][col]->add_candidates( result_set ); 
           if ( m_cells[row][col]->solved() )
           {
             row = 0;
             col = -1;
           }
         }
       }
     }
   }

 
   // remove_candidates
   // if a cell is not solved, it will check its rcs for solved cells and remove the value for the solved cell from the this cell
   // 
   // @param nothing
   // @return nothing
   void remove_candidates()
   {
     for ( const auto row : boost::irange( 0, SIZE_GRID ))
     {
       for ( const auto col : boost::irange( 0, SIZE_GRID ))
       {
         const std::set<int>& candidates = m_cells[row][col]->get_candidates();
         if( !m_cells[row][col]->solved() )
         {
           std::set<std::shared_ptr<Cell>, mless> rcs = get_solved_RCS( row, col);
           for( const auto& rcs_iter : rcs )
           {
             if( rcs_iter->solved() )
             {
               //removing the solution of the solved cell from the analysed cell
               m_cells[row][col]->remove_candidate( rcs_iter->get_solution() );
             }
           }
         }
       }
     }
   }

   std::set<std::shared_ptr<Cell>>
   intersect( const std::set<std::shared_ptr<Cell>>& first, 
              const std::set<std::shared_ptr<Cell>>& second )
   {
     std::set<std::shared_ptr<Cell>> result;

     std::set_intersection( first.begin(), first.end(),
                            second.begin(), second.end(),
                            std::inserter( result, result.end() ));


     return result;
   }

   // solve for naked pairs
   // eliminates more candidates, when two cells with an intersection in rcs, both with the same two candidates, those two candidates can be removed from
   // all cells with the same intersection of rcs. Similar to remove_candidates() but in pairs.
   //
   //@param  nothing
   //@return nothing
   void
   solve_for_naked_pairs()
   {
     for ( const auto row : boost::irange( 0, SIZE_GRID ))
     {
       for ( const auto col : boost::irange( 0, SIZE_GRID ))
       {
         std::shared_ptr<Cell> analysed_cell = m_cells[row][col];
         std::set<std::shared_ptr<Cell>> first_rcs = get_unsolved_RCS( row, col);
         first_rcs.erase( m_cells[row][col]);


         if( m_cells[row][col]->get_candidates().size() == 2 )
         {
           for( std::set<std::shared_ptr<Cell>>::iterator rcs_iter = first_rcs.begin();
             rcs_iter != first_rcs.end();
             ++rcs_iter)
           {
             std::shared_ptr<Cell> rcs_cells = *rcs_iter;
             if( (*rcs_iter)->get_candidates().size() == 2)
             {
               if(  m_cells[row][col]->get_candidates() == (*rcs_iter)->get_candidates() )
               {
                 std::pair<int, int>& row_col = m_map_cell_to_row_col[ rcs_iter->get() ];
                 std::set<std::shared_ptr<Cell>> second_rcs = get_unsolved_RCS( row_col.first, row_col.second );
                 second_rcs.erase( m_cells[row_col.first][row_col.second]);
                 std::set<std::shared_ptr<Cell>> intercept = intersect( first_rcs, second_rcs );
                 std::pair< int, int > pair;
                 std::set<int>::iterator cand_iter = m_cells[row][col]->get_candidates().begin();
                 pair.first  =  *cand_iter++;
                 pair.second =  *cand_iter;
                 for( const auto& intercept_value : intercept )
                 {
                   intercept_value->remove_candidate( pair.first );
                   intercept_value->remove_candidate( pair.second );
                 }
               }
             }
           }
         }
       }
     }
   }

   // solve for hidden pairs
   // similar to solve_for_row/column/subgrid but in pairs, if the pair of candidates in a pair of cells appears once in the intersection of rcs
   // then all other candidates can be removed from the analysed cells apart from the pair.
   //
   //@param nothing
   //@return nothing
   void
   solve_for_hidden_pairs()
   {
     for ( const auto row : boost::irange( 0, SIZE_GRID ))
     {
       for ( const auto col : boost::irange( 0, SIZE_GRID ))
       {
         std::set<int> candidates = m_cells[row][col]->get_candidates();
         std::set<std::shared_ptr<Cell>> first_rcs = get_unsolved_RCS( row, col);
         std::pair< int, int > pair;
         for( const auto& rcs_iter : first_rcs )
         {
           //if two cells in the rcs contains 2 candidates that are the same and that are unique to the intercept of the rcs,
           //remove all candidates other than those two.
           std::set<int> candidates2 = rcs_iter->get_candidates();
           int counter = 0;
           for( const auto& candidates2_iter : candidates2 )
           {
             for( const auto& candidates_iter : candidates )
             {
               if( candidates_iter == candidates2_iter )
               {
                 ++counter;
                 if( counter == 1 )
                 {
                   pair.first = candidates_iter;
                 }
                 if( counter == 2)
                 {
                   pair.second = candidates_iter;
                 }
               }
             }
           }
           if( counter == 2 )
           {
             std::pair<int, int>& row_col                      = m_map_cell_to_row_col[ rcs_iter.get() ];
             std::set<std::shared_ptr<Cell>> row_set           = get_row_cells( row, col );
             std::set<std::shared_ptr<Cell>> row_set2          = get_row_cells( row_col.first, row_col.second );
             std::set<std::shared_ptr<Cell>> row_intercept     = intersect( row_set, row_set2 );
             std::set<std::shared_ptr<Cell>> col_set           = get_col_cells( row, col );
             std::set<std::shared_ptr<Cell>> col_set2          = get_col_cells( row_col.first, row_col.second );
             std::set<std::shared_ptr<Cell>> col_intercept     = intersect( col_set, col_set2 );
             std::set<std::shared_ptr<Cell>> subgrid_set       = get_subgrid_cells( row, col );
             std::set<std::shared_ptr<Cell>> subgrid_set2      = get_subgrid_cells( row_col.first, row_col.second );
             std::set<std::shared_ptr<Cell>> subgrid_intercept = intersect( subgrid_set, subgrid_set2 );
             if( row_intercept.size() != 0 )
             {
               for( const auto& row_intercept_iter : row_intercept )
               {
                 std::set<int> row_candidates = row_intercept_iter->get_candidates();
                 for( const auto& row_candidates_iter : row_candidates )
                 {
                   if( row_candidates_iter != pair.first && row_candidates_iter != pair.second )
                   {
                     candidates.clear();
                     candidates.insert( pair.first );
                     candidates.insert( pair.second );
                     candidates2.clear();
                     candidates2.insert( pair.first );
                     candidates2.insert( pair.second );
                   }
                 }
               }
             }

             if( subgrid_intercept.size() != 0 )
             {
               for( const auto& subgrid_intercept_iter : subgrid_intercept )
               {
                 std::set<int> subgrid_candidates = subgrid_intercept_iter->get_candidates();
                 for( const auto& subgrid_candidates_iter : subgrid_candidates )
                 {
                   if( subgrid_candidates_iter != pair.first && subgrid_candidates_iter != pair.second )
                   {
                     candidates.clear();
                     candidates.insert( pair.first );
                     candidates.insert( pair.second );
                     candidates2.clear();
                     candidates2.insert( pair.first );
                     candidates2.insert( pair.second );
                   }
                 }
               }
             }

             if( col_intercept.size() != 0 )
             {
               for( const auto& col_intercept_iter : col_intercept )
               {
                 std::set<int> col_candidates = col_intercept_iter->get_candidates();
                 for( const auto& col_candidates_iter : col_candidates )
                 {
                   if( col_candidates_iter != pair.first && col_candidates_iter != pair.second )
                   {
                     candidates.clear();
                     candidates.insert( pair.first );
                     candidates.insert( pair.second );
                     candidates2.clear();
                     candidates2.insert( pair.first );
                     candidates2.insert( pair.second );
                   }
                 }
               }
             }
           }
         }
       }
     }
   }


   // solve by guessing
   // if a cell that is unsolved has two candidates only, it will eliminate one to solve that cell,
   // if it is correct it keeps this grid, if it is false it will choose the other option and keep going from there
   //
   //@param nothing
   //@return nothing
   bool
   solve_by_guessing()
   {
     for ( const auto row : boost::irange( 0, SIZE_GRID ))
     {
       for ( const auto col : boost::irange( 0, SIZE_GRID ))
       {
         if( m_cells[row][col]->get_candidates().size() == 2 )
         {
           Grid temp_grid1( *this );
           Grid temp_grid2( *this );
           bool solved = false;
           int first_guess = *m_cells[row][col]->get_candidates().begin();
           *(temp_grid1.m_cells[row][col]) = first_guess;
           bool solved1 = temp_grid1.solve();
           if( solved1 )
           {
             *(this->m_cells[row][col]) = first_guess;
             this->solve();
             solved = true;
             return true;
           }
           else
           {
             temp_grid2.m_cells[row][col]->remove_candidate( first_guess );
             bool solved2 = temp_grid2.solve();
             if( solved2 )
             {
               *(this->m_cells[row][col]) = *(temp_grid2.m_cells[row][col]->get_candidates().begin());
               this->solve();
               solved = true;
               return true;
             }
           }
         }
       }
     }
     return false;
   }
    
 
 bool 
 reducing_candidates_and_not_solved( int no_of_candidates_before, int no_of_candidates_after )
 {
   return no_of_candidates_before != no_of_candidates_after && no_of_candidates_after != ( NUM_CELLS );
 }

  bool
  solve()
  {
    int no_of_candidates_before =  0;
    int no_of_candidates_after  = -1;
    initialise();
    // while stil reducing candidates and not solved
    while( reducing_candidates_and_not_solved( no_of_candidates_before, no_of_candidates_after ))
    {
      no_of_candidates_before = no_of_candidates_after;
      no_of_candidates_after  = 0;
      

      remove_candidates();
      solve_for_row();
      solve_for_col();
      solve_for_subgrid();
      solve_for_naked_pairs();
      
      for ( const auto row : boost::irange( 0, SIZE_GRID ))
      {
        for ( const auto col : boost::irange( 0, SIZE_GRID ))
        {
          no_of_candidates_after += m_cells[row][col]->get_candidates().size();  
        }
      }
    }
    
    return (no_of_candidates_after == ( NUM_CELLS ));
  }

  static int 
  get_size()
  { 
    return SIZE_GRID; 
  }


private:

  typedef std::map< Cell*, std::pair<int, int>> MapCellToRowColT;

  MapCellToRowColT                                m_map_cell_to_row_col;
  std::vector<std::vector<std::shared_ptr<Cell>>> m_cells;

  void 
  set_cell_value( int row, int col, int value )
  {
    *( m_cells[row][col] ) = value;
  }

  friend std::istream& operator>>( std::istream& in, Grid& grid );
  friend std::ostream& operator<<( std::ostream& out, Grid& grid );
};// end class


 std::istream& operator>>( std::istream& in, Grid& grid )
 {
   if ( !in.eof() )
   {
     // loop for number of characters in line
     // CONVERT text into number
     // set cell value
     // USE THE LINE NUMBER AND CHARACTER NUMBER FOR ROW AND COLUMN
     for( int row = 0; row < Grid::get_size(); ++row)
     {
       std::string line;
       getline( in, line );
       for( int col = 0; col < Grid::get_size(); ++col )
       {
         char temp_char = line[col];
         int value;
         std::strstream str;
         str << temp_char;
         str >> value;
         grid.set_cell_value( row, col, value );
       }
     }
   }

   return in;
 }

std::ostream& operator<<( std::ostream& out, Grid& grid )
{
  for( int row = 0; row < 9; ++row )
    {
      for( int col = 0; col < 9; ++col )
      {
        out << "[ " << *(grid.m_cells[row][col]) << " ]";
      }
      out << std::endl;
    }
    
  return out;
}

