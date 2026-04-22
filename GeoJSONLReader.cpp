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
    featureData.Properties.Number = feature["properties"]["number"].get<std::string>();
    featureData.Properties.Street = feature["properties"]["street"].get<std::string>();
    featureData.Properties.City = feature["properties"]["city"].get<std::string>();

    //std::cout << std::format("type: {}, number: {}, street: {}\n", feature["type"].get<std::string>(), feature["properties"]["number"].get<std::string>(), feature["properties"]["street"].get<std::string>());



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
    }

    listFeatures.push_back(std::move(featureData));
  }

  return listFeatures;
}