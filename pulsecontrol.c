/*
 * pulsecontrol.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <pulse/pulseaudio.h>

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "control settings of pulseaudio";
static const char *MAINMENUENTRY  = "Pulsecontrol";

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

class cPulseLoop : public cThread {
private:
  pa_mainloop *_mainloop;
  pa_context *_context;
  pa_mainloop_api *_mainloop_api;
  cMutex _mutex;
  int _action;

  static void context_drain_complete(pa_context *c, void *userdata)
  {
    isyslog("pulsecontrol: disconnecting");
    pa_context_disconnect(c);
  }

  void Drain(void)
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

  void Quit(int ret)
  {
    _mutex.Lock();
    if (_mainloop_api)
       _mainloop_api->quit(_mainloop_api, ret);
    _mutex.Unlock();
  }

  static void get_server_info_callback(pa_context *c, const pa_server_info *i, void *userdata)
  {
    if (!i) {
       esyslog("pulsecontrol: Failed to get server information: %s", pa_strerror(pa_context_errno(c)));
       return;
       }
    isyslog("pulsecontrol: Server String: %s, "
             "Library Protocol Version: %u, "
             "Server Protocol Version: %u",
             pa_context_get_server(c),
             pa_context_get_protocol_version(c),
             pa_context_get_server_protocol_version(c));
  }

  static void context_state_callback(pa_context *c, void *userdata)
  {
    if (!c)
       return;

    cPulseLoop *loop = (cPulseLoop*)userdata;
    pa_operation *o = NULL;

    switch (pa_context_get_state(c)) {
      case PA_CONTEXT_CONNECTING:
      case PA_CONTEXT_AUTHORIZING:
      case PA_CONTEXT_SETTING_NAME:
        break;

      case PA_CONTEXT_READY:
       {
         int action = loop->_action;
         loop->_action = 0;
         switch (action) {
           case 1:
            {
              o = pa_context_get_server_info(c, get_server_info_callback, userdata);
              break;
            }
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

    if (o)
       pa_operation_unref(o);
  }

protected:
  virtual void Action(void)
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

public:
  cPulseLoop()
  {
    _mainloop = NULL;
    _context = NULL;
    _mainloop_api = NULL;
    _action = 1;
  }

  virtual ~cPulseLoop()
  {
    Stop();
  }

  void Stop()
  {
    Drain();
    Quit(0);
    Cancel(3);
  }
  };

class cPluginPulsecontrol : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cPulseLoop *_loop;

public:
  cPluginPulsecontrol(void);
  virtual ~cPluginPulsecontrol();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginPulsecontrol::cPluginPulsecontrol(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  _loop = NULL;
}

cPluginPulsecontrol::~cPluginPulsecontrol()
{
  // Clean up after yourself!
}

const char *cPluginPulsecontrol::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginPulsecontrol::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginPulsecontrol::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginPulsecontrol::Start(void)
{
  // Start any background activities the plugin shall perform.
  _loop = new cPulseLoop();
  _loop->Start();
  return true;
}

void cPluginPulsecontrol::Stop(void)
{
  // Stop any background activities the plugin is performing.
  DELETENULL(_loop);
}

void cPluginPulsecontrol::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginPulsecontrol::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginPulsecontrol::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginPulsecontrol::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginPulsecontrol::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginPulsecontrol::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginPulsecontrol::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

bool cPluginPulsecontrol::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginPulsecontrol::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginPulsecontrol::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginPulsecontrol); // Don't touch this!
