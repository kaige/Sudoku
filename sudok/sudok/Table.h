#pragma once

namespace KSudoku {

    class Cell {
    public:
        Cell() :mX(0), mY(0), mValue(0), mIsConst(false) {}

        void 	setCoordinate(int i, int j) { mX = i; mY = j; }
        void 	setConst() { mIsConst = true; }
        bool 	isConst() const { return mIsConst; }
        void 	setValue(int v) { mValue = v; }
        void 	resetValue() { if (!mIsConst) mValue = 0; }
        int 	value() const { return mValue; }

        bool 	increaseValue();

        int 	x() const { return mX; }
        int 	y() const { return mY; }

    private:
        int 	mX;
        int 	mY;
        int 	mValue;
        bool 	mIsConst;
    };

    class Table {
    public:
        static Table*  createTable(int inputArray[9][9]);
        void 	print();

        bool 	hasNextCell(const Cell& cell);
        Cell& 	getNextCell(const Cell& cell);
        Cell&   getFirstCell();
        bool 	veifyAll();
        bool 	verifyCell(const Cell& cell);

    private:
        Table();
        void 	initConstCell(int i, int j, int v);
        bool 	verifyNumbers(int v[9]);

    private:
        Cell 	mCells[9][9];
    };

    

};
