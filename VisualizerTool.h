#pragma once


#include "Feature.h"

#include <string>
#include <list>


namespace MyGIS
{
  void exportToSVG(const std::string& svgPath, const std::list<Geometry*> geometryList);
}