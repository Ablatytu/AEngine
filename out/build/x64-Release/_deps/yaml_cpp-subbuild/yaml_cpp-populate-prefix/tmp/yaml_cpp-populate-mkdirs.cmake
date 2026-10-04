# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-src")
  file(MAKE_DIRECTORY "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-src")
endif()
file(MAKE_DIRECTORY
  "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-build"
  "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix"
  "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/tmp"
  "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/src/yaml_cpp-populate-stamp"
  "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/src"
  "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/src/yaml_cpp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/src/yaml_cpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Stanislav/Desktop/AEngineGitHub/AEngine/out/build/x64-Release/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/src/yaml_cpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
