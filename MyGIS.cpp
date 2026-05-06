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
#include <list>
#include <tuple>

#ifdef _WIN32
#include <windows.h>
#endif


namespace fs = std::filesystem;


bool parseArgs(std::optional<int>& idToCalculateArea, std::optional<std::string>& mergedProvinceJson, int argc, char** argv);
void visualizeOnSVG(std::list<Feature>& features, const char* svgFile);
std::tuple<std::string, double> calculateAreaFromId(int id, const std::list<Feature>& features);

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
		auto [name, areaKm2] = calculateAreaFromId(id, features);
		if (!name.empty() && areaKm2 > 0)
		{
			std::cout << std::format("Area of {} before merge: {:.2f} Km2", name, areaKm2) << std::endl;
		}
		else
		{
			std::cout << std::format("Not found id {}", id) << std::endl;
		}
	}

	// Update centroid position
	for (auto& f : features)
	{
		if (f.Geometry)
		{
			if (f.Geometry->isPolygon())
			{
				MyGIS::updateCentroid<PolygonXY>(f.Geometry.get());
			}
			else if (f.Geometry->isMultiPolygon())
			{
				MyGIS::updateCentroid<MultiPolygonXY>(f.Geometry.get());
			}
		} 
	}

	// Visualize on svg file
	visualizeOnSVG(features, "polygons.svg");


	// Merge province
	if (mergedProvinceJson)
	{
		std::list<Feature> mergedProvinceFeatures;
		mergeVietnamProvinces(*mergedProvinceJson, features, mergedProvinceFeatures);

		visualizeOnSVG(mergedProvinceFeatures, "merged-polygons.svg");

		if (idToCalculateArea)
		{
			int id = *idToCalculateArea;
			auto [name, areaKm2] = calculateAreaFromId(id, mergedProvinceFeatures);
			if (!name.empty() && areaKm2 > 0)
			{
				std::cout << std::format("Area of {} after merge: {:.2f} Km2", name, areaKm2) << std::endl;
			}
			else
			{
				std::cout << std::format("Not found id {}", id) << std::endl;
			}
		}
	}

	return 0;
}

bool parseArgs(std::optional<int>& idToCalculateArea, std::optional<std::string>& mergedProvinceJson, int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Please run: MyGIS.exe /path/to/GeoJSONL-file [--cal-area id] [--merged-provinces]\n";
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

void visualizeOnSVG(std::list<Feature>& features, const char* svgFile)
{
	std::list<Feature*> l;
	for (auto& f : features)
	{
		if (!f.Geometry)
		{
			continue;
		}

		l.push_back(&f);
	}
	MyGIS::exportToSVG(svgFile, l);
}

// Return in Km2
std::tuple<std::string, double> calculateAreaFromId(int id, const std::list<Feature>& features)
{

	auto it = std::find_if(features.begin(), features.end(), [id](const Feature& feature)
		{
			return feature.Properties.Id == id;
		});
	if (it != features.end())
	{
		double areaKm2 = 0.0;
		if (it->Geometry->isPolygon())
		{
			areaKm2 = MyGIS::calculateArea(*(static_cast<PolygonXY*>(it->Geometry.get()))) / std::pow(10, 6);
		}
		else if (it->Geometry->isMultiPolygon())
		{
			areaKm2 = MyGIS::calculateArea(*(static_cast<MultiPolygonXY*>(it->Geometry.get()))) / std::pow(10, 6);
		}
		
		return {it->Properties.Name, areaKm2 };
	}

	return {"", -1.0};
}
