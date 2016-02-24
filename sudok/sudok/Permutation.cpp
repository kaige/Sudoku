#include "stdafx.h"
#include "Table.h"
#include "BackTracking.h"
#include <stdio.h>

namespace KSudoku {

    bool GetNextPermuation(Table& table)
    {

        Cell* pCell = &table.getFirstCell();

        bool bIsValueValid = pCell->increaseValue();
        if (bIsValueValid)
            return true;

        bool bHasNextCell = true;
        while (bHasNextCell) {
            bHasNextCell = table.hasNextCell(*pCell);
            if (!bHasNextCell)
                break;

            Cell* pNextCell = &table.getNextCell(*pCell);
            bIsValueValid = pNextCell->increaseValue();
            if (bIsValueValid)
                return true;
            else
                pCell = pNextCell;
        }

        return false;
    }

    bool IsEqualOnConstValue(const Table& inputTable, const Table& solutionTable)
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                const Cell& inputCell = inputTable.getCell(i, j);
                if (!inputCell.isConst())
                    continue;

                if (inputCell.value() != solutionTable.getCell(i, j).value())
                    return false;
            }
        }

        return true;
    }

    bool SolveByPermutation(const Table& table)
    {
        // create an empty solution table whose value is all 0
        int allZeroArray[9][9];
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                allZeroArray[i][j] = 0;
            }
        }

        bool bHasSolution = false;
        int trialSolutionNumber = 0;

        Table* pSolutionTable = Table::createTable(allZeroArray);
        while (GetNextPermuation(*pSolutionTable))
        {
            pSolutionTable->print();

            getchar();

            if (IsEqualOnConstValue(table, *pSolutionTable))
            {
                if (pSolutionTable->veifyAll())
                {
                    bHasSolution = true;
                    printf("we find a solution: \n");
                    pSolutionTable->print();
                }
                else
                {
                    printf("triable solution %d is not valid\n", trialSolutionNumber);
                }
            }
            else
            {
                printf("trial solution %d is not equal to input\n", trialSolutionNumber);
            }

            trialSolutionNumber++;
        }

        if (!bHasSolution)
            printf("we don't find a solution.\n");

        delete pSolutionTable;

        return true;
    }


}