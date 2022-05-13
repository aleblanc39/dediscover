#ifndef JNIOUTOFMEMORY_H
#define JNIOUTOFMEMORY_H

#include <string>

class JNIOutOfMemoryException {

 public:
    JNIOutOfMemoryException() {}
    std::string getMsg() {return "Ran out of memory when creating the java objects from JNI";}

};

#endif
