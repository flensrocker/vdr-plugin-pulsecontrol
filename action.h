#ifndef _action_h
#define _action_h

#include <vdr/tools.h>
#include <pulse/pulseaudio.h>

class cPulseLoop;

class cPulseAction : public cListObject {
private:
  friend class cPulseLoop;

  cPulseLoop &_loop;
  pa_threaded_mainloop *_mainloop;
  bool _ready;

  void Run(pa_threaded_mainloop *mainloop, pa_context *context);

protected:
  void SignalReady();
  virtual pa_operation *Action(pa_context *context) = 0;

public:
  cPulseAction(cPulseLoop &loop);
  virtual ~cPulseAction(void);
  };

#endif

