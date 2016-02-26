#include "stdafx.h"
#include "Table.h"
#include "BackTracking.h"
#include <stdio.h>
#include <vector>
#include <tuple>

namespace KSudoku {

    class CellValue {
    public:
        CellValue(Cell* pCell, const ValueList& valueList);
        bool takeNextPossibleValue();
        void takeCurrentPossibleValue();
        Cell* cell() const        { return m_pCell; }
        int currentValue() const  { return m_possibleValues[m_curentValueIndex]; }

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
        int  currentSolutionIndex()             { return m_currentSolutionIndex; }
        void setCurrentSolutionIndex(int n)     { m_currentSolutionIndex = n; }
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
        for (; it != m_cellValueList.end(); ++it)
        {
            it->takeCurrentPossibleValue();
        }

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

    typedef std::tuple<int, int, int>           SubRegionRowSolution;
    typedef std::vector<SubRegionRowSolution>   SubRegionRowSolutionList;

    class SubRegionRow
    {
    public:
        SubRegionRow();
        void setSubRegion(Table* pTable, int nBaseRow, SubRegion* p1, SubRegion* p2, SubRegion* p3);

        bool solveByPermutation();
        bool verify();

        void takeCurrentSolution();
        void setCurrentSolutionIndex(int n) { m_currentSolutionIndex = n; }
        bool takeNextSolution();

    private:
        static bool GetNextPermutation(SubRegion* regions[3]);
        void addSubRegionRowSolution();
    private:
        Table*                      m_pTable;
        int                         m_baseRow;
        SubRegion*                  m_subRegions[3];
        SubRegionRowSolutionList    m_solutions;
        int                         m_currentSolutionIndex;
    };

    SubRegionRow::SubRegionRow()
        : m_pTable(nullptr), m_baseRow(0), m_currentSolutionIndex(0)
    {
        for (int i = 0; i < 3; i++)
        {
            m_subRegions[i] = nullptr;
        }
    }

    void SubRegionRow::setSubRegion(Table* pTable, int nBaseRow, SubRegion* p1, SubRegion* p2, SubRegion* p3)
    {
        m_pTable = pTable;
        m_baseRow = nBaseRow;
        m_subRegions[0] = p1;
        m_subRegions[1] = p2;
        m_subRegions[2] = p3;
    }

    bool SubRegionRow::solveByPermutation()
    {
        bool bHasSolution = false;

        for (int i = 0; i < 3; i++)
        {
            m_subRegions[i]->setCurrentSolutionIndex(0);
            m_subRegions[i]->takeCurrentSolution();
        }

        if (verify())
        {
            bHasSolution = true;
            addSubRegionRowSolution();
        }

        while (GetNextPermutation(m_subRegions))
        {
            if (verify())
            {
                bHasSolution = true;
                addSubRegionRowSolution();
            }
        }

        if (!bHasSolution)
            printf("we don't find a solution.\n");

        return bHasSolution;
    }

    bool SubRegionRow::verify()
    {
        for (int i = 0; i < 3; i++)
        {
            // in each row the number 1 ~ 9 should appear and only appear once
            int numAppearnce[9];
            for (int n = 0; n < 9; n++)
                numAppearnce[n] = 0;

            for (int j = 0; j < 9; j++)
            {
                int cellValue = m_pTable->getCell(m_baseRow + i, j).value();
                numAppearnce[cellValue - 1]++;
            }

            for (int n = 0; n < 9; n++)
            {
                if (numAppearnce[n] != 1)
                    return false;
            }
        }

        return true;
    }

    bool SubRegionRow::GetNextPermutation(SubRegion* regions[3])
    {
        for (int i = 0; i < 3; i++)
        {
            bool bIsSolutionValid = regions[i]->takeNextSolution();
            if (bIsSolutionValid)
                return true;
        }

        return false;
    }

    void SubRegionRow::addSubRegionRowSolution()
    {
        SubRegionRowSolution sol(m_subRegions[0]->currentSolutionIndex(),
            m_subRegions[1]->currentSolutionIndex(),
            m_subRegions[2]->currentSolutionIndex());

        m_solutions.push_back(sol);
    }

    void SubRegionRow::takeCurrentSolution()
    {
        SubRegionRowSolution sol = m_solutions[m_currentSolutionIndex];
        m_subRegions[0]->setCurrentSolutionIndex(std::get<0>(sol));
        m_subRegions[0]->takeCurrentSolution();

        m_subRegions[1]->setCurrentSolutionIndex(std::get<1>(sol));
        m_subRegions[1]->takeCurrentSolution();

        m_subRegions[2]->setCurrentSolutionIndex(std::get<2>(sol));
        m_subRegions[2]->takeCurrentSolution();
    }

    bool SubRegionRow::takeNextSolution()
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

    bool GetNextPermutation(SubRegionRow subRegionRows[3])
    {
        for (int i = 0; i < 3; i++)
        {
            bool bHasSolution = subRegionRows[i].takeNextSolution();
            if (bHasSolution)
                return true;
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

        // divide the sub regions into 3 sub region rows
        bHasSolution = false;

        SubRegionRow subRegionRows[3];
        subRegionRows[0].setSubRegion(&table, 0, &subRegions[0][0], &subRegions[0][1], &subRegions[0][2]);
        subRegionRows[1].setSubRegion(&table, 3, &subRegions[1][0], &subRegions[1][1], &subRegions[1][2]);
        subRegionRows[2].setSubRegion(&table, 6, &subRegions[2][0], &subRegions[2][1], &subRegions[2][2]);

        for (int i = 0; i < 3; i++)
        {
            bHasSolution = subRegionRows[i].solveByPermutation();
            if (!bHasSolution)
            {
                printf("we don't find a solution.\n");
                return false;
            }
        }

        // now let all sub region rows take their first solution, verify
        bHasSolution = false;

        for (int i = 0; i < 3; i++)
        {
            subRegionRows[i].setCurrentSolutionIndex(0);
            subRegionRows[i].takeCurrentSolution();
        }

        if (table.veifyAll())
        {
            bHasSolution = true;
            printf("we find a solution.\n");
            table.print();
        }

        // now permutate the solutions in each sub region row to get the solution of the whole table
        while (GetNextPermutation(subRegionRows))
        {
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