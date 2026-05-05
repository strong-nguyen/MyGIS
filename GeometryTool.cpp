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

  bg::correct(poly);

  return bg::area(poly);
}

double MyGIS::calculateArea(const MultiPolygonXY& multiPolygon)
{
  auto multiPoly = toBoostMultiPolygon(multiPolygon);
  bg::correct(multiPoly);

  return bg::area(multiPoly);
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

MultiPolygonXY MyGIS::fromBoostMultiPolygon(const geo_multi_polygon& multiPolygon)
{
  MultiPolygonXY multiPolygonOutput;
  for (const auto& p : multiPolygon)
  {
    PolygonXY polygon;
    for (const auto& point : p.outer())
    {
      polygon.Points.push_back({ point.get<0>(), point.get<1>() });
    }
    multiPolygonOutput.Polygons.push_back(polygon);
  }

  return multiPolygonOutput;
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

PolygonXY MyGIS::mergePolygon(const PolygonXY& polygon1, const PolygonXY& polygon2)
{
  auto poly1 = toBoostPolygon(polygon1);
  auto poly2 = toBoostPolygon(polygon2);
  bg::correct(poly1);
  bg::correct(poly2);

  /*
  * Expand both polygons (Buffer) so they overlap.
  * Union them.
  * Shrink them back (Negative Buffer).
  */

  std::vector<geo_polygon> output;
  bg::union_(poly1, poly2, output);

  if (output.size() == 1)
  {
    geo_polygon& poly = output[0];
    PolygonXY polygon;
    for (const auto& point : poly.outer())
    {
      polygon.Points.push_back({point.get<0>(), point.get<1>()});
    }

    return polygon;
  }
  else
  {
    std::cerr << "Failed to merge polygon" << std::endl;
    return PolygonXY();
  }
}

MultiPolygonXY MyGIS::mergePolygon(const PolygonXY& polygon, const MultiPolygonXY& multiPolygon)
{
  auto poly = toBoostPolygon(polygon);
  auto multiPoly = toBoostMultiPolygon(multiPolygon);
  bg::correct(poly);
  bg::correct(multiPoly);

  geo_multi_polygon output;
  bg::union_(poly, multiPoly, output);
  
  return fromBoostMultiPolygon(output);
}

MultiPolygonXY MyGIS::mergePolygon(const MultiPolygonXY& multiPolygon1, const MultiPolygonXY& multiPolygon2)
{
  auto multiPoly1 = toBoostMultiPolygon(multiPolygon1);
  auto multiPoly2 = toBoostMultiPolygon(multiPolygon2);
  bg::correct(multiPoly1);
  bg::correct(multiPoly2);

  geo_multi_polygon output;
  bg::union_(multiPoly1, multiPoly2, output);

  return fromBoostMultiPolygon(output);
}
