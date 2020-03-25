#ifndef TPS_DEMO_SPLINE_H
#define TPS_DEMO_SPLINE_H

#include <exception>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>

#ifdef VEC_WITH_DOUBLE_COORDINATES
#include "linalg3d-double.h"
#else
#include "linalg3d.h"
#endif

namespace tpsdemo
{
  struct SingularMatrixError : std::runtime_error
  {
    SingularMatrixError()
      : std::runtime_error("singular matrix occured while computing thin plate spline")
    {
    }
  };

  //---------------------------------------------------------------------------

  class Spline
  {
    public:
      // Throws SingularMatrixError if a singular matrix is detected.
      Spline(const std::vector<Vec> & control_pts, double regularization);

      double interpolate_height(double x, double z) const;
      double compute_bending_energy() const;

    private:
      unsigned p;
      const std::vector< Vec > & control_points;
      boost::numeric::ublas::matrix<double> mtx_v;
      boost::numeric::ublas::matrix<double> mtx_orig_k;
  };
}

#endif
