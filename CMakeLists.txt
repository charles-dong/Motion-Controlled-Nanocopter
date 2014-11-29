cmake_minimum_required(VERSION 2.6)
project(cflie)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories("${PROJECT_SOURCE_DIR}/include")

find_library(USB_LIB usb-1.0)
find_library(LEAP_LIBRARY_NAME libLeap.dylib)

add_library(${PROJECT_NAME}
  src/cflie/CCrazyRadio.cpp
  src/cflie/CCrazyflie.cpp
  src/cflie/CCRTPPacket.cpp
  src/cflie/CTOC.cpp
  src/leap/leap_controller.cpp
  src/leap/leap_frame.cpp
  src/leap/leap_gesture.cpp
  src/leap/leap_hand.cpp
  src/leap/leap_pointable.cpp
  src/leap/leap_utils.cpp
  src/control/jamieControl.cpp
  src/control/jamieControl.h
  src/control/charlieControl.cpp
  src/control/charlieControl.h

  )

add_executable(control src/control/control.cpp)

target_link_libraries(${PROJECT_NAME} ${USB_LIB})
target_link_libraries(control ${PROJECT_NAME} ${LEAP_LIBRARY_NAME})

install(FILES include/cflie/*.h DESTINATION include)
install(FILES lib/* DESTINATION lib)
