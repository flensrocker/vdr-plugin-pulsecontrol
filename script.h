#ifndef _script_h
#define _script_h

#include "loop.h"

class cPulseScriptLine : public cListObject {
private:
  cString _line;

public:
  cPulseScriptLine(void);
  virtual ~cPulseScriptLine(void);
  
  bool Parse(const char *s);
  
  cPulseAction *ToAction(cPulseLoop &loop) const;
  };

class cPulseScript {
private:
  cPulseLoop _loop;
  cList<cPulseAction> _actions;

  cPulseScript(void);

public:
  static cPulseScript *FromFile(const char *filename);
  static cPulseScript *FromLine(const char *line);
  
  virtual ~cPulseScript(void);
  
  int Run(void);
  };

#endif
