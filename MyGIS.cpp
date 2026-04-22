// MyGIS.cpp : Defines the entry point for the application.
//

#include "MyGIS.h"
#include "GeoJSONLReader.h"

#include <string>


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Please run: MyGIS.exe /path/to/GeoJSONL/file\n";
		return 1;
	}

	std::string geoJSONPath(argv[1]);
	GeoJSONLReader reader(geoJSONPath);
	std::list<Feature> features = reader.read();


	auto& first = features.front();
	PointXY* p1 = static_cast<PointXY*>(first.Geometry.get());
	std::cout << p1->Lon << ", " << p1->Lat << std::endl;


	return 0;
}
