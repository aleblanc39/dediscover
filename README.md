# DEDiscover

DEDiscover is a very small scientific workbench based of the software on the same name that was developed at the 
University of Rochester between 2006 and 2014, and for which it seems that no trace remains. 
It is a GUI application that runs on the desktop. It is meant to allow non-mathematicians and non-programmers to easily create
and analyze models based on differential equations.

This project implements a subset of 
the functionality of the original software. It uses some of the original code but also went through a lot a rewrite and redesign.

It currently runs only on Linux, and for some very specific version of java8 and gcc. We have successfully ran it on recent 
versions of Centos and various flavors of Ubuntu.

This is an alpha version. It will work most of the time, but also crash more often than it should.

More details and documentation, as well as detailed installation and building information, can be found at the [dediscover website](https://dediscover.org).

# Quick start

Here are quick start instructions if the website is down.

## Install sdk

You need a very specific version of java, gcc and maybe of gradle as well. First install sdkman with

- curl -s "https://get.sdkman.io" | bash

Then you can install the correct versions of java and gradle:
- sdk install gradle 6.8.2
- sdk install java 8.0.282.fx-zulu

Note that this specific version of java may no longer be available. Use the latest of the form 8.0.xxx.fx-zulu.

# Install the CMake gradle plugin

We use CMake to build the C++ code, and a Gradle plugin is required. Download the code from the git repository with

- git clone git@github.com:aleblanc39/gradle-cmake-plugin.git

It can go anywhere in your account. 

You will need to install the [maven](https://maven.apache.org/) package. This procedure will depend on your system.

Then from the gradle-cmake-plugin directory you can install the plug-in using
- gradle publishToMavenLocal

## Packages to Install

The following packages must be present on your system:

- gcc-g++ version 8
- gcc-fortran
- cmake >= 3.12
- make
- boost (should work with most versions >= 1.55)
- blas
- lapack

You may need to use [update-alternatives](https://linux.die.net/man/8/update-alternatives) to ensure that the correct
versions of these packages are used during the build process.

# Compiling and Running

Going into the dediscover top directory and typing

- gradle runDED

should result in the code compiling and running. It may take a long time the first time it is executed. Don't 
hesitate to contact us if you have any question.

