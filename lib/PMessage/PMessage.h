#ifndef LIB_PMESSAGE
#define LIB_PMESSAGE

#include "PObject.h"

#ifdef PLATFORM_TYPE
  #include "platform1.pb.h"
#endif

class PMessage : public PObject
{
public:
    PMessage(const char* name);

private:
};

#endif // LIB_PUTILS
