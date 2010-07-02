// This file is part of Hermes3D
//
// Copyright (c) 2009 hp-FEM group at the University of Nevada, Reno (UNR).
// Email: hpfem-group@unr.edu, home page: http://hpfem.org/.
//
// This file was written by:
// - David Andrs
// - Pavel Kus
//
// Hermes3D is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
//
// Hermes3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hermes3D; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef __COMMON_MATRIX_H
#define __COMMON_MATRIX_H

#include "common.h"
#include "common/error.h"

enum EMatrixDumpFormat {
	DF_MATLAB_SPARSE,
	DF_PLAIN_ASCII,
	DF_HERMES_BIN,
	DF_NATIVE					// native format for the linear solver
};

class CommonMatrix {
public:
        virtual ~CommonMatrix() { }

	/// allocate the memory for stiffness matrix and right-hand side
	virtual void alloc() = 0;

	/// free the memory associated with stiffness matrix and right-hand side
	virtual void free() = 0;

	/// Get the value from a position
	/// @return the value form the specified position
	/// @param[in] m - the number of row
	/// @param[in] n - the number of column
	virtual scalar get(int m, int n) = 0;

	virtual int get_size() = 0;

	/// Zero the matrix
	virtual void zero() = 0;

	/// update the stiffness matrix
	///
	/// @param[in] m    - the row where to update
	/// @param[in] n    - the column where to update
	/// @param[in] v    - value
	virtual void add(int m, int n, scalar v) = 0;

	/// update the stiffness matrix
	///
	/// @param[in] m         - number of rows of given block
	/// @param[in] n         - number of columns of given block
	/// @param[in] matrix    - block of values
	/// @param[in] rows      - array with row indexes
	/// @param[in] cols      - array with column indexes
	virtual void add(int m, int n, scalar **mat, int *rows, int *cols) = 0;

	/// dumping matrix and right-hand side
	///
	virtual bool dump(FILE *file, const char *var_name, EMatrixDumpFormat = DF_MATLAB_SPARSE) = 0;

	virtual int get_matrix_size() const = 0;
};

class CommonSparseMatrix : public CommonMatrix {
public:
        CommonSparseMatrix();
        virtual ~CommonSparseMatrix();

	/// prepare memory
	///
	/// @param[in] ndofs - number of unknowns
	virtual void prealloc(int n);

	/// add indices of nonzero matrix element
	///
	/// @param[in] row  - row index
	/// @param[in] col  - column index
	virtual void pre_add_ij(int row, int col);

	virtual void finish() { }

	virtual int get_size() { return size; }

	/// Return the number of entries in a specified row
	///
	/// @param[in] row - index of the row
	/// @return - the number of entries in the row 'row'
	virtual int get_num_row_entries(int row) { return -1; }

	/// Extract the copy of a row
	///
	/// @param[in] row - global row to extract
	/// @param[in] len - length of 'vals' and 'idxs' arrays.
	/// @param[out] n_entries - number of nonzero entries extracted.
	/// @param[out] vals - extracted values for this row.
	/// @param[out] idxs - extracted global column indices for the corresponding values.
	virtual void extract_row_copy(int row, int len, int &n_entries, double *vals, int *idxs) { }

	/// Return the number of entries in a specified column
	///
	/// @param[in] row - index of the column
	/// @return - the number of entries in the column 'col'
	virtual int get_num_col_entries(int col) { return -1; }

	/// Extract the copy of a column
	///
	/// @param[in] row - global column to extract
	/// @param[in] len - length of 'vals' and 'idxs' arrays.
	/// @param[out] n_entries - number of nonzero entries extracted.
	/// @param[out] vals - extracted values for this column.
	/// @param[out] idxs - extracted global row indices for the corresponding values.
	virtual void extract_col_copy(int col, int len, int &n_entries, double *vals, int *idxs) { }

	virtual double get_fill_in() const = 0;

	unsigned row_storage:1;
	unsigned col_storage:1;

protected:
	static const int PAGE_SIZE = 62;

	struct Page {
		int count;
		int idx[PAGE_SIZE];
		Page *next;
	};

	int size;							// number of unknowns
	Page **pages;

	int sort_and_store_indices(Page *page, int *buffer, int *max);
	int get_num_indices();

	// mem stat
	int mem_size;
};

class Vector {
public:
	virtual ~Vector() { }

	/// allocate memory for storing ndofs elements
	///
	/// @param[in] ndofs - number of elements of the vector
	virtual void alloc(int ndofs) = 0;
	/// free the memory
	virtual void free() = 0;
	// finish the assembly of the vector
	virtual void finish() { }

	/// Get the value from a position
	/// @return the value form the specified index
	/// @param[in] idx - index which to obtain the value from
	virtual scalar get(int idx) = 0;

	/// Extract vector values into user-provided array.
	/// @param[out] v - array which will contain extracted values
	virtual void extract(scalar *v) const = 0;

	/// Zero the vector
	virtual void zero() = 0;

	/// set the entry on a specified position
	///
	/// @param[in] idx - indices where to update
	/// @param[in] y   - value
	virtual void set(int idx, scalar y) = 0;

	/// update element on the specified position
	///
	/// @param[in] idx - indices where to update
	/// @param[in] y   - value
	virtual void add(int idx, scalar y) = 0;

	/// update subset of the elements
	///
	/// @param[in] n   - number of positions to update
	/// @param[in] idx - indices where to update
	/// @param[in] y   - values
	virtual void add(int n, int *idx, scalar *y) = 0;

	virtual bool dump(FILE *file, const char *var_name, EMatrixDumpFormat = DF_MATLAB_SPARSE) = 0;

protected:
	int size;
};

#endif
