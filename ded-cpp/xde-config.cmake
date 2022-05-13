# Configuration file for cmake to be used by all the libraries to ensure
# that they are all compiled with the same options.


# Beacuse of the many versions and distributions of linux you may have to
# fiddle with the following commands involving boost. 
# Your version of CMake may also cause problems.

#set (BOOST_LIBRARYDIR $ENV{BOOST_ROOT})
#set  (BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu/")

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".so.0" ".so.1" ".so.2" ".so.3" ".so.4" ".so.5" ".so.1.62.0")

#set  (BOOST_LIBRARYDIR "/usr/lib64")
set (BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu/")
set (BOOST_INCLUDEDIR "/usr/include")


find_package(Boost  1.62.0  REQUIRED COMPONENTS filesystem regex system date_time thread)
#find_package(Boost  1.65.1 REQUIRED COMPONENTS filesystem regex system date_time)
# find_package(Boost  REQUIRED  filesystem regex system date_time)
if (NOT Boost_FOUND) 
   message("Could not find the boost libraries. Leaving...")	
   return()
endif (NOT Boost_FOUND) 

set (BOOST_LIBRARIES ${Boost_LIBRARIES}) # Just so I don't rewrite all makefiles
set (LIBRARY_TYPE "SHARED")
  
set (EXTRA_OPTIONS "-g  -Wno-deprecated -fPIC -std=c++17 -DLINUX -DBOOST_PHOENIX_NO_VARIADIC_EXPRESSION")


# TODO Move all the include, lib from thirdparty packages into 
# ${THIRDPARTY}/include. Should be feasible with the install
# configurations of respective packages.
include_directories(
  ${Boost_INCLUDE_DIRS}
)


# Look at environment for library paths. If both DYLD_LIBRARY_PATH and
# LD_LBRARY_PATH are use only the former.
if( NOT $ENV{DYLD_LIBRARY_PATH}  STREQUAL "")
  string (REPLACE ":" " " DYLD_LIBRARIES $ENV{DYLD_LIBRARY_PATH})
  string (REPLACE "//" "/" DYLD_LIBRARIES ${DYLD_LIBRARIES})
  string(REGEX MATCHALL "[^ ]+" LIST_DYLD_LIBRARIES ${DYLD_LIBRARIES})
else (NOT $ENV{DYLD_LIBRARY_PATH}  STREQUAL "")
  if (NOT $ENV{LD_LIBRARY_PATH}  STREQUAL "")
    string (REPLACE ":" ";" LIBRARIES $ENV{LD_LIBRARY_PATH})
    string (REPLACE "//" "/" LIBRARIES ${LIBRARIES})
  endif (NOT $ENV{LD_LIBRARY_PATH}  STREQUAL "")
endif(NOT $ENV{DYLD_LIBRARY_PATH}  STREQUAL "")

set (LIBRARIES_SEARCH_PATH ${LIBRARIES} /lib /usr/lib64 /usr/lib /usr/local/lib)
find_library(CMATH NAMES m PATHS  /usr/lib64)
