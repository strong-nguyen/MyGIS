// MyGIS.cpp : Defines the entry point for the application.
//

#include "MyGIS.h"
#include "GeoJSONLReader.h"
#include "GeoJSONReader.h"
#include "GeometryTool.h"
#include "VisualizerTool.h"

#include <string>


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Please run: MyGIS.exe /path/to/GeoJSONL-file\n";
		return 1;
	}

	std::string geoJSONPath(argv[1]);
	//GeoJSONLReader reader(geoJSONPath);
	GeoJSONReader reader(geoJSONPath);
	std::list<Feature> features = reader.read();


	PointXY* firstPoint = nullptr;
	PointXY* secondPoint = nullptr;
	PolygonXY* firstPoly = nullptr;
	PolygonXY* secondPoly = nullptr;

	//int i = 0;
	//for (auto it = features.begin(); it != features.end(); ++it)
	//{
	//	if (it->Geometry->Type == GeometryType::Point)
	//	{
	//		if (i == 0)
	//		{
	//			firstPoint = static_cast<PointXY*>(it->Geometry.get());
	//		}
	//		else if (i == 1)
	//		{
	//			secondPoint = static_cast<PointXY*>(it->Geometry.get());
	//		}
	//		
	//		++i;

	//		if (i == 2)
	//		{
	//			break;
	//		}
	//	}
	//}

	//i = 0;
	//for (auto it = features.begin(); it != features.end(); ++it)
	//{
	//	if (it->Geometry->Type == GeometryType::Polygon)
	//	{
	//		if (i == 0)
	//		{
	//			firstPoly = static_cast<PolygonXY*>(it->Geometry.get());
	//		}
	//		else if (i == 1)
	//		{
	//			secondPoly = static_cast<PolygonXY*>(it->Geometry.get());
	//		}

	//		++i;
	//		if (i == 2)
	//		{
	//			break;
	//		}
	//	}
	//}

	//if (firstPoint && secondPoint)
	//{
	//	std::cout << "First point: " << *firstPoint << std::endl;
	//	std::cout << "Second point: " << *secondPoint << std::endl;

	//	std::cout << std::format("Distance: {}\n", MyGIS::calculateDistance(*firstPoint, *secondPoint));
	//}

	//if (firstPoly)
	//{
	//	std::cout << std::format("Polygon area: {} m2\n", MyGIS::calculateArea(*firstPoly));
	//}

	std::list<Geometry*> l;
	for (const auto& f : features)
	{
		if (!f.Geometry)
		{
			continue;
		}
		l.push_back(f.Geometry.get());
	}

	MyGIS::exportToSVG("polygons.svg", l);

	return 0;
}
