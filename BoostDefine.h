#pragma once


#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry.hpp>

#include <variant>

namespace bg = boost::geometry;
typedef bg::model::point<double, 2, bg::cs::geographic<bg::degree>> geo_point;
typedef bg::model::polygon<geo_point> geo_polygon;
typedef bg::model::multi_polygon<geo_polygon> geo_multi_polygon;
