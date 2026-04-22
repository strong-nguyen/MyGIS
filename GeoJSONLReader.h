#pragma once

#include "Feature.h"

#include <string>
#include <list>


class GeoJSONLReader
{
public:
  GeoJSONLReader(const std::string& geoJsonPath);

  std::list<Feature> read();

private:
  std::string _geoJsonPath;

};