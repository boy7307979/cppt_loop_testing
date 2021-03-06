cmake_minimum_required(VERSION 2.8.11)
project(cppt_modules)

# Cross Compile
if ("$ENV{CROSS_COMPILE_FLAG}" STREQUAL "XU4")
  message("Cross Compile For XU4")
  SET(CMAKE_SYSTEM_NAME Linux)
  SET(CMAKE_SYSTEM_PROCESSOR armv7l)
  SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
  SET(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)
  SET(CMAKE_BUILD_TYPE Release)
endif()

# If on OSX, force cmake to use gcc-6/5 instead of the built-in gcc (AppleClang)
# The compiler overriden has to happen before calling "project"
if (APPLE)
  if (EXISTS /usr/local/bin/g++-6)
    message("Override CXX compiler to /usr/local/bin/g++-6!")
    set(CMAKE_C_COMPILER   /usr/local/bin/gcc-6)
    set(CMAKE_CXX_COMPILER /usr/local/bin/g++-6)
  elseif (EXISTS /usr/local/bin/g++-5)
    message("Override CXX compiler to /usr/local/bin/g++-5!")
    set(CMAKE_C_COMPILER   /usr/local/bin/gcc-5)
    set(CMAKE_CXX_COMPILER /usr/local/bin/g++-5)
  elseif (EXISTS /usr/local/bin/g++-7)
    message("Override CXX compiler to /usr/local/bin/g++-7!")
    set(CMAKE_C_COMPILER   /usr/local/bin/gcc-7)
    set(CMAKE_CXX_COMPILER /usr/local/bin/g++-7)
  else ()
    message(FATAL_ERROR "Cannot find GNU gcc on Mac!")
  endif()
  set(CMAKE_MACOSX_RPATH 0)
endif()

# Configure cxx flags
if(CYGWIN)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -msse2 -Wno-unused-result")
else()
# NEON or SSE
if((CMAKE_SYSTEM_PROCESSOR MATCHES "^arm"))
  set(CMAKE_CXX_FLAGS "-D__ARM_NEON__ -DENABLE_NEON -mfloat-abi=hard -mfpu=neon -Ofast -std=c++11 -Wno-unused-result")
  message("Enabling neon for armv7 " ${CMAKE_CXX_FLAGS})
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
  set(CMAKE_CXX_FLAGS "-D__ARM_NEON__ -DENABLE_NEON -Ofast -std=c++11 -Wno-unused-result")
  message("Detected armv8 " ${CMAKE_CXX_FLAGS})
else()
 # SSE4.1 seems to be not supported by GCC 4.8.4
 # TODO(mingyu): See if we still need sse4.1
 message(STATUS "Enable SSE2 and SSE4.1")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -msse2 -msse4.1 -Wno-unused-result")
 # Export compile_commands.json
 set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()
endif()

# Suppress warnings for deprecated declarations for GCC 5 and above
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

# enable openmp flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

# Debug / Release / RelWithDebInfo
set(CMAKE_BUILD_TYPE Release)

# Thirdparty dependencies
# If CFG_VIEW is turned on, need to install glut (freeglut3-dev) and glew (libglew-dev)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/../cmake")

