#ifndef __GEOM_H
#define __GEOM_H

/*
 *  Minimal set of linear algebra primitives need for this program.  Also,
 *  the vector class uses double-precision for its position coordinates.
 */

/*
 *  One file long C++ library of linear algebra primitives for
 *  simple 3D programs
 *
 *  Copyright (C) 2001-2003 by Jarno Elonen
 *
 *  Permission to use, copy, modify, distribute and sell this software
 *  and its documentation for any purpose is hereby granted without fee,
 *  provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear
 *  in supporting documentation.  The authors make no representations
 *  about the suitability of this software for any purpose.
 *  It is provided "as is" without express or implied warranty.
 */

#include <cmath>

// =========================================
// 3-vector
// =========================================
class Vec
{
public:

  // Position
  double x, y, z;

  // Default constructor
  Vec()
  : x( 0 ), y( 0 ), z( 0 ) {}

  // Element constructor
  Vec( double x, double y, double z )
  : x( x ), y( y ), z( z ) {}

  // Norm (len^2)
  inline double norm() const { return x*x + y*y + z*z; }

  // Length of the vector
  inline double len() const { return sqrt(norm()); }

  Vec &operator -= ( const Vec &src ) { x -= src.x; y -= src.y; z -= src.z; return *this; }
  Vec operator - ( const Vec &src ) const { Vec tmp( *this ); return ( tmp -= src ); }

};

  // Cross product
  inline Vec cross( const Vec &a, const Vec &b )
  { return Vec( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x ); }

#endif
