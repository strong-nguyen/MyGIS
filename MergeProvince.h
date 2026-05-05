#pragma once

#include "Feature.h"
#include "GeometryTool.h"

#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <set>

#include <nlohmann/json.hpp>


// Main method to merge Vietnam province
void mergeVietnamProvinces(const std::string& mergedProvinceFile, const std::list<Feature>& originFeatures, std::list<Feature>& mergedProvinceFeatures);
