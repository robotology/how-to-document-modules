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
   * This an example service that just prints a sentence.
   * Put here meaningful information about input/output parameters.
   * @return true
   */
  virtual bool exampleService();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif
