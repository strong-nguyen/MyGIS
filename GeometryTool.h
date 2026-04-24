#pragma once

#include "Feature.h"
#include "GeometryDefine.h"


namespace MyGIS
{
  double calculateDistance(const PointXY& p1, const PointXY& p2);

  double calculateArea(const PolygonXY& polygon);

  geo_polygon toBoostPolygon(const PolygonXY& polygon);

  geo_multi_polygon toBoostMultiPolygon(const MultiPolygonXY& multiPolygon);
}