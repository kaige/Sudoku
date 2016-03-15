Sudoku
======

A simple 9X9 Sudoku solver


Usage: ksudoku.exe <sudoku_file_path> [mode]

The "sudoku file" should be a text file like the following:

3 0 0 0 6 7 9 0 0
1 0 0 0 8 9 2 5 0
0 6 5 0 4 0 3 0 0
0 0 2 0 0 0 0 3 0
8 0 6 0 0 3 1 7 0
0 0 0 0 0 0 4 0 5
0 0 0 7 0 0 6 0 0
0 0 0 4 0 0 0 1 0
0 8 9 0 3 0 0 2 0

where "0" is the cell to be filled

the "mode" can take below values, which refers to different solving algorithms
* backtracking
* permutation
 
If not specifying mode it's default to "backtracking"

