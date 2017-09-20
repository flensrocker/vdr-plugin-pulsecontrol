#ifndef _profile_h
#define _profile_h

#include "object.h"

class cPulseProfile : public cPulseObject {
private:
  int _available;

public:
  cPulseProfile(const char *name, int available)
   :cPulseObject(0, name)
   ,_available(available)
  {
  }

  cPulseProfile(const cPulseProfile &profile)
   :cPulseObject(profile)
   ,_available(profile._available)
  {
  }

  virtual ~cPulseProfile(void)
  {
  }

  virtual cString MenuItemText(void) const
  {
    return Name();
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
