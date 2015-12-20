#ifndef _device_h
#define _device_h

#include "format.h"

class cPulseDevice : public cPulseObject {
private:
  cString _description;
  cList<cPulseFormat> _formats;

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

  void AddFormat(cPulseFormat *format)
  {
    _formats.Add(format);
  }

  virtual cString MenuItemText(void) const
  {
    if (*_description && **_description)
       return cString::sprintf("%d - %s", Index(), Description());
    return cString::sprintf("%d - %s", Index(), Name());
  }

  void SetDescription(const char *description)
  {
    _description = description;
  }

  const char *Description(void) const
  {
    return *_description;
  }

  const cList<cPulseFormat> &Formats(void) const
  {
    return _formats;
  }
  };

#endif
