#pragma once

#include <string>
#include <memory>
#include <format>
#include <iostream>
#include <vector>
#include <optional>

struct Properties
{
  std::string Number;
  std::string Street;
  std::string City;

  std::string Hash;
  std::string Pid;

  std::string Name;
  int Id = -1;

};

enum class GeometryType
{
  Point,
  Polygon,
  MultiPolygon
};

struct Geometry
{
  GeometryType Type;

  Geometry(GeometryType _type) : Type(_type) {}

  bool isPoint() const
  {
    return Type == GeometryType::Point;
  }

  bool isPolygon() const
  {
    return Type == GeometryType::Polygon;
  }

  bool isMultiPolygon() const
  {
    return Type == GeometryType::MultiPolygon;
  }
};

struct Feature
{
  Properties Properties;
  std::unique_ptr<Geometry> Geometry;

  Feature();
  Feature(Feature&& rhs);
  Feature(const Feature& rhs);
  virtual ~Feature();
};


struct PointXY : public Geometry
{
  double Lon = 0.0;
  double Lat = 0.0;

  PointXY(double lon = 0.0, double lat = 0.0) : Geometry(GeometryType::Point), Lat(lat), Lon(lon) {}

  PointXY(const PointXY& rhs);

  friend std::ostream& operator<<(std::ostream& ss, const PointXY& point)
  {
    ss << std::format("Point({}, {})", point.Lon, point.Lat);
    return ss;
  }
};


struct PolygonXY : public Geometry
{
  std::vector<PointXY> Points;

  std::optional<PointXY> Centroid;

  PolygonXY();

  PolygonXY(const PolygonXY& rhs);
};


struct MultiPolygonXY : public Geometry
{
  std::vector<PolygonXY> Polygons;

  std::optional<PointXY> Centroid;

  MultiPolygonXY();

  MultiPolygonXY(const MultiPolygonXY& rhs);
};