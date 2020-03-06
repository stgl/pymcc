/*
 *  Copyright (C) 2003,2005 by Jarno Elonen
 *
 *  TPSDemo is Free Software / Open Source with a very permissive
 *  license:
 *
 *  Permission to use, copy, modify, distribute and sell this software
 *  and its documentation for any purpose is hereby granted without fee,
 *  provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear
 *  in supporting documentation.  The authors make no representations
 *  about the suitability of this software for any purpose.
 *  It is provided "as is" without express or implied warranty.
 */

#include <boost/numeric/ublas/matrix.hpp>

#include "spline.h"
#include "ludecomposition.h"
#include "solvers.h"

#include <fstream>
#include <vector>
#include <cmath>

using namespace boost::numeric::ublas;

//-----------------------------------------------------------------------------

static double tps_base_func(double r)
{
  if ( r == 0.0 )
    return 0.0;
  else
    return r*r * log(r);
}

//-----------------------------------------------------------------------------

/*
 *  Calculate Thin Plate Spline (TPS) weights from
 *  control points.
 */
tpsdemo::Spline::Spline(const std::vector<Vec> & control_pts, double regularization, int idx)
  : p(control_pts.size()),
    control_points(control_pts),
    mtx_v(p+3, 1),
    //mtx_v(p, 1),
    mtx_orig_k(p, p)
{
  // You We need at least 3 points to define a plane
  if ( control_points.size() < 3 )
    throw std::runtime_error("need at least 3 points for thin plate spline");

  int id_number = rand() % 1000;

  //unsigned p = control_points.size();

  // Allocate the matrix and vector
  matrix<double> mtx_l(p+3, p+3);
  //matrix<double> mtx_l(p, p);
  //matrix<double> mtx_v(p+3, 1);
  //matrix<double> mtx_orig_k(p, p);

  // Fill K (p x p, upper left of L) and calculate
  // mean edge length from control points
  //
  // K is symmetrical so we really have to
  // calculate only about half of the coefficients.
  double a = 0.0;
  for ( unsigned i=0; i<p; ++i )
  {
    for ( unsigned j=i+1; j<p; ++j )
    {
      Vec pt_i = control_points[i];
      Vec pt_j = control_points[j];
      pt_i.y = pt_j.y = 0;
      double elen = (pt_i - pt_j).len();
      mtx_l(i,j) = mtx_l(j,i) =
        mtx_orig_k(i,j) = mtx_orig_k(j,i) =
          tps_base_func(elen);
      a += elen * 2; // same for upper & lower tri
    }
  }
  a /= (double)(p*p);

  // Fill the rest of L
  for ( unsigned i=0; i<p; ++i )
  {
    // TODO: Select reg. parameter 
    // diagonal: reqularization parameters (lambda * a^2)
    mtx_l(i,i) = mtx_orig_k(i,i) =
      regularization * (a*a);

    // P (p x 3, upper right)
    mtx_l(i, p+0) = 1.0;
    mtx_l(i, p+1) = control_points[i].x;
    mtx_l(i, p+2) = control_points[i].z;

    // P transposed (3 x p, bottom left)
    mtx_l(p+0, i) = 1.0;
    mtx_l(p+1, i) = control_points[i].x;
    mtx_l(p+2, i) = control_points[i].z;
  }

  // O (3 x 3, lower right)
  for ( unsigned i=p; i<p+3; ++i )
    for ( unsigned j=p; j<p+3; ++j )
      mtx_l(i,j) = 0.0;
  
  // Fill the right hand vector V
  for ( unsigned i=0; i<p; ++i )
    mtx_v(i,0) = control_points[i].y;

  mtx_v(p+0, 0) = mtx_v(p+1, 0) = mtx_v(p+2, 0) = 0.0;

  std::ofstream dat1("matrix_examples/L" + std::to_string(idx) + ".txt", std::ofstream::out);
  for (int i=0; i < p+3; ++i) {
      for (int j=0; j < p+2; ++j) {
        dat1 << mtx_l(i, j) << ", "; 
      }
      dat1 << mtx_l(i, p+3) << std::endl;
  }
  dat1.close();

  std::ofstream dat2("matrix_examples/v" + std::to_string(idx) + ".txt", std::ofstream::out);
  for (int i=0; i < p+3; ++i) {
    dat2 << mtx_v(i, 0) << ", ";
  }
  dat2.close();

  // Solve the linear system "inplace"
  int code = 0;
  code = LU_Solve(mtx_l, mtx_v);

  // Iterative methods (Not in place)
  //mtx_v = Gauss_Seidel_Solve(mtx_l, mtx_v, 1000);
  //mtx_v = SOR_Solve(mtx_l, mtx_v, 1000, 0.5);
  //mtx_v = CG_Solve(mtx_l, mtx_v, 1e-10);
  
  if (code != 0)
  {
    throw SingularMatrixError();
  }

  /*
  // Example (from Wikipedia)
  std::cout << "TEST" << std::endl;
  matrix<double> A(2, 2);
  matrix<double> b(2, 1);

  A(0, 0) = 4;
  A(0, 1) = 1;
  A(1, 0) = 1;
  A(1, 1) = 3;

  b(0, 0) = 1;
  b(1, 0) = 2;

  matrix<double> x = CG_Solve(A, b, 1e-10);
  for ( int i = 0; i < 2; ++i) {
    std::cout << "x[" << i << "] = " << x(i, 0) << std::endl;
  }
  // Solution is [0.0909, 0.6364] 
  */
}

//-----------------------------------------------------------------------------

double tpsdemo::Spline::interpolate_height(double x, double z) const
{
  double h = mtx_v(p+0, 0) + mtx_v(p+1, 0)*x + mtx_v(p+2, 0)*z;

  Vec pt_i, pt_cur(x,0,z);
  for ( unsigned i=0; i<p; ++i )
  {
    pt_i = control_points[i];
    pt_i.y = 0;
    h += mtx_v(i,0) * tps_base_func( ( pt_i - pt_cur ).len());
  }
  return h;
}

//-----------------------------------------------------------------------------

double tpsdemo::Spline::compute_bending_energy() const
{
  matrix<double> w( p, 1 );
  for ( unsigned i=0; i<p; ++i )
    w(i,0) = mtx_v(i,0);
  matrix<double> be = prod( prod<matrix<double> >( trans(w), mtx_orig_k ), w );
  return be(0,0);
}
