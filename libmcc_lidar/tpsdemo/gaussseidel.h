#ifndef GAUSSSEIDEL_H
#define GAUSSSEIDEL_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include "spline.h"

// Solve a linear equation system a*x=b using the Gauss Seidel method 
//
// Possible return values:
//  0=success
//  1=singular matrix
//  2=a.rows != b.rows
//
template <typename T> int Gauss_Seidel_Solve(
  boost::numeric::ublas::matrix<T>& a,
  boost::numeric::ublas::matrix<T>& b,
  int n_iter)
{
  
  int nrows = a.size1(), ncols = a.size2();
  boost::numeric::ublas::zero_matrix<T> x(nrows, 1);
  boost::numeric::ublas::zero_matrix<T> n(nrows, 1);

  if (a.size1() != b.size1())
    throw SingularMatrixError();  

  while (n_iter > 0) {
    for (i = 0; i < p; i++) {
      n[i] = (b[i] / a(i,i));
      for (j = 0; j < p; j++) {
        if (j == i)
          continue;
        n[i] = n[i] - ((a(i,j) / a(i,i)) * x[j]);
        x[i] = n[i];
      }
    }
    n_iter--;
  }

  // Check singluarity
  for (int j = 0; j < n; ++j)
    if (a(j,j) == 0)
      throw SingularMatrixError();  

  return x;
}

#endif // GAUSSSEIDEL_H

