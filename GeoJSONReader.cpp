#include "GeoJSONReader.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>


using json = nlohmann::json;


GeoJSONReader::GeoJSONReader(const std::string& geoJsonPath)
  :
  _geoJsonPath(geoJsonPath)
{

}

std::list<Feature> GeoJSONReader::read()
{
  std::list<Feature> features;
  std::ifstream f(_geoJsonPath);
  json root = json::parse(f);

  for (const auto& feature : root["features"])
  {
    Feature parsedFeature;

    // Parse properties
    parsedFeature.Properties.Name = feature["properties"].value("name", "");
    parsedFeature.Properties.Id = feature.value("id", -1);

    // Parse geometry
    std::string type = feature["geometry"]["type"];
    if (type == "Polygon")
    {
      auto polygon = std::make_unique<PolygonXY>();
      for (const auto& point : feature["geometry"]["coordinates"][0])
      {
        polygon->Points.push_back({ point[0], point[1] });
      }
      parsedFeature.Geometry = std::move(polygon);
    }
    else if (type == "MultiPolygon")
    {
      auto mPolygon = std::make_unique<MultiPolygonXY>();
      for (const auto& polygon : feature["geometry"]["coordinates"])
      {
        PolygonXY poly;
        for (const auto& point : polygon[0])
        {
          poly.Points.push_back({point[0], point[1]});
        }
        mPolygon->Polygons.push_back(poly);
      }
      parsedFeature.Geometry = std::move(mPolygon);
    }

    features.push_back(std::move(parsedFeature));
  }

  return features;
}