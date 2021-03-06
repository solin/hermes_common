// Copyright (c) 2009 hp-FEM group at the University of Nevada, Reno (UNR).
// Distributed under the terms of the BSD license (see the LICENSE
// file for the exact terms).
// Email: hermes1d@googlegroups.com, home page: http://hpfem.org/

#ifndef __HERMES_COMMON_MATRIX_H
#define __HERMES_COMMON_MATRIX_H

#include <math.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <math.h>
#include <string.h>
#include <complex>
#include <map>

typedef std::complex<double> cplx;
class Matrix;

#include "solvers.h"

// printf debug information about the stiffness/Jacobi matrix
#define _error(x) throw std::runtime_error(x)

class CooMatrix;
class CSRMatrix;
class CSCMatrix;

/// Creates a new (full) matrix with m rows and n columns with entries of the type T.
/// The entries can be accessed by matrix[i][j]. To delete the matrix, just
/// do "delete matrix".
template<typename T>
T** _new_matrix(int m, int n = 0)
{
    if (!n) n = m;
    T** vec = (T**) new char[sizeof(T*)*m + sizeof(T)*m*n];
    if (vec == NULL) _error("Out of memory.");
    T* row = (T*) (vec + m);
    for (int i = 0; i < m; i++, row += n)
        vec[i] = row;
    return vec;
}

class Matrix {
public:
    Matrix() {}
    virtual ~Matrix() {}

    inline virtual void init() { this->complex = false; free_data(); }
    virtual void free_data() = 0;

    virtual void set_zero() = 0;

    inline virtual int get_size() { return this->size; }
    inline bool is_complex() { return this->complex; }
    virtual void print() = 0;

    virtual void add(int m, int n, double v) = 0;
    virtual void add(int m, int n, cplx v)
    {
        _error("internal error: add(int, int, cplx) not implemented.");
    }
    virtual void add_block(int *iidx, int ilen, int *jidx, int jlen, double** mat)
    {
        for (int i = 0; i < ilen; i++)
            for (int j=0; j < jlen; j++)
                if (iidx[i] >= 0 && jidx[j] >= 0)
                    this->add(iidx[i], jidx[j], mat[i][j]);
    }
    virtual void add_block(int *iidx, int ilen, int *jidx, int jlen, cplx** mat)
    {
        for (int i = 0; i < ilen; i++)
            for (int j=0; j < jlen; j++)
                if (iidx[i] >= 0 && jidx[j] >= 0)
                    this->add(iidx[i], jidx[j], mat[i][j]);
    }
    virtual double get(int m, int n) = 0;
    virtual cplx get_cplx(int m, int n)
    {
        _error("internal error: get_cplx() not implemented.");
    }

    virtual void copy_into(Matrix *m) = 0;

    virtual void times_vector(double* vec, double* result, int rank)
    {
        _error("internal error: times_vector() not implemented.");
    }

protected:
    int size;
    bool complex;
};

// **********************************************************************************************************

class CooMatrix : public Matrix {
public:
    CooMatrix(bool complex = false);
    CooMatrix(int size, bool complex = false);
    CooMatrix(Matrix *m);
    CooMatrix(CooMatrix *m);
    CooMatrix(CSRMatrix *m);
    CooMatrix(CSCMatrix *m);
    ~CooMatrix();

    inline virtual void init() { this->complex = false; free_data(); }
    virtual void free_data();

    virtual void set_zero()
    {
        _error("CooMatrix::set_zero() not implemented.");
    }

    virtual int get_nnz();
    virtual void print();

    void add_from_csr(CSRMatrix *m);
    void add_from_csc(CSCMatrix *m);

    virtual void add(int m, int n, double v);
    virtual void add(int m, int n, cplx v);
    void get_row_col_data(int *row, int *col, double *data);
    void get_row_col_data(int *row, int *col, cplx *data);
    void get_row_col_data(int *row, int *col, double *data_real, double *data_imag);

    virtual void copy_into(Matrix *m);

    inline virtual double get(int m, int n) { return A[m][n]; }

    virtual void times_vector(double* vec, double* result, int rank);

protected:
    std::map<size_t, std::map<size_t, double> > A;
    std::map<size_t, std::map<size_t, cplx> > A_cplx;
};

// **********************************************************************************************************

class DenseMatrix : public Matrix
{
public:
    DenseMatrix(Matrix *m);
    DenseMatrix(CooMatrix *m);
    DenseMatrix(int size, bool is_complex = false);
    ~DenseMatrix();

    virtual void init();

    virtual void free_data();
    virtual void set_zero();

    inline virtual void add(int m, int n, double v) { this->A[m][n] += v; }
    inline virtual void add(int m, int n, cplx v) { this->A_cplx[m][n] += v; }

    void add_from_coo(CooMatrix *m);

    inline virtual double get(int m, int n) { return this->A[m][n]; }

    virtual void copy_into(Matrix *m)
    {
        m->free_data();
        for (int i = 0; i < this->size; i++)
        {
            for (int j = 0; j < this->size; j++)
            {
                for (int j = 0; j < this->size; j++)
                {
                    if (complex)
                        if (std::abs(A_cplx[i][j]) > 1e-12)
                             m->add(i, j, A_cplx[i][j]);
                    else
                        if (fabs(A[i][j]) > 1e-12)
                            m->add(i, j, A[i][j]);
                }
            }
        }
    }

