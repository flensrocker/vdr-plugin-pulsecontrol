#include "action.h"
#include "loop.h"

cPulseAction::cPulseAction(cPulseLoop &loop)
 :_loop(loop)
 ,_mainloop(NULL)
 ,_ready(false)
{
  loop.AddAction(this);
}

cPulseAction::~cPulseAction(void)
{
}

void cPulseAction::SignalReady()
{
  _ready = true;
  pa_threaded_mainloop_signal(_mainloop, 1);
}

void cPulseAction::Run(pa_threaded_mainloop *mainloop, pa_context *context)
{
  pa_threaded_mainloop_lock(mainloop);
  _mainloop = mainloop;
  _ready = false;
  pa_operation *o = Action(context);
  while (!_ready)
        pa_threaded_mainloop_wait(mainloop);
  pa_operation_unref(o);
  pa_threaded_mainloop_accept(mainloop);
  pa_threaded_mainloop_unlock(mainloop);
}
