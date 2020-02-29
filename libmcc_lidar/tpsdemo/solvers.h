#ifndef SOLVERS_H
#define SOLVERS_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include "spline.h"

// Solve a linear system a*x=b using the Gauss Seidel method 
//
// Returns the solution vector x
//
// Throws an exception if
// 1. a and b do not have the same number of rows
// 2. a has zero entires on its main diagonal
//
template <typename T> int Gauss_Seidel_Solve(
  boost::numeric::ublas::matrix<T>& a,
  boost::numeric::ublas::matrix<T>& b,
  int n_iter)
{
  
  int nrows = a.size1();
  boost::numeric::ublas::zero_matrix<T> x(nrows, 1);
  boost::numeric::ublas::zero_matrix<T> n(nrows, 1);

  if (a.size1() != b.size1())
    throw SingularMatrixError();

  // Check singluarity
  for (int j = 0; j < n; ++j)
    if (a(j,j) == 0)
      throw SingularMatrixError();

  while (n_iter > 0) {
    for (i = 0; i < nrows; i++) {
      n[i] = (b[i] / a(i,i));
      for (j = 0; j < nrows; j++) {
        if (j == i)
          continue;
        n[i] = n[i] - ((a(i,j) / a(i,i)) * x[j]);
        x[i] = n[i];
      }
    }
    n_iter--;
  }

  return x;
}

// Solve a linear system a*x=b using Successive Overrelaxation 
//
// Returns the solution vector x
//
// Throws an exception if
// 1. a and b do not have the same number of rows
// 2. a has zero entires on its main diagonal
//
template <typename T> int SOR_Solve(
  boost::numeric::ublas::matrix<T>& a,
  boost::numeric::ublas::matrix<T>& b,
  int n_iter,
  double w)
{
  
  int nrows = a.size1();
  boost::numeric::ublas::zero_matrix<T> x(nrows, 1);
  boost::numeric::ublas::zero_matrix<T> x_new(nrows, 1);

  if (a.size1() != b.size1())
    throw SingularMatrixError();

  // Check singluarity
  for (int j = 0; j < n; ++j)
    if (a(j,j) == 0)
      throw SingularMatrixError();

  while (n_iter > 0) {
    for (i = 0; i < nrows; i++) {
      x_new[i] = (b[i] / a(i,i));
      for (j = 0; j < nrows; j++) {
        if (j == i)
          continue;
        x_new[i] = x_new[i] - ((a(i,j) / a(i,i)) * x[j]);
        // TODO: Implement SOR weighted update
      }
    }
    // TODO: Convergence condition?
    n_iter--;
  }

  return x;
}

#endif // SOLVERS_H

