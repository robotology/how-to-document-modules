/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Giulia Vezzani
 * email:  giulia.vezzani@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#include <string>
#include <iomanip>

#include <yarp/os/all.h>

#include "src/howToDocument_IDL.h"

using namespace std;
using namespace yarp::os;

class FakeModule : public RFModule,
                   public howToDocument_IDL
{
protected:
    BufferedPort<Bottle> portOut;
    BufferedPort<Bottle>  portIn;
    /************************************************************************/
    bool attach(RpcServer &source)
    {
        return this->yarp().attachAsServer(source);
    }

    /************************************************************************/
    bool exampleService()
    {
        yInfo()<<"This is an example thrift service";
        return true;
    }

public:

    /***********************************************************************/
    double getPeriod()
    {
        return 1.0;
    }

    /***********************************************************************/
    bool updateModule()
    {
        yInfo()<<"update Module is running...";

        return true;
    }

    /***********************************************************************/
    bool configure(ResourceFinder &rf)
    {
        portOut.open("how-to-document-modules/data:o");
        portIn.open("how-to-document-modules/data:i");
        return true;
    }

    /***********************************************************************/
    bool close()
    {
        if (!portOut.isClosed())
            portOut.close();

        if (!portIn.isClosed())
            portIn.close();

        return true;
    }
};

    /**********************************************************************/
    int main(int argc,char *argv[])
    {
        Network yarp;
        if (!yarp.checkNetwork())
        {
            yError("unable to find YARP server!");
            return 1;
        }

        FakeModule mod;
        ResourceFinder rf;
        rf.setDefaultContext("superquadric-detection");
        rf.configure(argc,argv);
        return mod.runModule(rf);
    }

