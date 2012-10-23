################################################################################
#
#  Program: OpenCV.cmake
#
#  Copyright (c) Atsushi Yamada PhD, 
#  Surgican Navigation and Robotics Lab (SNR Lab), Brigham and Women's Hospital and Harvard Medical School
#
#  This file was based on
#   SuperBuild.cmake 
#  on Slicer4/Slicer
#
################################################################################

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#-----------------------------------------------------------------------------
include(ExternalProject)
include(SlicerMacroCheckExternalProjectDependency)

# With CMake 2.8.9 or later, the UPDATE_COMMAND is required for updates to occur.
# For earlier versions, we nullify the update state to prevent updates and
# undesirable rebuild.
if(CMAKE_VERSION VERSION_LESS 2.8.9)
  set(slicer_external_update UPDATE_COMMAND "")
else()
  set(slicer_external_update)
endif()

# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

# Set CMake OSX variable to pass down the external project
set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
if(APPLE)
  list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
    -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
endif()

#------------------------------------------------------------------------------
# Configure and build OpenCV under NeuroendoscopeDemo-build folder
#------------------------------------------------------------------------------
set(proj NeuroendoscopeDemo)

ExternalProject_Add(${proj}
  #SVN_REPOSITORY "https://code.ros.org/svn/opencv/trunk"
  GIT_REPOSITORY "git://code.opencv.org/opencv.git"
  SOURCE_DIR OpenCV
  BINARY_DIR OpenCV-build
    CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DBUILD_TESTING:BOOL=OFF
    -DBUILD_SHARED_LIBS:BOOL=ON
  INSTALL_COMMAND ""
  )
  
# This custom external project step forces the build and later
# steps to run whenever a top level build is done...
ExternalProject_Add_Step(${proj} forcebuild
  COMMAND ${CMAKE_COMMAND} -E echo_append ""
  COMMENT "Forcing build step for '${proj}'"
  DEPENDEES configure
  DEPENDERS build
  ALWAYS 1
  )  