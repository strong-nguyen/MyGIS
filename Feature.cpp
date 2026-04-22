#include "Feature.h"


Feature::Feature()
  :
  Geometry(nullptr)
{

}

Feature::Feature(Feature&& rhs) = default;

Feature::~Feature()
{

}