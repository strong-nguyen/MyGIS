#include "Feature.h"


Feature::Feature()
  :
  Geometry(nullptr)
{

}

Feature::Feature(Feature&& rhs) = default;

Feature::~Feature()
{

}

PolygonXY::PolygonXY()
  :
  Geometry(GeometryType::Polygon)
{
}

MultiPolygonXY::MultiPolygonXY()
  :
  Geometry(GeometryType::MultiPolygon)
{
}