# add 3rd party
find_package(Eigen3 3.3.5 REQUIRED)
set(THRID_PARTY_INCLUDE_DIRS ${THRID_PARTY_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})
message("EIGEN3_INCLUDE_DIR:" ${EIGEN3_INCLUDE_DIR})
find_package(OpenCV 3.4.3 REQUIRED)
set(THRID_PARTY_INCLUDE_DIRS ${THRID_PARTY_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
set(THRID_PARTY_LIBS ${THRID_PARTY_LIBS} ${OpenCV_LIBS})
message("OpenCV_INCLUDE_DIRS:" ${OpenCV_INCLUDE_DIRS})
message("OpenCV_LIBS:" ${OpenCV_LIBS})
find_package(Ceres REQUIRED)
set(THRID_PARTY_LIBS ${THRID_PARTY_LIBS} ${CERES_LIBRARIES})
message("CERES_LIBRARIES:" ${CERES_LIBRARIES})
find_package(GTest REQUIRED)
set(THRID_PARTY_INCLUDE_DIRS ${THRID_PARTY_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
set(THRID_PARTY_LIBS ${THRID_PARTY_LIBS} ${GTEST_LIBRARIES})
message("GTEST_INCLUDE_DIRS:" ${GTEST_INCLUDE_DIRS})
message("GTEST_LIBRARIES:" ${GTEST_LIBRARIES})
#find_package(ArrayFire REQUIRED)
#message("ArrayFire::afcuda:" ArrayFire::afcuda)
#set(THRID_PARTY_LIBS ${THRID_PARTY_LIBS} ArrayFire::afcuda)
#find_package(GTSAM REQUIRED)
#set(THRID_PARTY_INCLUDE_DIRS ${THRID_PARTY_INCLUDE_DIRS} ${GTSAM_INCLUDE_DIR})
#set(THRID_PARTY_LIBS ${THRID_PARTY_LIBS} gtsam)

# add ros
find_package(catkin REQUIRED COMPONENTS
roscpp
roslib
cmake_modules
systrace
)

set(THRID_PARTY_INCLUDE_DIRS ${THRID_PARTY_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})
set(THRID_PARTY_LIBS ${THRID_PARTY_LIBS} ${catkin_LIBRARIES})

catkin_package(
DEPENDS
CATKIN_DEPENDS roscpp roslib
INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
LIBRARIES cppt_modules
)

# DBoW2
set(DBOW2_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/reloc/3rdParty/DBoW2)

# include
include_directories(
${THRID_PARTY_INCLUDE_DIRS}
${PROJECT_SOURCE_DIR}
${DBOW2_INCLUDE_DIRS}
)

add_library(cppt_modules SHARED
basic_datatype/so3_extent.cpp
basic_datatype/frame.cpp
basic_datatype/sliding_window.cpp
basic_datatype/mappoint.cpp
basic_datatype/util_datatype.cpp
basic_datatype/keyframe.cpp
camera_model/camera.cpp
camera_model/pinhole_camera.cpp
camera_model/stereo_camera.cpp
camera_model/simple_stereo_camera.cpp
# front_end/image_processor.cpp
front_end/frontend_config.cpp
front_end/utility.cpp
front_end/simple_frontend.cpp
front_end/sparse_img_align.cpp
# back_end/msckf.cpp
back_end/imu_state.cpp
back_end/imu_preintegration.cpp
# back_end/isam2_backend.cpp
back_end/simple_backend.cpp
pose_graph/simple_pose_graph.cpp
pose_graph/utility.cpp
ceres/projection_factor.cpp
ceres/marginalization_factor.cpp
system/vo_system.cpp
${DBOW2_INCLUDE_DIRS}/DBoW2/BowVector.cpp
${DBOW2_INCLUDE_DIRS}/DBoW2/FBrief.cpp
${DBOW2_INCLUDE_DIRS}/DBoW2/FORB.cpp
${DBOW2_INCLUDE_DIRS}/DBoW2/FeatureVector.cpp
${DBOW2_INCLUDE_DIRS}/DBoW2/QueryResults.cpp
${DBOW2_INCLUDE_DIRS}/DBoW2/ScoringObject.cpp
${DBOW2_INCLUDE_DIRS}/DUtils/Random.cpp
${DBOW2_INCLUDE_DIRS}/DUtils/Timestamp.cpp
${DBOW2_INCLUDE_DIRS}/DVision/BRIEF.cpp
#${DBOW2_INCLUDE_DIRS}/VocabularyBinary.cpp
)
target_link_libraries(cppt_modules ${THRID_PARTY_LIBS})

add_executable(cppt_test
test/cppt_test.cpp
test/openmp_test.cpp
#test/arrayfire_test.cpp
#test/gtsam_test.cpp
camera_model/pinhole_camera_test.cpp
camera_model/stereo_camera_test.cpp
)
target_link_libraries(cppt_test cppt_modules)
