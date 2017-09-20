#ifndef _sink_port_h
#define _sink_port_h

#include "object.h"

class cPulseSinkPort : public cPulseObject {
private:
  cString _description;
  uint32_t _priority;
  int _available;

public:
  cPulseSinkPort(const char *name, const char *description, uint32_t priority, int available)
   :cPulseObject(0, name)
   ,_description(description)
   ,_priority(priority)
   ,_available(available)
  {
  }

  cPulseSinkPort(const cPulseSinkPort &port)
   :cPulseObject(port)
   ,_description(port._description)
   ,_priority(port._priority)
   ,_available(port._available)
  {
  }

  virtual ~cPulseSinkPort(void)
  {
  }

  const char *Description(void) const
  {
    return *_description;
  }

  uint32_t Priority(void) const
  {
    return _priority;
  }

  int Available(void) const
  {
    return _available;
  }

  const char *PluggedText() const
  {
    if (_available == PA_PORT_AVAILABLE_YES)
       return "plugged in";
    if (_available == PA_PORT_AVAILABLE_NO)
       return "unavailable";
    return "unplugged";
  }
  };

#endif
