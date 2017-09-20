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
  };

#endif
