// -----------------------------------------------------------------------------
//
// This program generates a solution for a 9X9 sudoku game.
//
// Usage: ksudoku.exe <sudoku_file.txt>
// the sudoku file should be a text file like the following
//
// 3 0 0 0 6 7 9 0 0
// 1 0 0 0 8 9 2 5 0
// 0 6 5 0 4 0 3 0 0
// 0 0 2 0 0 0 0 3 0
// 8 0 6 0 0 3 1 7 0
// 0 0 0 0 0 0 4 0 5
// 0 0 0 7 0 0 6 0 0
// 0 0 0 4 0 0 0 1 0
// 0 8 9 0 3 0 0 2 0
//
// where "0" is the cell to be filled
//
// Author: 	Kai Zhang
// Date: 	2014/4/19
// ----------------------------------------------------------------------------

#include "stdafx.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include "Table.h"
#include "BackTracking.h"
#include "Permutation.h"

using namespace KSudoku;

Table* createTableFromFile(const char* fileName)
{
	int a[9][9];	// the sudoku array

	std::ifstream file(fileName);
    std::string str;
	int i = 0;
    while (std::getline(file, str))
    {
		sscanf_s(str.c_str(), "%d %d %d %d %d %d %d %d %d",
			&a[i][0], &a[i][1], &a[i][2], &a[i][3], &a[i][4], &a[i][5], &a[i][6], &a[i][7], &a[i][8]);
		i++;
    }

	Table* table = Table::createTable(a);
	return table;
}


int _tmain(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("please specify the sudoku file\n");
		return -1;
	}

	Table* aTable = createTableFromFile(argv[1]);
	if (!aTable)
	{
		printf("the input file is illegal\n");
		return -1;
	}

	printf("the original table is...\n\n");
	aTable->print();

    if (argc == 2)
    {
        SolveByBackTracking(*aTable);   // default to backtracking
    }
    else if (argc == 3){
        const char* mode = argv[2];
        if (strcmp(mode, "backtracking") == 0)
        {
            SolveByBackTracking(*aTable);
        }
        else if (strcmp(mode, "permutation") == 0)
        {
            SolveByPermutation(*aTable);
        }
    }

	delete aTable;
	return 0;
}
