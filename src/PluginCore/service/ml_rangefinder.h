#ifndef ML_RANGEFINDER_H_
#define ML_RANGEFINDER_H_

#include <exception>

class MLRangeFinderError : public std::exception
{
public:
    MLRangeFinderError() : std::exception("ML range finder error!") {}
    MLRangeFinderError(const char* msg) : std::exception(msg) {}
};

class MLRangeFinder
{
public:
    virtual bool Connect(const char* type_name) = 0;
    virtual bool Disconnect() = 0;
    virtual bool IsConnected() = 0;
    virtual double GetPosition(int index = 0) = 0;
};

#endif // !ML_RANGEFINDER_H_