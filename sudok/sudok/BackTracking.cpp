#include "stdafx.h"
#include "Table.h"
#include "BackTracking.h"

namespace KSudoku {

bool ProcessCell(Table& table, Cell& cell);

// Algorithm:
//
// 1. Beginning from the first null cell, filling a number "1".
// 2. If that's legal (no duplication in row/column/sub-region), then go to the next null cell.
//    Of course filling number "1" this time is illegal, so increments its number to "2".
// 3. If this is still legal, go to the next cell again...until we finish all.
// 4. If in a cell all the numbers are illegal, it means a previous cell isn't filled right,
//    so go back one cell, incrementing its number, then start over from that cell again...
//
// The idea is that we can always meet the chance of all the numbers in each cell(step) are filling right.
//
bool SolveByBackTracking(Table& table) {
    bool b = ProcessCell(table, table.getFirstCell());
    if (b)
    {
        b = table.veifyAll();
    }

    return b;
}

bool ProcessCell(Table& table, Cell& cell) {
    if (cell.isConst())
    {
        if (table.hasNextCell(cell))
        {
            Cell& nextCell = table.getNextCell(cell);
            return ProcessCell(table, nextCell);
        }
        else
        {
            return table.verifyCell(cell);
        }
    }
    else {
        bool getAValidAnswer = false;
        while (!getAValidAnswer)
        {
            bool b = cell.increaseValue();
            if (b)
            {
                getAValidAnswer = table.verifyCell(cell);
                if (getAValidAnswer)
                {
                    if (table.hasNextCell(cell))
                    {
                        Cell& nextCell = table.getNextCell(cell);
                        getAValidAnswer = ProcessCell(table, nextCell);
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

};