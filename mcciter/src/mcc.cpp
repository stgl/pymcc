#include "mcc.h"
#include "PointVector.h"
#include "XyzPoint.h"
#include "XyExtent.h"
#include "IClassificationVector.h"
#include <cmath>
#include <boost/cstdint.hpp>

// Read a vector of points from arrays.
shared_ptr<PointVector> readPoints(double *x,
								   double *y,
								   double *z,
								   int n,
                                   XyExtent &                      xyExtent) {

	(*xyExtent).minX = std::nan("1");
	(*xyExtent).minY = std::nan("1");
	(*xyExtent).maxX = std::nan("1");
	(*xyExtent).maxY = std::nan("1");

	for(int i=0; i < n; i++) {
		shared_ptr<PointVector> pts(new PointVector(n));
		(*pts)[index].setCoordinates(XyzPoint(x[n], y[n], z[n]));
		(*xyExtent).minX = (std::isnan((*xyExtent).minX) | (x[n] < (*xyExtent).minX)) ? x[n] : (*xyExtent).minX;
		(*xyExtent).maxX = (std::isnan((*xyExtent).maxX) | (x[n] > (*xyExtent).maxX)) ? x[n] : (*xyExtent).maxX;
		(*xyExtent).minY = (std::isnan((*xyExtent).minY) | (y[n] < (*xyExtent).minY)) ? y[n] : (*xyExtent).minY;
		(*xyExtent).maxY = (std::isnan((*xyExtent).maxY) | (y[n] > (*xyExtent).maxY)) ? y[n] : (*xyExtent).maxY;
	}

	return pts;
}

uint8_t * mcc(double *x, double *y, double *z, int n, double scaleDomain2Spacing, double curvatureThreshold) {

	XyExtent xyExtent;
	shared_ptr<PointVector> points = readPoints(x, y, z, n, xyExtent);

	SurfaceInterpolation surfaceInterpolation;
	surfaceInterpolation.setXyExtent(xyExtent);

	Algorithm algorithm(surfaceInterpolation, false, false);
	UnclassifiedPoints unclassifiedPoints(points);
	algorithm.classifyPoints(unclassifiedPoints, scaleDomain2Spacing, curvatureThreshold);

	uint8_t *classification = new uint8_t[n];
	IClassificationVector::index_type i;
	for (i.value = 0; i.value < n; i.value++) {
		if(mcc::isGround((*points)[i])) {
			classification[i] = mcc::PointClassification::Ground;
		} else {
			classification[i] = mcc::PointClassification::NonGround;
		}
	}

	return classification;

}

double * mcc_pass(double *x, double *y, double *z, int n, double scaleDomainSpacing) {

	XyExtent xyExtent;
	shared_ptr<PointVector> points = readPoints(x, y, z, n, xyExtent);

	SurfaceInterpolation surfaceInterpolation;
	surfaceInterpolation.setXyExtent(xyExtent);

	Algorithm algorithm(surfaceInterpolation, false, false);
	UnclassifiedPoints unclassifiedPoints(points);
	algorithm.labelPointsUsingPass(unclassifiedPoints, scaleDomainSpacing);

	double *h = new double[n];
	for (int i = 0; i < n; i++) {
		h[i] = (*points)[i].h()
	}

	return h;

}
