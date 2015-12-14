#ifndef _action_getinfo_h
#define _action_getinfo_h

#include "tools.h"

class cPulseGetInfoAction : public cPulseAction {
private:
  cString _info;

  static void get_server_info_callback(pa_context *c, const pa_server_info *i, void *userdata)
  {
    cPulseGetInfoAction *action = (cPulseGetInfoAction*)userdata;
    if (!i) {
       esyslog("pulsecontrol: Failed to get server information: %s", pa_strerror(pa_context_errno(c)));
    } else {
       action->_info = cString::sprintf("Server String: %s\r\n"
               "Library Protocol Version: %u\r\n"
               "Server Protocol Version: %u",
               pa_context_get_server(c),
               pa_context_get_protocol_version(c),
               pa_context_get_server_protocol_version(c));
       }
    action->SetReady(true);
  }

public:
  cPulseGetInfoAction(void)
  {
    _info = "";
  };

  virtual pa_operation *Action(pa_context *context)
  {
    return pa_context_get_server_info(context, get_server_info_callback, this);
  };

  const char *Info(void) const
  {
    return *_info;
  }
  };

#endif

