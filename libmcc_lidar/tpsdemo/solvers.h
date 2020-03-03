#ifndef SOLVERS_H
#define SOLVERS_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include "spline.h"

// Solve a linear system a*x=b using Conjugate Gradient method 
//
// Returns the solution vector x
//
// Throws an exception if
// 1. a and b do not have the same number of rows
// 2. a is not positive semi-definite
//
// Adapted from https://en.wikipedia.org/wiki/Conjugate_gradient_method
//
template <typename T> boost::numeric::ublas::matrix<T> CG_Solve(
  boost::numeric::ublas::matrix<T> a,
  boost::numeric::ublas::matrix<T> b,
  double tol)
{
  std::cout << "SOLVER: Conjugate gradient" << std::endl;

  using namespace boost::numeric::ublas;
  
  const double EPS = 1.0e-10;
  int nrows = a.size1();

  if (a.size1() != b.size1()) {
    throw tpsdemo::SingularMatrixError();
  }

  // TODO: This sucks, use Boost matrix range and sum
  // Check that matrix is semi-positive definite
  double trace = 0;
  for (int i = 0; i < nrows; ++i) {
    trace += a(i, i);
  }
  if (trace < 0) {
    std::cout << "Matrix A is not positive semidefinite" << std::endl;
    throw tpsdemo::SingularMatrixError();
  }
  //std::cout << "Matrix has " << nzeros << " zero, " << npos << " pos, " << nneg << " neg entries on diagonal" << std::endl;

  vector<T> x(b.size1(), 0);
  vector<T> r(b.size1());
  vector<T> r_old(b.size1());
  vector<T> p(b.size1());
  vector<T> Ap(b.size1()); 

  // TODO: This sucks, use Boost matrix-vector copy
  for (int i = 0; i < nrows; ++i) {
    r(i) = b(i, 0);
  }
  p = r;

  int k = 0;

  while (k < nrows)
  {
     r_old = r;                                      // Previous residual
     Ap = prod(a, p);                                // Project onto p

     double alpha = inner_prod(r, r) / std::max(inner_prod(p, Ap), EPS);
     x = x + alpha * p;                             // Next estimate
     r = r - alpha * Ap;                            // Residual 

     if (norm_2( r ) < tol) break;                  // Test convergence

     double beta = inner_prod(r, r) / std::max(inner_prod(r_old, r_old), EPS);
     p = r + beta * p;                              // Next gradient
     k++;
  }
  
  // TODO: This sucks, but have to type cast for compat with codebase
  //       (which expects output (b) to be matrix<T>)
  matrix<T> x_mtx(b.size1(), b.size2());
  for (int i = 0; i < nrows; ++i) {
    x_mtx(i, 0) = x(i);
  }

  return x_mtx;
}

// Solve a linear system a*x=b using the Gauss Seidel method 
//
// Returns the solution vector x
//
// Throws an exception if
// 1. a and b do not have the same number of rows
// 2. a has zero entires on its main diagonal
//
template <typename T> boost::numeric::ublas::matrix<T> Gauss_Seidel_Solve(
  boost::numeric::ublas::matrix<T> a,
  boost::numeric::ublas::matrix<T> b,
  int n_iter)
{
  std::cout << "SOLVER: Gauss-Seidel" << std::endl;

  int nrows = a.size1();
  boost::numeric::ublas::matrix<T> x(b.size1(), b.size2(), 0);
  boost::numeric::ublas::matrix<T> n(b.size1(), b.size2(), 0);

  if (a.size1() != b.size1()) {
    throw tpsdemo::SingularMatrixError();
  }

  // Check singluarity
  int nzeros = 0;
  int npos = 0;
  int nneg = 0;
  for (int j = 0; j < nrows; ++j) {
    if (a(j, j) == 0) {
        nzeros++;
      //throw tpsdemo::SingularMatrixError();
    } else if (a(j, j) > 0) {
        npos++;
    } else {
        nneg++;
    }
  }
  //std::cout << "Matrix has " << nzeros << " zero, " << npos << " pos, " << nneg << " neg entries on diagonal" << std::endl;

  while (n_iter > 0) {
    for (int i = 0; i < nrows; i++) {
      n(i, 0) = (b(i, 0) / a(i, i));
      for (int j = 0; j < nrows; j++) {
        if (j == i)
          continue;
        n(i, 0) = n(i, 0) - ((a(i, j) / a(i, i)) * x(j, 0));
        x(i, 0) = n(i, 0);
      }
    }
    n_iter--;
  }
 
  //std::cout << "DEBUG: Exited GS iterations" << std::endl;

  return x;
}

#endif // SOLVERS_H

