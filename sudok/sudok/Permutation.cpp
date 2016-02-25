#include "stdafx.h"
#include "Table.h"
#include "BackTracking.h"
#include <stdio.h>
#include <vector>

namespace KSudoku {

    class CellValue {
    public:
        CellValue(Cell* pCell, const ValueList& valueList);
        bool takeNextPossibleValue();
        void takeCurrentPossibleValue();
        int numOfPossibleValues() { return m_possibleValues.size(); }
    private:
        Cell*               m_pCell;
        ValueList           m_possibleValues;
        int                 m_curentValueIndex;
    };

    typedef std::vector<CellValue> CellValueList;

    CellValue::CellValue(Cell* pCell, const ValueList& valueList)
        : m_pCell(pCell), m_possibleValues(valueList), m_curentValueIndex(0)
    {
    }

    bool CellValue::takeNextPossibleValue()
    {
        m_curentValueIndex++;

        bool bCurrentValueValid = false;
        if (m_curentValueIndex < (int) m_possibleValues.size())
            bCurrentValueValid = true;
        else
        {
            m_curentValueIndex = 0;     // reset to 0
            bCurrentValueValid = false;
        }

        m_pCell->setValue(m_possibleValues[m_curentValueIndex]);
        return bCurrentValueValid;
    }

    void CellValue::takeCurrentPossibleValue()
    {
        m_pCell->setValue(m_possibleValues[m_curentValueIndex]);
    }

    typedef std::vector<int>                SubRegionSolution;
    typedef std::vector<SubRegionSolution>  SubRegionSolutionList;

    class SubRegion {
    public:
        SubRegion();
        void setBaseCoordinate(int x, int y);
        void addCellValue(const CellValue& cv);
    private:
        int                     m_baseX;
        int                     m_baseY;
        CellValueList           m_cellValueList;
        SubRegionSolutionList   m_solutions;
    };

    SubRegion::SubRegion()
        : m_baseX(0), m_baseY(0)
    {

    }

    void SubRegion::setBaseCoordinate(int x, int y)
    {
        m_baseX = x;
        m_baseY = y;
    }

    void SubRegion::addCellValue(const CellValue& cv)
    {
        m_cellValueList.push_back(cv);
    }

    bool GetNextPermutation( CellValueList& cellValueList)
    {
        CellValueList::iterator it = cellValueList.begin();
        for (; it != cellValueList.end(); ++it)
        {
            bool bIsValueValid = it->takeNextPossibleValue();
            if (bIsValueValid)
                return true;
        }

        return false; // we exhaust all the value combination
    }

    void BuildCellValueList(Table& table, CellValueList& cellValueList)
    {
        cellValueList.clear();

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                Cell* pCell = table.getCellPtr(i, j);
                if (!pCell->isConst())
                {
                    ValueList possibleValueList;
                    table.getPossibleValues(*pCell, possibleValueList);

                    CellValue valueListEntry(pCell, possibleValueList);
                    cellValueList.push_back(valueListEntry);
                }
            }
        }
    }

    bool SolveByPermutation(Table& table)
    {
        bool bHasSolution = false;

        CellValueList cellValueList;
        BuildCellValueList(table, cellValueList);

        CellValueList::iterator it = cellValueList.begin();
        double product = 1;
        for (; it != cellValueList.end(); ++it)
        {
            product *= it->numOfPossibleValues();
            it->takeCurrentPossibleValue();
        }

        printf("we'll evaluate %f solutions\n", product);

        if (table.veifyAll())
        {
            bHasSolution = true;
            printf("we find a solution: \n");
            table.print();
        }

        while (GetNextPermutation(cellValueList))
        {
            if (table.veifyAll())
            {
                bHasSolution = true;
                printf("we find a solution: \n");
                table.print();
            }
        }

        if (!bHasSolution)
            printf("we don't find a solution.\n");

        return true;
    }


}