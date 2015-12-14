#ifndef _tools_h
#define _tools_h

#include <vdr/thread.h>
#include <vdr/tools.h>
#include <pulse/pulseaudio.h>

class cPulseObject : public cListObject {
private:
  int _index;
  cString _name;

public:
  cPulseObject(int index, const char *name)
   :_index(index)
   ,_name(name)
  {
  }

  int Index() const { return _index; }
  const char *Name() const { return *_name; }
 
  virtual int Compare(const cListObject &ListObject) const
  {
    const cPulseObject &rhs = (const cPulseObject&)ListObject;
    return strcmp(Name(), rhs.Name());
  }
  };

class cPulseAction : public cListObject {
protected:
  bool _ready;

  void SetReady(bool ready)
  {
    _ready = ready;
  };

public:
  cPulseAction(void)
  {
    _ready = false;
  };

  bool IsReady(void) const
  {
    return _ready;
  };

  void Wait(void) const
  {
    while (!IsReady())
          cCondWait::SleepMs(10);
  };

  virtual pa_operation *Action(pa_context *context) = 0;
  };

#endif

