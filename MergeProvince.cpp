#include "MergeProvince.h"


std::map<std::u8string, std::set<std::u8string>> readJsonData(const std::string& mergedProvinceFile)
{
  std::ifstream f(mergedProvinceFile);
  if (!f)
  {
    std::cerr << "Not found " << mergedProvinceFile << std::endl;
    return {};
  }

  std::map<std::u8string, std::set<std::u8string>> mergedProvinces;

  nlohmann::json mergedPronvinceJson = nlohmann::json::parse(f);
  for (auto& [key, value] : mergedPronvinceJson.items())
  {
    std::u8string provinceName(key.begin(), key.end());

    std::set<std::u8string> set;
    for (auto& mergedProvince : value)
    {
      std::string val = mergedProvince;
      std::u8string valU8(val.begin(), val.end());
      set.insert(valU8);
    }

    mergedProvinces.insert({ provinceName , set });
  }

  return mergedProvinces;
}

// There are provinces which is kept, not merged with other provinces
void addNotMergedProvinces(const std::set<std::u8string>& mergedProvinces, const std::list<Feature>& originFeatures, std::list<Feature>& mergedProvinceFeatures)
{
  for (const auto& feature : originFeatures)
  {
    std::u8string provinceName(feature.Properties.Name.begin(), feature.Properties.Name.end());
    if (mergedProvinces.count(provinceName))
    {
      continue;
    }

    Feature copyFeature = feature;
    mergedProvinceFeatures.push_back(copyFeature);
  }
}

void mergeVietnamProvinces(const std::string& mergedProvinceFile, const std::list<Feature>& originFeatures, std::list<Feature>& mergedProvinceFeatures)
{
  // Read json data
  std::map<std::u8string, std::set<std::u8string>> mergedProvinces = readJsonData(mergedProvinceFile);

  std::set<std::u8string> provinces;
  for (const auto& [_, value] : mergedProvinces)
  {
    provinces.insert(value.begin(), value.end());
  }

  addNotMergedProvinces(provinces, originFeatures, mergedProvinceFeatures);

  // Merge provinces
  for (const auto& [key, value] : mergedProvinces)
  {
    if (value.empty())
    {
      // Skip
      continue;
    }

    auto keyIt = std::find_if(originFeatures.begin(), originFeatures.end(), [&key](const Feature& feature)
      {
        std::u8string provinceName(feature.Properties.Name.begin(), feature.Properties.Name.end());
        return provinceName == key;
      });

    std::optional<PolygonXY> mergedPolygon;
    std::optional<MultiPolygonXY> mergedMultiPolygon;
    for (auto it = value.begin(); it != value.end(); ++it)
    {
      auto featureIt = std::find_if(originFeatures.begin(), originFeatures.end(), [it](const Feature& feature)
        {
          std::u8string provinceName(feature.Properties.Name.begin(), feature.Properties.Name.end());
          return provinceName == *it;
        });
      if (featureIt != originFeatures.end())
      {
        if (featureIt->Geometry->isPolygon())
        {
          PolygonXY* polygon = static_cast<PolygonXY*>(featureIt->Geometry.get());

          if (mergedMultiPolygon)
          {
            mergedMultiPolygon = MyGIS::mergePolygon(*polygon, *mergedMultiPolygon);
          }
          else if (!mergedPolygon)
          {
            // Initialize with the first polygon
            mergedPolygon = *polygon;
          }
          else
          {
            // Merge polygon
            mergedPolygon = MyGIS::mergePolygon(*mergedPolygon, *polygon);
          }
        }
        else if (featureIt->Geometry->isMultiPolygon())
        {
          MultiPolygonXY* multiPolygon = static_cast<MultiPolygonXY*>(featureIt->Geometry.get());
          if (mergedMultiPolygon)
          {
            // Merge multi-polygon vs multi-polygon
            mergedMultiPolygon = MyGIS::mergePolygon(*mergedMultiPolygon, *multiPolygon);
          }
          else if (mergedPolygon)
          {
            // Merge polygon vs multi-polygon
            mergedMultiPolygon = MyGIS::mergePolygon(*mergedPolygon, *multiPolygon);
          }
          else
          {
            // Initialize with the first multi-polygon
            mergedMultiPolygon = *multiPolygon;
          }
        }
      }
      else
      {
        std::cerr << "Cannot found feature of province " << std::string(it->begin(), it->end()) << std::endl;
      }
    }

    if (mergedMultiPolygon)
    {
      // We got a new multi-polygon
      Feature feature;
      feature.Properties.Id = keyIt != originFeatures.end() ? keyIt->Properties.Id : -1;
      feature.Properties.Name = std::string(key.begin(), key.end());
      mergedMultiPolygon->Centroid = MyGIS::findCentroid(*mergedMultiPolygon);
      feature.Geometry = std::make_unique<MultiPolygonXY>(*mergedMultiPolygon);
      mergedProvinceFeatures.push_back(feature);
    }
    else if (mergedPolygon)
    {
      // We got a new polygon
      Feature feature;
      feature.Properties.Id = keyIt != originFeatures.end() ? keyIt->Properties.Id : -1;
      feature.Properties.Name = std::string(key.begin(), key.end());
      mergedPolygon->Centroid = MyGIS::findCentroid(*mergedPolygon);
      feature.Geometry = std::make_unique<PolygonXY>(*mergedPolygon);
      mergedProvinceFeatures.push_back(feature);
    }
  }
}