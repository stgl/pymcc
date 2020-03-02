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
template <typename T> boost::numeric::ublas::matrix<T> Gauss_Seidel_Solve(
  boost::numeric::ublas::matrix<T>& a,
  boost::numeric::ublas::matrix<T>& b,
  int n_iter)
{
  std::cout << "SOLVER: Gauss-Seidel" << std::endl;
  
  int nrows = a.size1();
  boost::numeric::ublas::matrix<T> x(nrows, 1);
  boost::numeric::ublas::matrix<T> n(nrows, 1);

  if (a.size1() != b.size1())
    throw tpsdemo::SingularMatrixError();

  // Check singluarity
  for (int j = 0; j < nrows; ++j)
    if (a(j, j) == 0)
      throw tpsdemo::SingularMatrixError();

  while (n_iter > 0) {
    for (int i = 0; i < nrows; i++) {
      n(i, 1) = (b(i, 1) / a(i,i));
      for (int j = 0; j < nrows; j++) {
        if (j == i)
          continue;
        n(i, 1) = n(i, 1) - ((a(i,j) / a(i,i)) * x(j, 1));
        x(i, 1) = n(i, 1);
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
// TODO: Implement this
//
template <typename T> boost::numeric::ublas::matrix<T> SOR_Solve(
  boost::numeric::ublas::matrix<T>& a,
  boost::numeric::ublas::matrix<T>& b,
  int n_iter,
  double w)
{
  std::cout << "SOLVER: Successive Overrelaxation" << std::endl;
  
  int nrows = a.size1();
  boost::numeric::ublas::matrix<T> x(nrows, 1);
  boost::numeric::ublas::matrix<T> x_new(nrows, 1);

  if (a.size1() != b.size1())
    throw tpsdemo::SingularMatrixError();

  // Check singluarity
  for (int j = 0; j < nrows; ++j)
    if (a(j, j) == 0)
      throw tpsdemo::SingularMatrixError();

  while (n_iter > 0) {
    for (int i = 0; i < nrows; i++) {
      x_new(i, 1) = (b(i, 1) / a(i,i));
      for (int j = 0; j < nrows; j++) {
        if (j == i)
          continue;
        // TODO: Implement SOR weighted update
        x_new(i, 1) = x_new(i, 1) - ((a(i,j) / a(i,i)) * x(j, 1));
        x(i, 1) = x_new(i, 1);
      }
    }
    // TODO: Convergence condition?
    n_iter--;
  }

  return x;
}

#endif // SOLVERS_H

