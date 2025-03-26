# MIT License
# 
# Copyright (c) 2022 Rishabh Dwivedi<rishabhdwivedi17@gmail.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(ParsePackagingArguments)
include(GenPkgConfig)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

project("expected" LANGUAGES CXX VERSION 0.1.0 DESCRIPTION "P0323 & P2505 `std::expected` simple implementation" HOMEPAGE_URL "https://github.com/RishabhRD/expected")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

set(PUBLIC_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/")
add_library("${PROJECT_NAME}" INTERFACE)
target_include_directories("${PROJECT_NAME}" "INTERFACE" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" "$<BUILD_INTERFACE:${PUBLIC_INCLUDES}>")

option(ENABLE_TESTING "Enable Test Builds" ON)
option(ENABLE_EXAMPLES "Enable Example Builds" OFF)
option(ENABLE_CONAN "Build Conan package" OFF)
option(ENABLE_PACKAGING "Build packages using CPack" ON)

if(ENABLE_CONAN)
  include(cmake/Conan.cmake)
  run_conan()
endif()

if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

if(ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if(ENABLE_PACKAGING)
  set(PROJECT_UNIQ_NAME "rd${PROJECT_NAME}")
  set(CPACK_PACKAGE_NAME "${PROJECT_UNIQ_NAME}")
  set(CPACK_PACKAGECPACK_PACKAGE_DESCRIPTION "${PROJECTCPACK_PACKAGE_DESCRIPTION}")
  set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_UNIQ_NAME}-dev")
  set(CPACK_RPM_PACKAGE_NAME "lib${PROJECT_UNIQ_NAME}-devel")
  set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
  set(CPACK_PACKAGE_MAINTAINER "Rishabh Dwivedi")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_MAINTAINER}")
  set(CPACK_DEB_COMPONENT_INSTALL ON)
  set(CPACK_RPM_COMPONENT_INSTALL ON)
  set(CPACK_NSIS_COMPONENT_INSTALL ON)
  set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
  set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")

  install(FILES "${PUBLIC_INCLUDES}/rd/expected.hpp"
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/rd"
  )

  install(TARGETS ${PROJECT_NAME}
      EXPORT ${PROJECT_NAME}
      INCLUDES
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )

  configure_pkg_config_file("${PROJECT_NAME}"
      NAME "${PROJECT_UNIQ_NAME}"
      VERSION "${PROJECT_VERSION}"
      DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}"
      URL "${PROJECT_HOMEPAGE_URL}"
      INSTALL_LIB_DIR "${CMAKE_INSTALL_DATAROOTDIR}"
      INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}"
  )

  set(CMAKE_CONFIG_VERSION_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_UNIQ_NAME}ConfigVersion.cmake")
  set(EXPORT_NAMESPACE "rd::")

  export(TARGETS ${PROJECT_NAME}
      NAMESPACE "${EXPORT_NAMESPACE}"
      FILE "${PROJECT_UNIQ_NAME}Export.cmake"
      EXPORT_LINK_INTERFACE_LIBRARIES
  )

  install(EXPORT ${PROJECT_NAME}
      FILE "${PROJECT_UNIQ_NAME}Config.cmake"
      NAMESPACE "${EXPORT_NAMESPACE}"
      DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_UNIQ_NAME}"
  )

  write_basic_package_version_file(
      "${CMAKE_CONFIG_VERSION_FILE_NAME}"
      VERSION "100500.100500.100500"  # any version of same bitness suits. CMake cannot compare to infinity, so use a large number we expect to be greater than any future version
      COMPATIBILITY AnyNewerVersion
      ARCH_INDEPENDENT
  )
  install(FILES "${CMAKE_CONFIG_VERSION_FILE_NAME}"
      DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_UNIQ_NAME}"
  )
  include(CPack)
endif()
