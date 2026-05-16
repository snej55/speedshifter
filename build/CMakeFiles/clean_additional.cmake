# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "src/CMakeFiles/main_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/main_autogen.dir/ParseCache.txt"
  "src/main_autogen"
  )
endif()
