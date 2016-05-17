# Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Giulia Vezzani
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
#
# idl.thrift

/**
* howToDocument_IDL
*
* IDL Interface to \ref how-to-document-modules services.
* This is a fake thrift service, just to show how to structure your repo 
* and how to document your code.
*/

service howToDocument_IDL
{

    /**
    * This an example service, just printing a sentence.
    * Here you have to document the working of your service and
    * if parameters are required, you have to describe them. Otherwise 
    * you have just to say what your method returns.
    * @return true 
    */
    bool exampleService()
}
