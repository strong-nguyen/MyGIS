# CMakeList.txt : CMake project for MyGIS, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("MyGIS")

find_package(nlohmann_json CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (MyGIS "MyGIS.cpp" "MyGIS.h" "GeoJSONLReader.h" "GeoJSONLReader.cpp" "Feature.h" "Feature.cpp" "GeometryTool.h" "GeometryTool.cpp" "VisualizerTool.h" "VisualizerTool.cpp" "GeometryDefine.h" "GeoJSONReader.h" "GeoJSONReader.cpp")
target_link_libraries(MyGIS PRIVATE nlohmann_json::nlohmann_json)
target_compile_features(MyGIS PRIVATE cxx_std_20)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MyGIS PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
