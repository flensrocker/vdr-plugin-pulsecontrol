#ifndef _action_listdevices_h
#define _action_listdevices_h

#include "action.h"
#include "device.h"

#include <pulse/ext-device-manager.h>

class cPulseListDevicesAction : public cPulseAction {
private:
  cList<cPulseDevice> _devices;

  static void ext_device_manager_read_callback(pa_context *c, const pa_ext_device_manager_info* info, int eol, void *userdata)
  {
    cPulseListDevicesAction *action = (cPulseListDevicesAction*)userdata;
    if (eol) {
       if (eol < 0)
	  esyslog("pulsecontrol: failed to initialize device manager extension: %s", pa_strerror(pa_context_errno(c)));
       action->SignalReady();
       }
    else  {
       dsyslog("pulsecontrol: found device %d: %s (%s)", info->index, info->name, info->description);
       cPulseDevice *device = new cPulseDevice(info->index, info->name, info->description);
       action->_devices.Add(device);
       }
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    return pa_ext_device_manager_read(context, ext_device_manager_read_callback, this);
  }

public:
  cPulseListDevicesAction(cPulseLoop &loop)
   :cPulseAction(loop, "ListDevices")
  {
  }

  virtual ~cPulseListDevicesAction(void)
  {
  }

  const cList<cPulseDevice> &Devices(void) const
  {
    return _devices;
  }
  
  cList<cPulseDevice> &Devices(void)
  {
    return _devices;
  }
  
  cString Info(void) const
  {
    if (Devices().Count() == 0)
       return "no devices found";
    cString ret = "";
    for (const cPulseDevice *d = Devices().First(); d; d = Devices().Next(d)) {
        dsyslog("pulsecontrol: device %d: %s", d->Index(), d->Name());
        ret = cString::sprintf("%sdevice %d: %s\r\n", *ret, d->Index(), d->Name());
	if (d->Description()) {
           dsyslog("pulsecontrol: desc: %s", d->Description());
           ret = cString::sprintf("%s desc: %s\r\n", *ret, d->Description());
	   }
        }
    return ret;
  }
  };

#endif
