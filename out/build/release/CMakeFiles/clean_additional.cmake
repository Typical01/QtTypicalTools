# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "TypicalTools\\CMakeFiles\\TypicalTools_autogen.dir\\AutogenUsed.txt"
  "TypicalTools\\CMakeFiles\\TypicalTools_autogen.dir\\ParseCache.txt"
  "TypicalTools\\TypicalTools_autogen"
  )
endif()
