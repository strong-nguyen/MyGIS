#include "GeometryTool.h"


#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;
typedef bg::model::point<double, 2, bg::cs::geographic<bg::degree>> geo_point;

double MyGIS::calculateDistance(const PointXY& p1, const PointXY& p2)
{
  geo_point geoP1(p1.Lon, p1.Lat);
  geo_point geoP2(p2.Lon, p2.Lat);

  return bg::distance(geoP1, geoP2);
}
