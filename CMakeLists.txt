cmake_minimum_required(VERSION 2.6)
project(volumereconstructor)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Xml)
find_package(VTK REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

include(${VTK_USE_FILE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(VolumeReconstructor_SRCS VolumeReconstructor.cpp main.cpp Parser.cpp imageprocessor.cpp)

add_executable(volumereconstructor ${VolumeReconstructor_SRCS})
target_link_libraries(volumereconstructor ${QT_QTCORE_LIBRARY})
if(VTK_LIBRARIES)
  target_link_libraries(volumereconstructor ${VTK_LIBRARIES})
else()
  target_link_libraries(volumereconstructor vtkHybrid vtkWidgets)
endif()

install(TARGETS volumereconstructor RUNTIME DESTINATION bin)
