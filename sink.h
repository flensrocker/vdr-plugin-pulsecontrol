#ifndef _sink_h
#define _sink_h

#include "sinkport.h"

class cPulseSink : public cPulseObject {
private:
  cList<cPulseSinkPort> _ports;
  const cPulseSinkPort *_active_port;

public:
  cPulseSink(uint32_t index, const char *name)
   :cPulseObject(index, name)
   ,_active_port(NULL)
  {
  }

  cPulseSink(const cPulseSink &sink)
   :cPulseObject(sink)
   ,_active_port(NULL)
  {
    cListHelper<cPulseSinkPort>::Copy(sink.Ports(), _ports);
    if (sink.ActivePort())
       _active_port = cListHelper<cPulseSinkPort>::Find(_ports, sink.ActivePort()->Name());
  }

  virtual ~cPulseSink(void)
  {
  }

  virtual cString MenuItemText(void) const
  {
    return cString::sprintf("%s (%s)", *cPulseObject::MenuItemText(), PluggedText());
  }

  void AddPort(cPulseSinkPort *port, bool isactive)
  {
    if (isactive)
       _active_port = port;
    _ports.Add(port);
  }

  const cPulseSinkPort *ActivePort(void) const
  {
    return _active_port;
  }

  const cList<cPulseSinkPort> &Ports(void) const
  {
    return _ports;
  }

  const char *PluggedText() const
  {
    bool hasNo = false;
    bool hasOther = false;

    for (const cPulseSinkPort *port = Ports().First(); port; port = Ports().Next(port)) {
        if (port->Available() == PA_PORT_AVAILABLE_NO)
           hasNo = true;
        else {
           hasOther = true;
           break;
           }
        }

    if (hasNo && !hasOther)
       return "unplugged";
    return "plugged in";
  }
  };

#endif
