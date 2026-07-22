# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/radarScan_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/radarScan_autogen.dir/ParseCache.txt"
  "radarScan_autogen"
  )
endif()
