# dediscover

This is a re-release of the DEDiscover software that was developed at the University of Rochester between 2006 and 2014. 


Prepare:

curl -s "https://get.sdkman.io" | bash

sdk install gradle 6.8.2

sdk install java 8.0.282.fx-zulu

git clone git@github.com:aleblanc39/gradle-cmake-plugin.git

From the gradle-cmake-plugin directory:
- gradle publishToMavenLocal


## Packages to Install

- gcc-g++
- gcc-cfortran
- cmake
- make


## On Centos8
- dnf install boost
- dnf install blas
- dnf install lapack

Will install locally,

Need to compile gradle-cmake plugin since the version in the repo has been compiled with java 11

To Build:

gradle ded-cpp:cmakeBuild (Will take a long time.)


## Setting up the development environment

There are many options.

# Ideaj

(tornadofx, kotlin, gradle)

# vscode

Add xde-cpp folder to workspace. Add C++, CMake extensions.
(cmake options for XDE_CPP_DIR)


# Running

Need to set DED_DATABASE_FILE environment variable. For now.
