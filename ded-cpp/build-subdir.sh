#!/bin/bash

# Script to run cmake from in a xde subdirectory from the command line. It will create all the files outside 
# of the source directories.

# The input should only have the subdirectory name: base, model, etc



BUILD_DIRECTORY=$HOME/ded-cpp-build
SRC_DIR=xde/$1
TEST_DIR=testing/xde-tests/$1

SRC_BUILD_DIR=${BUILD_DIRECTORY}/${SRC_DIR}


cmake -D XDE_CPP_DIR=$HOME/projects/public-dediscover/dediscover/ded-cpp  -S $SRC_DIR  -B ${SRC_BUILD_DIR}
make -C ${SRC_BUILD_DIR}

echo Executing tests?  ${SRC_BUILD_DIR}

cmake -D XDE_CPP_DIR=$HOME/projects/public-dediscover/dediscover/ded-cpp  -D BUILD_DIR=${SRC_BUILD_DIR} -S $TEST_DIR -B $BUILD_DIRECTORY/$TEST_DIR
make -C $BUILD_DIRECTORY/$TEST_DIR test


