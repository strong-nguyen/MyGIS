#include "VisualizerTool.h"
#include "GeometryDefine.h"
#include "GeometryTool.h"

#include <fstream>
#include <optional>

namespace MyGIS
{
  void drawText(std::ofstream& svg, const std::optional<PointXY>& pos, const std::string& text)
  {
    // Centroid name
    if (!std::empty(text) && pos)
    {
      std::string textElement = std::format("  <text x=\"{}\" y=\"{}\" text-anchor=\"middle\" dominant-baseline=\"middle\" font-size=\"0.1\">{}</text>\n", pos->Lon, -pos->Lat, text);
      svg << textElement;
    }
  }
  void drawPolygon(PolygonXY* polygon, const Properties properties, std::ofstream& svg)
  {
    svg << "  <path d=\"";
    for (int i = 0; i < polygon->Points.size(); ++i)
    {
      const auto& point = polygon->Points[i];
      svg << (i == 0 ? "M " : "L ") << point.Lon << "," << -point.Lat << " ";  // Because the Y coordinate in svg is from origin -> down
    }
    svg << "Z\"";

    svg << " fill=\"rgba(100, 150, 255, 0.5)\" stroke=\"blue\" stroke-width=\"" << 0.001 << "\" />\n";
  }
}


void MyGIS::exportToSVG(const std::string& svgPath, const std::list<Feature*>& geometryList)
{
  bg::model::box<geo_point> envelope;
  bg::assign_inverse(envelope);

  for (const auto& feature : geometryList)
  {
    Geometry* geo = feature->Geometry.get();
    if (geo->Type == GeometryType::Point)
    {
      PointXY* point = static_cast<PointXY*>(geo);
      bg::expand(envelope, geo_point(point->Lon, point->Lat));
    }
    else if (geo->Type == GeometryType::Polygon)
    {
      geo_polygon poly = toBoostPolygon(*(static_cast<PolygonXY*>(geo)));
      bg::expand(envelope, bg::return_envelope<bg::model::box<geo_point>>(poly));
    }
    else if (geo->Type == GeometryType::MultiPolygon)
    {
      geo_multi_polygon multi_poly = toBoostMultiPolygon(*(static_cast<MultiPolygonXY*>(geo)));
      bg::expand(envelope, bg::return_envelope<bg::model::box<geo_point>>(multi_poly));
    }
  }

  double minX = bg::get<bg::min_corner, 0>(envelope);
  double minY = bg::get<bg::min_corner, 1>(envelope);
  double width = bg::get<bg::max_corner, 0>(envelope) - minX;
  double height = bg::get<bg::max_corner, 1>(envelope) - minY;

  double paddingX = width * 0.05;
  double paddingY = height * 0.05;

  std::ofstream svg(svgPath);
  svg << R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>)" << "\n";
  svg << R"(<svg xmlns="http://www.w3.org/2000/svg" )";
  svg << std::format("viewBox=\"{} {} {} {}\" >", minX - paddingX, -(minY + height + paddingY), width + 2 * paddingX, height + 2 * paddingY) << std::endl;

  // Draw polygons
  for (const auto& feature : geometryList)
  {
    Geometry* geo = feature->Geometry.get();
    if (geo->isPolygon())
    {
      auto polygon = static_cast<PolygonXY*>(geo);
      drawPolygon(polygon, feature->Properties, svg);
      drawText(svg, polygon->Centroid, std::format("{}-{}", feature->Properties.Id, feature->Properties.Name));
    }
    else if (geo->isMultiPolygon())
    {
      auto multiPoly = static_cast<MultiPolygonXY*>(geo);
      for (auto& poly : multiPoly->Polygons)
      {
        drawPolygon(&poly, feature->Properties, svg);
      }
      drawText(svg, multiPoly->Centroid, std::format("{}-{}", feature->Properties.Id, feature->Properties.Name));
    }
  }
  svg << "</svg>";

  svg.close();
}