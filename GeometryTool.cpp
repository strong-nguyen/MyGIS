#include "GeometryTool.h"

#include <boost/geometry/strategies/strategies.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/strategies/centroid/geographic.hpp>

double MyGIS::calculateDistance(const PointXY& p1, const PointXY& p2)
{
  geo_point geoP1(p1.Lon, p1.Lat);
  geo_point geoP2(p2.Lon, p2.Lat);

  return bg::distance(geoP1, geoP2);
}

double MyGIS::calculateArea(const PolygonXY& polygon)
{
  geo_polygon poly;
  for (const auto& p : polygon.Points)
  {
    bg::append(poly.outer(), geo_point(p.Lon, p.Lat));
  }

  return bg::area(poly);
}

geo_polygon MyGIS::toBoostPolygon(const PolygonXY& polygon)
{
  geo_polygon poly;
  for (const auto& p : polygon.Points)
  {
    bg::append(poly.outer(), geo_point(p.Lon, p.Lat));
  }

  return poly;
}

geo_multi_polygon MyGIS::toBoostMultiPolygon(const MultiPolygonXY& multiPolygon)
{
  geo_multi_polygon multi_poly;
  for (const auto& poly : multiPolygon.Polygons)
  {
    geo_polygon boost_poly = toBoostPolygon(poly);
    multi_poly.push_back(boost_poly);
  }

  return multi_poly;
}

PointXY MyGIS::findCentroid(const PolygonXY& polygon)
{
  // I don't know why I cannot apply bg::centroid for the polygon in degree format
  // For simplicity, just use average method

  PointXY centroid(0.0, 0.0);
  for (const auto& point : polygon.Points)
  {
    centroid.Lon += point.Lon;
    centroid.Lat += point.Lat;
  }

  centroid.Lon /= polygon.Points.size();
  centroid.Lat /= polygon.Points.size();

  return centroid;
}

PointXY MyGIS::findCentroid(const MultiPolygonXY& multiPolygon)
{
  if (multiPolygon.Polygons.empty())
  {
    return {};
  }

  return findCentroid(multiPolygon.Polygons[0]);
}

geo_polygon MyGIS::mergePolygon(const geo_polygon& polygon1, const geo_polygon& polygon2)
{
  bg::correct(polygon1);
  bg::correct(polygon2);

  /*
  * Expand both polygons (Buffer) so they overlap.
  * Union them.
  * Shrink them back (Negative Buffer).
  */


  return geo_polygon();
}
