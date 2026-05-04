#include "Feature.h"


Feature::Feature()
  :
  Geometry(nullptr)
{

}

Feature::Feature(Feature&& rhs) = default;

Feature::Feature(const Feature& rhs)
{
  Properties = rhs.Properties;
  if (rhs.Geometry)
  {
    if (rhs.Geometry->isPoint())
    {
      Geometry = std::make_unique<PointXY>(*static_cast<PointXY*>(rhs.Geometry.get()));
    }
    else if (rhs.Geometry->isPolygon())
    {
      Geometry = std::make_unique<PolygonXY>(*static_cast<PolygonXY*>(rhs.Geometry.get()));
    }
    else if (rhs.Geometry->isMultiPolygon())
    {
      Geometry = std::make_unique<MultiPolygonXY>(*static_cast<MultiPolygonXY*>(rhs.Geometry.get()));
    }
    else
    {
      std::cerr << "Not implemented\n";
    }
  }
}

Feature::~Feature()
{

}

PolygonXY::PolygonXY()
  :
  Geometry(GeometryType::Polygon)
{
}

PolygonXY::PolygonXY(const PolygonXY& rhs)
  :
  Geometry(GeometryType::Polygon),
  Points(rhs.Points),
  Centroid(rhs.Centroid)
{

}

MultiPolygonXY::MultiPolygonXY()
  :
  Geometry(GeometryType::MultiPolygon)
{
}

MultiPolygonXY::MultiPolygonXY(const MultiPolygonXY& rhs)
  :
  Geometry(GeometryType::MultiPolygon),
  Polygons(rhs.Polygons),
  Centroid(rhs.Centroid)
{
}

PointXY::PointXY(const PointXY& rhs)
  :
  Geometry(GeometryType::Point),
  Lon(rhs.Lon),
  Lat(rhs.Lat)
{
}
