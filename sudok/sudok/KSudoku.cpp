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

namespace KSudoku {

class Cell {
public:
	Cell():mX(0), mY(0), mValue(0), mIsConst(false) {}

	void 	setCoordinate(int i, int j) {mX = i; mY = j;}
	void 	setConst() {mIsConst = true;}
	bool 	isConst() const {return mIsConst;}
	void 	setValue(int v) {mValue = v;}
	void 	resetValue() { if (!mIsConst) mValue = 0;}
	int 	value() const {return mValue;}

	bool 	generateAnswer();

	int 	x() const {return mX;}
	int 	y() const {return mY;}

private:
	int 	mX;
	int 	mY;
	int 	mValue;
	bool 	mIsConst;
};

bool Cell::generateAnswer()
{
	mValue++;
	return mValue <= 9;
}

class Table {
public:
	static Table*  createTable(int inputArray[9][9]);

	bool 	generateSolution();
	void 	print();

private:
	Table();
	void 	initConstCell(int i, int j, int v);
	bool 	processCell(Cell& cell);
	bool 	hasNextCell(const Cell& cell);
	Cell& 	getNextCell(const Cell& cell);
	bool 	verifyCell(const Cell& cell);
	bool 	veifyAll();
	bool 	verifyNumbers(int v[9]);

private:
	Cell 	mCells[9][9];
};

Table* Table::createTable(int inputArray[9][9])
{
	Table* table = new Table();

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			int v = inputArray[i][j];
			if (v < 0 || v >9)
			{
				printf("invalid input\n");
				delete table;
				return NULL;
			}
			else if (v > 0)
			{
				table->initConstCell(i, j, v);
			}
		}
	}

	return table;
}

Table::Table()
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			mCells[i][j].setCoordinate(i, j);
		}
	}
}

bool Table::hasNextCell(const Cell& cell)
{
	int x = cell.x();
	int y = cell.y();

	if (x == 8 && y == 8)		//last cell
		return false;
	else
		return true;
}

Cell& Table::getNextCell(const Cell& cell)
{
	int x = cell.x();
	int y = cell.y();

	if (y < 8)
	{
		return mCells[x][y+1];
	}
	else
	{
		return mCells[x+1][0];
	}
}

bool Table::verifyNumbers(int v[9])
{
	int numbersAppearance[9];
	for (int i = 0; i < 9; i++)
	{
		numbersAppearance[i] = 0;
	}

	for (int i = 0; i < 9; i++)
	{
		int value = v[i];
		if (value >= 1 && value <=9 )
		{
			numbersAppearance[value-1]++;
		}
	}

	//we shouldn't have duplicate numbers
	for (int i = 0; i < 9; i++)
	{
		if (numbersAppearance[i] > 1)
			return false;
	}

	return true;
}

bool Table::verifyCell(const Cell& cell)
{
	int x = cell.x();
	int y = cell.y();

	int numbers[9];

	// validate row
	for (int j = 0; j < 9; j++)
	{
		numbers[j] = mCells[x][j].value();
	}
	if (!verifyNumbers(numbers))
		return false;

	// validate column
	for (int i = 0; i < 9; i++)
	{
		numbers[i] = mCells[i][y].value();
	}
	if (!verifyNumbers(numbers))
		return false;

	// validate sub-region
	//
	// the 9x9 sudok matrix can be divided into 9 sub-region,
	// with the following 9 cells as the top-left corners
	// (0, 0), (0, 3), (0, 6)
	// (3, 0), (3, 3), (3, 6)
	// (6, 0), (6, 3), (6, 6)
	//
	// now we determine which sub-region this cell is in
	//
	int baseX = cell.x()/3*3;
	int baseY = cell.y()/3*3;

	// collect the numbers in the identified sub-region
	int nIndex = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			numbers[nIndex++] = mCells[baseX + i][baseY + j].value();
		}
	}
	if (!verifyNumbers(numbers))
		return false;

	return true;
}

bool Table::veifyAll()
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			bool r = verifyCell(mCells[i][j]);
			if (!r)
				return false;
		}
	}

	return true;
}

// Algorithm:
//
// 1. Beginning from the first null cell, filling a number "1".
// 2. If that's legal (no duplication in row/colum/sub-region), then go to the next null cell.
//    Of course filling number "1" this time is illegal, so increments its number to "2".
// 3. If this is still legal, go to the next cell again...until we finish all.
// 4. If in a cell all the numbers are illegal, it means a previous cell isn't filled right,
//    so go back one cell, incrementing its number, then start over from that cell again...
//
// The idea is that we can always meet the chance of all the numbers in each cell(step) are filling right.
//
bool Table::generateSolution()
{
	bool b = processCell(mCells[0][0]);
	if(b)
	{
		b = veifyAll();
	}

	return b;
}

bool Table::processCell(Cell& cell)
{
	if (cell.isConst())
	{
		if (hasNextCell(cell))
		{
			Cell& nextCell = getNextCell(cell);
			return processCell(nextCell);
		}
		else
		{
			return verifyCell(cell);
		}
	}
	else {
		bool getAValidAnswer = false;
		while (!getAValidAnswer)
		{
			bool b = cell.generateAnswer();
			if (b)
			{
				getAValidAnswer = verifyCell(cell);
				if (getAValidAnswer)
				{
					if (hasNextCell(cell))
					{
						Cell& nextCell = getNextCell(cell);
						getAValidAnswer = processCell(nextCell);
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
		}

		if (!getAValidAnswer)
			cell.resetValue();

		return getAValidAnswer;
	}
}

void Table::print()
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			printf("%d ", mCells[i][j].value());
			if ( (j+1) % 3 == 0)
			{
				printf (" ");
			}
		}
		if ((i+1)%3 == 0)
		{
			printf("\n");
		}
		printf("\n");
	}
}

void Table::initConstCell(int i, int j, int v)
{
	mCells[i][j].setValue(v);
	mCells[i][j].setConst();
}

};

using namespace KSudoku;

Table* createTableFromFile(char* fileName)
{
	int a[9][9];	// the sudok array

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

	bool b = aTable->generateSolution();

	if (b)
	{
		printf("we find a solutions!\n");
		aTable->print();
	}
	else
	{
		printf("we don't find a solution\n");
	}

	delete aTable;
	return 0;
}
