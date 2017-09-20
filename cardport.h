#ifndef _card_port_h
#define _card_port_h

#include "profile.h"

class cPulseCardPort : public cPulseObject {
private:
  cString _description;
  uint32_t _priority;
  int _available;
  cList<cPulseProfile> _profiles;

public:
  cPulseCardPort(const char *name, const char *description, uint32_t priority, int available)
   :cPulseObject(0, name)
   ,_description(description)
   ,_priority(priority)
   ,_available(available)
  {
  }

  cPulseCardPort(const cPulseCardPort &port)
   :cPulseObject(port)
   ,_description(port._description)
   ,_priority(port._priority)
   ,_available(port._available)
  {
    cListHelper<cPulseProfile>::Copy(port.Profiles(), _profiles);
  }

  virtual ~cPulseCardPort(void)
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

  void AddProfile(const char *name, int available)
  {
    _profiles.Add(new cPulseProfile(name, available));
  }

  const cList<cPulseProfile> &Profiles(void) const
  {
    return _profiles;
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
