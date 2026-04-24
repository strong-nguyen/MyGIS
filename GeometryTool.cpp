#include "GeometryTool.h"


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