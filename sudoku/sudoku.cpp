// sudoku.cpp : Defines the entry point for the console application.
//Problem 96
//27 May 2005
//
//Su Doku (Japanese meaning number place) is the name given to a popular puzzle concept. Its origin is unclear, but credit must be attributed to Leonhard Euler who invented a similar, and much more difficult, puzzle idea called Latin Squares. The objective of Su Doku puzzles, however, is to replace the blanks (or zeros) in a 9 by 9 grid in such that each row, column, and 3 by 3 box contains each of the digits 1 to 9. Below is an example of a typical starting puzzle grid and its solution grid.
//
//
//  A well constructed Su Doku puzzle has a unique solution and can be solved by logic, 
//  although it may be necessary to employ "guess and test" methods in order to eliminate options 
//  (there is much contested opinion over this). The complexity of the search determines the difficulty of the puzzle;
//  the example above is considered easy because it can be solved by straight forward direct deduction.
//
//  The 6K text file, sudoku.txt (right click and 'Save Link/Target As...'), 
//  contains fifty different Su Doku puzzles ranging in difficulty, but all with unique solutions (the first puzzle in the file is the example above).
//
//  By solving all fifty puzzles find the sum of the 3-digit numbers found in the top left corner of each solution grid;
//  for example, 483 is the 3-digit number found in the top left corner of the solution grid above.

#include "stdafx.h"
#include "Cell.h"
#include "Grid.h"
#include <fstream>
#include <vector>

bool solve( Grid& grid )
{
  return grid.solve() || grid.solve_by_guessing();
}

int euler_number_calc( Grid grid )
{
   std::vector<int> euler_values= grid.get_values_from_grid( 3 );
   return ( euler_values[0] * 100 ) +
          ( euler_values[1] * 10  ) +
          ( euler_values[2]       );
}

int _tmain(int argc, _TCHAR* argv[])
{
  using namespace std;

  if ( argc != 2 )
  {
    cout << "Usage: sudoku <file>" << endl;
    return 1;
  }

  ifstream grids_io( argv[1] );
  
  if (!grids_io)
  {
    cout << "bad file: " << argv[1] << endl;
    return 1;
  }
  int count_solved   = 0;
  int count_unsolved = 0;
  char line[  Grid::SIZE_GRID ];
  int cumulative = 0;
  while ( !grids_io.eof() )
  {
    grids_io.getline( line,  Grid::get_size() );
    if ( strstr( line, "Grid" ) != 0 )
    {
      Grid a_grid;
      grids_io >> a_grid;

      if( solve( a_grid ))
      {
        ++count_solved;
      }
      else
      {
        ++count_unsolved;
      }
      
      std::cout << a_grid;
      std::cout << " " << std::endl;
      std::cout << " " << std::endl;
      cumulative += euler_number_calc( a_grid );
    }
  }

  std::cout << "solved: "<< count_solved << std::endl;
  std::cout << "unsolved: "<< count_unsolved << std::endl;
  std::cout << "number: " << cumulative << std::endl;
	return 0;
}

