#!/bin/bash

# Script to run cmake from the command line. It will create all the files outside 
# of the source directories.

# Better to run it from this directory, with relative paths to the subdirectories.

# Edit for your system and preferences

# TODO Use location of thos script for the XDE_CPP_DIR property
# export XDE_CPP_DIR=$HOME/projects/public-dediscover/dediscover/ded-cpp

BUILD_DIRECTORY=$HOME/ded-cpp-build

#cmake -D XDE_CPP_DIR=$HOME/projects/public-dediscover/dediscover/ded-cpp  -S $1  -B $BUILD_DIRECTORY/$1
cmake -D XDE_CPP_DIR=$HOME/projects/dediscover/ded-cpp  -S $1  -B $BUILD_DIRECTORY/$1

make -C $BUILD_DIRECTORY/$1
