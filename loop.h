#ifndef _loop_h
#define _loop_h

#include <vdr/thread.h>
#include <pulse/pulseaudio.h>

#include "tools.h"

class cPulseLoop : public cThread {
private:
  pa_mainloop *_mainloop;
  pa_context *_context;
  pa_mainloop_api *_mainloop_api;
  cMutex _mutex;

  cMutex _actionMutex;
  cList<cPulseAction> _actions;

  void Drain(void);

protected:
  virtual void Action(void);

public:
  cPulseLoop(void);
  virtual ~cPulseLoop(void);

  void Quit(int ret);
  void Stop(void);

  void AddAction(cPulseAction *action);
  cPulseAction *NextAction(void);
  };

#endif

