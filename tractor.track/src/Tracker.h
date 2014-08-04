#ifndef _TRACKER_H_
#define _TRACKER_H_

#include <RcppArmadillo.h>

#include "Space.h"
#include "NiftiImage.h"
#include "DiffusionDataSource.h"
#include "Streamline.h"
#include "Logger.h"

#define LOOPCHECK_RATIO 5.0

class Tracker
{
private:
    DiffusionDataSource *dataSource;
    NiftiImage<short> *mask;
    
    Array<Space<3>::Vector> *loopcheck;
    Array<bool> *visited;
    NiftiImage<int> *visitationMap;
    
    std::map<std::string,bool> flags;
    
    Space<3>::Point seed;
    Space<3>::Vector rightwardsVector;
    float innerProductThreshold;
    float stepLength;
    
    Logger logger;
    
public:
    Tracker () {}
    Tracker (DiffusionDataSource * const dataSource, NiftiImage<short> * const mask)
        : dataSource(dataSource), mask(mask), loopcheck(NULL), visited(NULL), visitationMap(NULL) {}
    
    ~Tracker ()
    {
        delete loopcheck;
        delete visited;
        delete visitationMap;
    }
    
    Space<3>::Point getSeed () const { return seed; }
    Space<3>::Vector getRightwardsVector () const { return rightwardsVector; }
    float getInnerProductThreshold () const { return innerProductThreshold; }
    float getStepLength () const { return stepLength; }
    
    void setSeed (const Space<3>::Point &seed) { this->seed = seed; }
    void setRightwardsVector (const Space<3>::Vector &rightwardsVector) { this->rightwardsVector = rightwardsVector; }
    void setInnerProductThreshold (const float innerProductThreshold) { this->innerProductThreshold = innerProductThreshold; }
    void setStepLength (const float stepLength) { this->stepLength = stepLength; }
    
    void setFlag (const std::string &key, const bool value = true) { this->flags[key] = value; }
    void setFlags (const std::map<std::string,bool> &flags) { this->flags = flags; }
    
    void setDebugLevel (const int &level) { this->logger.setOutputLevel(level); }
    
    Streamline run (const int maxSteps);
};

#endif
