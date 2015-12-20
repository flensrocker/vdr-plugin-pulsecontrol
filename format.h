#ifndef _format_h
#define _format_h

#include "object.h"
#include <pulse/pulseaudio.h>

class cPulseFormat : public cPulseObject {
private:
  pa_encoding_t _encoding;

public:
  cPulseFormat(pa_encoding_t encoding)
   :cPulseObject(0, pa_encoding_to_string(encoding))
   ,_encoding(encoding)
  {
  }

  cPulseFormat(const cPulseFormat &format)
   :cPulseObject(format)
   ,_encoding(format._encoding)
  {
  }

  virtual ~cPulseFormat(void)
  {
  }

  virtual cString MenuItemText(void) const
  {
    return cString::sprintf("%s", Name());
  }

  pa_encoding_t Encoding(void) const
  {
    return _encoding;
  }
  };

#endif
