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
        Cell* cell() const        { return m_pCell; }
        int currentValue() const  { return m_possibleValues[m_curentValueIndex]; }

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

        bool bCurrentIndexValid = false;
        if (m_curentValueIndex < (int) m_possibleValues.size())
            bCurrentIndexValid = true;
        else
        {
            m_curentValueIndex = 0;     // reset to 0
            bCurrentIndexValid = false;
        }

        m_pCell->setValue(m_possibleValues[m_curentValueIndex]);
        return bCurrentIndexValid;
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
        void setTable(Table* pTable);
        void setBaseCoordinate(int x, int y);
        void addCellValue(const CellValue& cv);

        bool solveByPermutation();
        bool verify();

        void takeCurrentSolution();
        bool takeNextSolution();

        int numOfSolutions() const { return (int) m_solutions.size(); }

    private:
        static bool GetNextPermutation(CellValueList& cellValueList);
        void addSubRegionSolution();
    private:
        Table*                  m_pTable;
        int                     m_baseX;
        int                     m_baseY;
        CellValueList           m_cellValueList;
        SubRegionSolutionList   m_solutions;
        int                     m_currentSolutionIndex;
    };

    SubRegion::SubRegion()
        : m_pTable(nullptr), m_baseX(0), m_baseY(0), m_currentSolutionIndex(0)
    {

    }

    void SubRegion::setTable(Table* pTable)
    {
        m_pTable = pTable;
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

    bool SubRegion::GetNextPermutation(CellValueList& cellValueList)
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

    bool SubRegion::solveByPermutation()
    {
        bool bHasSolution = false;

        CellValueList::iterator it = m_cellValueList.begin();
        double product = 1;
        for (; it != m_cellValueList.end(); ++it)
        {
            product *= it->numOfPossibleValues();
            it->takeCurrentPossibleValue();
        }

        printf("we'll evaluate %f solutions in this sub-region\n", product);

        if (verify())
        {
            bHasSolution = true;
            addSubRegionSolution();
        }

        while (GetNextPermutation(m_cellValueList))
        {
            if (verify())
            {
                bHasSolution = true;
                addSubRegionSolution();
            }
        }

        if (!bHasSolution)
            printf("we don't find a solution.\n");

        return bHasSolution;
    }

    void SubRegion::addSubRegionSolution()
    {
        SubRegionSolution solution;
        CellValueList::const_iterator it = m_cellValueList.begin();
        for (; it != m_cellValueList.end(); ++it)
            solution.push_back(it->currentValue());

        m_solutions.push_back(solution);
    }

    bool SubRegion::verify()
    {
        // in this sub region the number 1 ~ 9 should also appear once and only once
        //
        int numAppearnce[9];
        for (int i = 0; i < 9; i++)
            numAppearnce[i] = 0;

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                int cellValue = m_pTable->getCell(m_baseX + i, m_baseY + j).value();
                numAppearnce[cellValue - 1]++;
            }
        }

        for (int i = 0; i < 9; i++)
        {
            if (numAppearnce[i] != 1)
                return false;
        }

        return true;
    }


    void SubRegion::takeCurrentSolution()
    {
        SubRegionSolution& sol = m_solutions[m_currentSolutionIndex];

        for (size_t i = 0; i < m_cellValueList.size(); i++)
        {
            m_cellValueList[i].cell()->setValue(sol[i]);
        }
    }

    bool SubRegion::takeNextSolution()
    {
        m_currentSolutionIndex++;

        bool bCurrentIndexValid = false;
        if (m_currentSolutionIndex < (int)m_solutions.size())
            bCurrentIndexValid = true;
        else
        {
            m_currentSolutionIndex = 0;     // reset to 0
            bCurrentIndexValid = false;
        }

        takeCurrentSolution();
        return bCurrentIndexValid;
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

    void BuildSubRegion(Table* pTable, CellValueList& cellValueList, SubRegion subRegions[3][3])
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                subRegions[i][j].setTable(pTable);
                subRegions[i][j].setBaseCoordinate(i * 3, j * 3);
            }
        }

        CellValueList::iterator it = cellValueList.begin();
        for (; it != cellValueList.end(); ++it)
        {
            int i = it->cell()->x() / 3;
            int j = it->cell()->y() / 3;
            subRegions[i][j].addCellValue(*it);
        }
    }

    bool GetNextPermutation(SubRegion subRegions[3][3])
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                bool bHasSolution = subRegions[i][j].takeNextSolution();
                if (bHasSolution)
                    return true;
            }
        }

        return false;
    }


    bool SolveByPermutation(Table& table)
    {
        bool bHasSolution = false;

        CellValueList cellValueList;
        BuildCellValueList(table, cellValueList);

        SubRegion subRegions[3][3];
        BuildSubRegion(&table, cellValueList, subRegions);

        // firstly find solutions for each sub-region
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                bHasSolution = subRegions[i][j].solveByPermutation();
                if (!bHasSolution)
                {
                    printf("we don't find a solution.\n");
                    return false;
                }
            }
        }

        // let all sub regions take their first solution, verify
        int product = 1;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                product *= subRegions[i][j].numOfSolutions();
                subRegions[i][j].takeCurrentSolution();
            }
        }

        printf("we'll evaluate %d solutions\n", product);

        int numSolutionVerified = 0;

        numSolutionVerified++;
        if (table.veifyAll())
        {
            bHasSolution = true;
            printf("we find a solution.\n");
            table.print();
        }

        // now permutate the solutions in each sub region to get the solution of the whole table
        while (GetNextPermutation(subRegions))
        {
            numSolutionVerified++;
            printf("%d solutions verified\n", numSolutionVerified);

            if (table.veifyAll())
            {
                bHasSolution = true;
                printf("we find a solution.\n");
                table.print();
            }
        }

        if (!bHasSolution)
            printf("we don't find a solution.\n");

        return bHasSolution;
    }

}