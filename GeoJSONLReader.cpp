#include "GeoJSONLReader.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <format>

using json = nlohmann::json;

GeoJSONLReader::GeoJSONLReader(const std::string& geoJsonPath)
  :
  _geoJsonPath(geoJsonPath)
{

}

std::list<Feature> GeoJSONLReader::read()
{
  std::list<Feature> listFeatures;
  std::ifstream f(_geoJsonPath);
  if (!f)
  {
    std::cerr << std::format("Failed to open {}\n", _geoJsonPath);
    return {};
  }

  std::string line;
  int i = 0;
  while (std::getline(f, line))
  {
    json feature = json::parse(line);

    // Extract properties
    Feature featureData;
    if (feature.contains("properties"))
    {
      featureData.Properties.Number = feature["properties"].value("number", "");
      featureData.Properties.Street = feature["properties"].value("street", "");
      featureData.Properties.City = feature["properties"].value("city", "");

      featureData.Properties.Hash = feature["properties"].value("hash", "");
      featureData.Properties.Pid = feature["properties"].value("pid", "");
    }

    // Extract geometry
    if (feature.contains("geometry"))
    {
      std::string type = feature["geometry"]["type"];
      if (type == "Point")
      {
        double lon = feature["geometry"]["coordinates"][0];
        double lat = feature["geometry"]["coordinates"][1];
        featureData.Geometry = std::make_unique<PointXY>(lon, lat);
      }
      else if (type == "Polygon")
      {
        auto polygon = std::make_unique<PolygonXY>();
        json listPoints = feature["geometry"]["coordinates"][0];
        for (int i = 0; i < listPoints.size(); ++i)
        {
          polygon->Points.push_back({listPoints[i][0], listPoints[i][1]});
        }
        featureData.Geometry = std::move(polygon);
      }
    }

    listFeatures.push_back(std::move(featureData));
  }

  return listFeatures;
}