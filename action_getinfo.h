#ifndef _action_getinfo_h
#define _action_getinfo_h

#include "action.h"

class cPulseGetInfoAction : public cPulseAction {
private:
  cString _server;
  uint32_t _protocol_version;
  uint32_t _server_protocol_version;

  static void get_server_info_callback(pa_context *c, const pa_server_info *i, void *userdata)
  {
    cPulseGetInfoAction *action = (cPulseGetInfoAction*)userdata;
    if (!i) {
       esyslog("pulsecontrol: Failed to get server information: %s", pa_strerror(pa_context_errno(c)));
    } else {
       action->_server = pa_context_get_server(c); 
       action->_protocol_version = pa_context_get_protocol_version(c);
       action->_server_protocol_version = pa_context_get_server_protocol_version(c); 
       }
    action->SignalReady();
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    return pa_context_get_server_info(context, get_server_info_callback, this);
  }

public:
  cPulseGetInfoAction(cPulseLoop &loop)
   :cPulseAction(loop, "GetInfo")
   ,_server("")
   ,_protocol_version(0)
   ,_server_protocol_version(0)
  {
  }

  virtual ~cPulseGetInfoAction(void)
  {
  }

  const char *Server(void) const
  {
    return *_server;
  }

  uint32_t ProtocolVersion(void) const
  {
    return _protocol_version;
  }

  uint32_t ServerProtocolVersion(void) const
  {
    return _server_protocol_version;
  }

  cString Info(void) const
  {
    return cString::sprintf("Server String: %s\r\n"
            "Library Protocol Version: %u\r\n"
            "Server Protocol Version: %u",
            *_server,
            _protocol_version,
            _server_protocol_version);
  }
  };

#endif
