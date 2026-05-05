#pragma once

#include "Feature.h"
#include "GeometryDefine.h"


namespace MyGIS
{
  double calculateDistance(const PointXY& p1, const PointXY& p2);

  double calculateArea(const PolygonXY& polygon);

  geo_polygon toBoostPolygon(const PolygonXY& polygon);

  geo_multi_polygon toBoostMultiPolygon(const MultiPolygonXY& multiPolygon);

  PointXY findCentroid(const PolygonXY& polygon);

  PointXY findCentroid(const MultiPolygonXY& multiPolygon);

  // Merge two nearby polygons to form a new one
  PolygonXY mergePolygon(const PolygonXY& polygon1, const PolygonXY& polygon2);
}