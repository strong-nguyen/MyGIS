// MyGIS.cpp : Defines the entry point for the application.
//

#include "MyGIS.h"
#include "GeoJSONLReader.h"
#include "GeoJSONReader.h"
#include "GeometryTool.h"
#include "VisualizerTool.h"
#include "MergeProvince.h"

#include <string>
#include <filesystem>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif


namespace fs = std::filesystem;

template <typename T>
void updateCentroid(Geometry* geo)
{
	T* shape = static_cast<T*>(geo);
	PointXY centroid = MyGIS::findCentroid(*shape);
	shape->Centroid = centroid;
}

bool parseArgs(std::optional<int>& idToCalculateArea, std::optional<std::string>& mergedProvinceJson, int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Please run: MyGIS.exe /path/to/GeoJSONL-file\n";
		return false;
	}

	for (int i = 0; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (arg == "--cal-area" && i + 1 < argc)
		{
			idToCalculateArea = std::stoi(argv[i + 1]);
		}
		else if (arg == "--merged-provinces" && i + 1 < argc)
		{
			mergedProvinceJson = argv[i + 1];
		}
	}

	if (idToCalculateArea)
	{
		std::cout << "Id to calculate area: " << *idToCalculateArea << std::endl;
	}

	if (mergedProvinceJson)
	{
		std::cout << "Merged province json path: " << *mergedProvinceJson << std::endl;
	}

	return true;
}


int main(int argc, char** argv)
{
#ifdef _WIN32
	// Set console output to UTF-8 (65001)
	SetConsoleOutputCP(65001);
#endif

	std::optional<int> idToCalculateArea;
	std::optional<std::string> mergedProvinceJson;
	if (!parseArgs(idToCalculateArea, mergedProvinceJson, argc, argv))
	{
		return 1;
	}

	std::string geoJSONPath(argv[1]);
	fs::path jsonFile(geoJSONPath);
	std::list<Feature> features;
	if (fs::path ext = jsonFile.extension(); ext == ".geojson")
	{
		GeoJSONLReader reader(geoJSONPath);
		features = reader.read();
	}
	else if (ext == ".json")
	{
		GeoJSONReader reader(geoJSONPath);
		features = reader.read();
	}

	// Calculate area
	if (idToCalculateArea)
	{
		int id = *idToCalculateArea;
		auto it = std::find_if(features.begin(), features.end(), [id](const Feature& feature)
			{
				return feature.Properties.Id == id;
			});
		if (it != features.end())
		{
			double areaKm2 = MyGIS::calculateArea(*(static_cast<PolygonXY*>(it->Geometry.get()))) / std::pow(10, 6);
			std::string msg = std::format("Area of {} before merge: {:.2f} km2", it->Properties.Name, areaKm2);
			std::cout << msg << std::endl;
		}
	}

	for (auto& f : features)
	{
		if (f.Geometry)
		{
			if (f.Geometry->isPolygon())
			{
				updateCentroid<PolygonXY>(f.Geometry.get());
			}
			else if (f.Geometry->isMultiPolygon())
			{
				updateCentroid<MultiPolygonXY>(f.Geometry.get());
			}
		} 
	}


	std::list<Feature*> l;
	for (auto& f : features)
	{
		if (!f.Geometry)
		{
			continue;
		}

		l.push_back(&f);
	}

	MyGIS::exportToSVG("polygons.svg", l);


	// Merge province
	if (mergedProvinceJson)
	{
		std::list<Feature> mergedProvinceFeatures;
		mergeVietnamProvinces(*mergedProvinceJson, features, mergedProvinceFeatures);

		std::list<Feature*> l;
		for (auto& f : mergedProvinceFeatures)
		{
			if (!f.Geometry)
			{
				continue;
			}

			l.push_back(&f);
		}
		MyGIS::exportToSVG("merged-polygons.svg", l);
	}

	return 0;
}
