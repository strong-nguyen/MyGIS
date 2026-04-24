#pragma once

#include "Feature.h"

#include <string>
#include <list>

class GeoJSONReader
{
public:
  GeoJSONReader(const std::string& geoJsonPath);

  std::list<Feature> read();

private:
  std::string _geoJsonPath;
};