    virtual int get_nnz();

    virtual void print();

    // Return the internal matrix.
    inline double **get_A() { return this->A; }
    inline cplx **get_A_cplx() { return this->A_cplx; }

private:
    double **A;
    cplx **A_cplx;
};

// **********************************************************************************************************

class CSRMatrix : public Matrix
{
public:
    CSRMatrix(int size);
    CSRMatrix(Matrix *m);
    CSRMatrix(CooMatrix *m);
    CSRMatrix(CSCMatrix *m);
    CSRMatrix(DenseMatrix *m);
    ~CSRMatrix();

    virtual void init();
    virtual void free_data();

    virtual void set_zero()
    {
        _error("CSRMatrix::set_zero() not implemented.");
    }

    void add_from_dense(DenseMatrix *m);
    void add_from_coo(CooMatrix *m);
    void add_from_csc(CSCMatrix *m);

    virtual void add(int m, int n, double v)
    {
        _error("CSR matrix add() not implemented.");
    }

    virtual double get(int m, int n)
    {
        _error("CSR matrix get() not implemented.");
    }

    virtual int get_size()
    {
        return this->size;
    }
    inline int get_nnz() { return this->nnz; }
    virtual void copy_into(Matrix *m) {
        _error("CSR matrix copy_into() not implemented.");
    }

    virtual void print();

    inline int *get_Ap() { return this->Ap; }
    inline int *get_Ai() { return this->Ai; }
    inline double *get_Ax() { return this->Ax; }
    inline cplx *get_Ax_cplx() { return this->Ax_cplx; }

private:
    // number of non-zeros
    int nnz;

    int *Ap;
    int *Ai;
    double *Ax;
    cplx *Ax_cplx;
};

// **********************************************************************************************************

class CSCMatrix : public Matrix
{
public:
    CSCMatrix(int size);
    CSCMatrix(Matrix *m);
    CSCMatrix(DenseMatrix *m);
    CSCMatrix(CooMatrix *m);
    CSCMatrix(CSRMatrix *m);
    CSCMatrix(int size, int nnz, int *Ap, int *Ai, double *Ax);
    CSCMatrix(int size, int nnz, int *Ap, int *Ai, cplx *Ax_cplx);
    ~CSCMatrix();

    virtual void init();
    virtual void free_data();

    virtual void set_zero()
    {
        _error("CSCMatrix::set_zero() not implemented.");
    }

    void add_from_dense(DenseMatrix *m);
    void add_from_coo(CooMatrix *m);
    void add_from_csr(CSRMatrix *m);

    virtual void add(int m, int n, double v)
    {
        _error("CSC matrix add() not implemented.");
    }
    virtual double get(int m, int n)
    {
        _error("CSC matrix get() not implemented.");
    }

    virtual int get_size()
    {
        return this->size;
    }
    inline int get_nnz() { return this->nnz;
    }
    virtual void copy_into(Matrix *m)
    {
        _error("CSC matrix copy_into() not implemented.");
    }

    virtual void print();

    inline int *get_Ap() { return this->Ap; }
    inline int *get_Ai() { return this->Ai; }
    inline double *get_Ax() { return this->Ax; }
    inline cplx *get_Ax_cplx() { return this->Ax_cplx; }

private:
    // number of non-zeros
    int nnz;

    double *Ax;
    cplx *Ax_cplx;

    int *Ap;
    int *Ai;
};

// print vector - int
void print_vector(const char *label, int *value, int size);
// print vector - double
void print_vector(const char *label, double *value, int size);
// print vector - cplx
void print_vector(const char *label, cplx *value, int size);

template<typename T>
void dense_to_coo(int size, int nnz, T **Ad, int *row, int *col, T *A);
template<typename T>
void coo_to_csr(int size, int nnz, int *row, int *col, T *A, int *Ap, int *Ai, T *Ax);
template<typename T>
void coo_to_csc(int size, int nnz, int *row, int *col, T *A, int *Ap, int *Ai, T *Ax);
template<typename T>
void csr_to_csc(int size, int nnz, int *Arp, int *Ari, T *Arx, int *Acp, int *Aci, T *Acx);
template<typename T>
void csc_to_csr(int size, int nnz, int *Acp, int *Aci, T *Acx, int *Arp, int *Ari, T *Arx);
template<typename T>
void csc_to_coo(int size, int nnz, int *Ap, int *Ai, T *Ax, int *row, int *col, T *A);
template<typename T>
void csr_to_coo(int size, int nnz, int *Ap, int *Ai, T *Ax, int *row, int *col, T *A);

// matrix vector multiplication
void mat_dot(Matrix *A, double *x, double *result, int n_dof);
// vector vector multiplication
double vec_dot(double *r, double *s, int n_dof);

void ludcmp(double** a, int n, int* indx, double* d);
void lubksb(double** a, int n, int* indx, double* b);

#endif
