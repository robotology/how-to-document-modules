// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_howToDocument_IDL
#define YARP_THRIFT_GENERATOR_howToDocument_IDL

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class howToDocument_IDL;


/**
 * howToDocument_IDL
 * IDL Interface to \ref how-to-document-modules services.
 * This is a fake thrift service, just to show how to structure your repo
 * and how to document your code.
 */
class howToDocument_IDL : public yarp::os::Wire {
public:
  howToDocument_IDL();
  /**
   * This is an example service, just printing a sentence.
   * Here you have to document how your service works and
   * if parameters are required, you have to describe them. Otherwise
   * you have just to say what your method returns.
   * @return true
   */
  virtual bool exampleService();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif
