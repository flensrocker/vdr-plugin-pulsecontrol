#ifndef _sink_h
#define _sink_h

#include "object.h"

class cPulseSink : public cPulseObject {
public:
  cPulseSink(uint32_t index, const char *name)
   :cPulseObject(index, name)
  {
  }

  virtual ~cPulseSink(void)
  {
  }
  };

#endif
