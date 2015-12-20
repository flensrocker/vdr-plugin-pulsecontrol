#ifndef _device_h
#define _device_h

#include "object.h"

class cPulseDevice : public cPulseObject {
private:
  cString _description;

public:
  cPulseDevice(uint32_t index, const char *name, const char *description)
   :cPulseObject(index, name)
   ,_description(description)
  {
  }

  cPulseDevice(const cPulseDevice &device)
   :cPulseObject(device)
   ,_description(device._description)
  {
  }

  virtual ~cPulseDevice(void)
  {
  }

  virtual cString MenuItemText(void) const
  {
    if (*_description && **_description)
       return cString::sprintf("%d - %s", Index(), Description());
    return cString::sprintf("%d - %s", Index(), Name());
  }

  const char *Description(void) const
  {
    return *_description;
  }
  };

#endif
