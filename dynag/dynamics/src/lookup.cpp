#include "lookup.h"

LookUpTable::LookUpTable(const int& Rows, const int& Cols)
{
	nRows = Rows;
	nCols = Cols;

	if (nCols == 1) // Make sure table is 1D
	{
		rowCounter = 1;
		colCounter = 0;
	}
	else
	{
		rowCounter = 0;
		colCounter = 1;
	}

	data.resize(nRows + 1, nCols + 1);
	data.setConstant(0); 

	lastRowIndex = 2;
	lastColIndex = 2;
}

LookUpTable& LookUpTable::operator<<(const float& n)
{
	if (rowCounter <= nRows)
	{
		data(rowCounter, colCounter) = n;
		if (colCounter >= nCols)
		{
			colCounter = 0;
			rowCounter += 1;
		}
		else colCounter += 1;
	}
	else
	{
		std::cout << " There is no space to fill table with data `" << n << "` !"  << std::endl;
	}

	return *this;
}

float LookUpTable::get_value_1D(const float& key)
{
	int r = lastRowIndex;

	// if key is not in the table, do not extrapolate. Return the end value of table.
	if (key <= data(1, 0))
	{
		lastRowIndex = 2;
		return data(1, 1);
	}
	else if (key >= data(nRows, 0))
	{
		lastRowIndex = nRows;
		return data(nRows, 0);
	}

	// if key is middle of the table, search it.

	while ((r > 2) && (data(r - 1, 0) > key))
	{
		r -= 1;
	}
	while (r < nRows && data(r, 0) < key) r += 1;

	lastRowIndex = r;

	float span = data(r, 0) - data(r - 1, 0);
	float factor = 0.0;
	if (span != 0.0f)
	{
		factor = (key - data(r - 1, 0)) / span;
		factor = (factor > 1.0f) ? 1.0f : factor;
	}
	else factor = 1.0f;

	std::cout << factor * (data(r, 1) - data(r - 1, 1)) + data(r - 1, 1) << std::endl;

	return factor * (data(r, 1) - data(r - 1, 1)) + data(r - 1, 1);
}

float LookUpTable::get_value_2D(const float& rowKey, const float& colKey)
{
	int r = lastRowIndex;
	int c = lastColIndex;

	while (r > 2 && data(r - 1, 0) > rowKey) r -= 1;
	while (r < nRows && data(r, 0) < rowKey) r += 1;

	while (c > 2 && data(0, c - 1) > colKey) c -= 1;
	while (c < nCols && data(0, c) < colKey) c += 1;

	lastRowIndex = r;
	lastColIndex = c;

	float rFactor = (rowKey - data(r - 1, 0)) / (data(r, 0) - data(r - 1, 0));
	float cFactor = (colKey - data(0, c - 1)) / (data(0, c) - data(0, c - 1));

	if (rFactor > 1.0f) rFactor = 1.0f;
	else if (rFactor < 0.0f) rFactor = 0.0f;

	if (cFactor > 1.0f) cFactor = 1.0f;
	else if (cFactor < 0.0f) cFactor = 0.0f;

	float col1temp = rFactor * (data(r, c - 1) - data(r - 1, c - 1)) + data(r - 1, c - 1);
	float col2temp = rFactor * (data(r, c) - data(r - 1, c)) + data(r - 1, c);

	return col1temp + cFactor * (col2temp - col1temp);
}

void LookUpTable::print_table()
{
	std::cout << "Table with " << nRows << " rows & " << nCols << " cols : " << std::endl;
	std::cout << data << std::endl;
}
