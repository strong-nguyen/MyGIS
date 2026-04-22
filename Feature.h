#pragma once

#include <string>
#include <memory>

struct Properties
{
  std::string Number;
  std::string Street;
  std::string City;

  Properties(const std::string& number = "", const std::string& street = "", const std::string& city = "") : Number(number), Street(street), City(city) {}
};

enum class GeometryType
{
  Point
};

struct Geometry
{
  GeometryType Type;

  Geometry(GeometryType _type) : Type(_type) {}
};

struct Feature
{
  Properties Properties;
  std::unique_ptr<Geometry> Geometry;

  Feature();
  Feature(Feature&& rhs);
  virtual ~Feature();
};


struct PointXY : public Geometry
{
  double Lon = 0.0;
  double Lat = 0.0;

  PointXY(double lon, double lat) : Geometry(GeometryType::Point), Lat(lat), Lon(lon) {}
};
