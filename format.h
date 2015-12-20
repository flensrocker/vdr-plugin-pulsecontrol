#ifndef _format_h
#define _format_h

#include "object.h"
#include <pulse/pulseaudio.h>

class cPulseFormat : public cPulseObject {
public:
  cPulseFormat(pa_encoding_t encoding)
   :cPulseObject(encoding, pa_encoding_to_string(encoding))
  {
  }

  cPulseFormat(const cPulseFormat &format)
   :cPulseObject(format)
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
    return (pa_encoding_t)Index();
  }

  pa_format_info *ToFormatInfo(void) const
  {
    pa_format_info *f = pa_format_info_new();
    f->encoding = Encoding();
    return f;
  }
  };

#endif
