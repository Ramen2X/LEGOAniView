# CMakeList.txt : CMake project for LEGOAniView, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("LEGOAniView")

# Add source to this project's executable.
add_executable (LEGOAniView "src/LEGOAniView.cpp" "src/LEGOAniView.h" "src/ani/ani.h" "src/main.cpp" "src/ani/actor.h" "src/ani/component.h" "src/ani/keyframe.h" "src/misc/color.h" "src/file/file.cpp" "src/file/file.h" "src/file/types.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LEGOAniView PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
