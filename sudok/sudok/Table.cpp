#include "stdafx.h"
#include "Table.h"

namespace KSudoku {

    bool Cell::increaseValue()
    {
        mValue++;
        if (mValue <= 9)
            return true;
        else
        {
            mValue = 0;     // reset to 0
            return false;
        }
    }

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

    bool Table::hasNextCell(const Cell& cell) const
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
            return mCells[x][y + 1];
        }
        else
        {
            return mCells[x + 1][0];
        }
    }

    Cell& Table::getFirstCell()
    {
        return mCells[0][0];
    }

    bool Table::verifyNumbers(int v[9]) const
    {
        int numbersAppearance[9];
        for (int i = 0; i < 9; i++)
        {
            numbersAppearance[i] = 0;
        }

        for (int i = 0; i < 9; i++)
        {
            int value = v[i];
            if (value >= 1 && value <= 9)
            {
                numbersAppearance[value - 1]++;
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

    bool Table::verifyCell(const Cell& cell) const
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
        // the 9x9 sudoku matrix can be divided into 9 sub-region,
        // with the following 9 cells as the top-left corners
        // (0, 0), (0, 3), (0, 6)
        // (3, 0), (3, 3), (3, 6)
        // (6, 0), (6, 3), (6, 6)
        //
        // now we determine which sub-region this cell is in
        //
        int baseX = cell.x() / 3 * 3;
        int baseY = cell.y() / 3 * 3;

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

    bool Table::veifyAll() const
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

    void Table::print()
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                printf("%d ", mCells[i][j].value());
                if ((j + 1) % 3 == 0)
                {
                    printf(" ");
                }
            }
            if ((i + 1) % 3 == 0)
            {
                printf("\n");
            }
            printf("\n");
        }
    }

    const Cell& Table::getCell(int i, int j) const
    {
        return mCells[i][j];
    }

    void Table::initConstCell(int i, int j, int v)
    {
        mCells[i][j].setValue(v);
        mCells[i][j].setConst();
    }



};