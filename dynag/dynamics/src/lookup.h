/*
* DynaG Dynamics C++ / Lookup
*
* Lookup class for implementing a lookup table for 1D or 2D tables.
* 
* It is based on https://jsbsim-team.github.io/jsbsim/FGTable_8cpp_source.html.
*
* Author : @MGokcayK
*
* C.Date : 21/09/2021
* Update : 21/09/2021 - Class Initialization & Implementation // @MGokcayK
*/

#pragma once

#include <Eigen/Eigen/Dense>
#include <iostream>

class LookUpTable
{
	// Table dims and params
	int nRows, nCols, rowCounter, colCounter, lastRowIndex, lastColIndex;

	// Table data
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> data;

public:

	// Construct table w.r.t row and column number.
	LookUpTable(const int& Rows = 1, const int& Cols = 1);

	// Shift operator to table which add the values into the table.
	LookUpTable& operator<<(const float& n);

	// Getting element w.r.t key in 1D table.
	float get_value_1D(const float& key);

	// Getting element w.r.t row and column key in 2D table.
	float get_value_2D(const float& rowKey, const float& colKey);
	
	// Print the table.
	void print_table();

};