#include "loop.h"
#include <vdr/tools.h>
#include <pulse/pulseaudio.h>

static void context_drain_complete(pa_context *c, void *userdata)
{
  isyslog("pulsecontrol: disconnecting");
  pa_context_disconnect(c);
}

static void context_state_callback(pa_context *c, void *userdata)
{
  if (!c)
     return;

  cPulseLoop *loop = (cPulseLoop*)userdata;

  switch (pa_context_get_state(c)) {
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
      break;

    case PA_CONTEXT_READY:
     {
       cPulseAction *a;
       while ((a = loop->NextAction()) != NULL) {
          pa_operation *o = a->Action(c);
          if (o)
             pa_operation_unref(o);
          }
       break;
     }

    case PA_CONTEXT_TERMINATED:
      isyslog("pulsecontrol: Connection terminated");
      loop->Quit(0);
      break;

    case PA_CONTEXT_FAILED:
    default:
      esyslog("pulsecontrol: Connection failure: %s", pa_strerror(pa_context_errno(c)));
      loop->Quit(1);
      break;
    }
}

void cPulseLoop::Drain(void)
{
  _mutex.Lock();
  if (_context) {
     pa_operation *o;
     if (!(o = pa_context_drain(_context, context_drain_complete, this)))
        pa_context_disconnect(_context);
     else
        pa_operation_unref(o);
     }
  _mutex.Unlock();
}

void cPulseLoop::Quit(int ret)
{
  _mutex.Lock();
  if (_mainloop_api)
     _mainloop_api->quit(_mainloop_api, ret);
  _mutex.Unlock();
}

void cPulseLoop::Action(void)
{
  _mutex.Lock();
  if (!(_mainloop = pa_mainloop_new())) {
      _mutex.Unlock();
      esyslog("pulsecontrol: pa_mainloop_new() failed");
      return;
     }
  _mainloop_api = pa_mainloop_get_api(_mainloop);
  if (!(_context = pa_context_new(_mainloop_api, NULL))) {
     _mutex.Unlock();
     esyslog("pulsecontrol: pa_context_new() failed");
     }
  else {
     pa_context_set_state_callback(_context, context_state_callback, this);
     if (pa_context_connect(_context, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        _mutex.Unlock();
        esyslog("pulsecontrol: pa_context_connect() failed: %s", pa_strerror(pa_context_errno(_context)));
        }
     else {
        _mutex.Unlock();
        int ret = 0;
        if (pa_mainloop_run(_mainloop, &ret) < 0) {
           esyslog("pulsecontrol: pa_mainloop_run() failed");
           }
        else {
           isyslog("pulsecontrol: mainloop quit with %d", ret);
           }
        }
     }

  _mutex.Lock();
  if (_context) {    
     pa_context_unref(_context);
     _context = NULL;
     }
  pa_mainloop_free(_mainloop);
  _mainloop = NULL;
  _mainloop_api = NULL;
  _mutex.Unlock();
}

cPulseLoop::cPulseLoop(void)
{
  _mainloop = NULL;
  _context = NULL;
  _mainloop_api = NULL;
}

cPulseLoop::~cPulseLoop(void)
{
  Stop();
}

void cPulseLoop::Stop(void)
{
  Drain();
  Quit(0);
  Cancel(3);
}

void cPulseLoop::AddAction(cPulseAction *action)
{
  if (!action)
     return;
  _actionMutex.Lock();
  _actions.Add(action);
  _mutex.Lock();
  context_state_callback(_context, this);
  _mutex.Unlock();
  _actionMutex.Unlock();
}

cPulseAction *cPulseLoop::NextAction(void)
{
  _actionMutex.Lock();
  cPulseAction *a = _actions.First();
  if (a)
     _actions.Del(a, false);
  _actionMutex.Unlock();
  return a;
}

