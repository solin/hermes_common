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

#include "common.h"
#include "common_matrix.h"

#include "common/error.h"
#include "common/callstack.h"


#define TINY 1.0e-20


// SparseMatrix ////////////////////////////////////////////////////////////////////////////////////

void qsort_int(int* pbase, size_t total_elems); // defined in qsort.cpp

CommonSparseMatrix::CommonSparseMatrix()
{
	_F_
	size = 0;
	pages = NULL;

	row_storage = false;
	col_storage = false;
}

CommonSparseMatrix::~CommonSparseMatrix()
{
	_F_
	delete [] pages;
}

void CommonSparseMatrix::prealloc(int n)
{
	_F_
	this->size = n;

	pages = new Page *[n];
	MEM_CHECK(pages);
	memset(pages, 0, n * sizeof(Page *));
}

void CommonSparseMatrix::pre_add_ij(int row, int col)
{
	_F_
	if (pages[col] == NULL || pages[col]->count >= PAGE_SIZE) {
		Page *new_page = new Page;
		MEM_CHECK(new_page);
		new_page->count = 0;
		new_page->next = pages[col];
		pages[col] = new_page;
	}
	pages[col]->idx[pages[col]->count++] = row;
}

int CommonSparseMatrix::sort_and_store_indices(Page *page, int *buffer, int *max)
{
	_F_
	// gather all pages in the buffer, deleting them along the way
	int *end = buffer;
	while (page != NULL) {
		memcpy(end, page->idx, sizeof(int) * page->count);
		end += page->count;
		Page *tmp = page;
		page = page->next;
		delete tmp;
	}

	// sort the indices and remove duplicities
	qsort_int(buffer, end - buffer);
	int *q = buffer;
	for (int *p = buffer, last = -1; p < end; p++)
		if (*p != last)
			*q++ = last = *p;

	return q - buffer;
}

int CommonSparseMatrix::get_num_indices()
{
	_F_
	int total = 0;
	for (int i = 0; i < size; i++)
		for (Page *page = pages[i]; page != NULL; page = page->next)
			total += page->count;

	return total;
}
