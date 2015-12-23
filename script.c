#include "script.h"

#include <vdr/config.h>

#include "action_listsinkinputs.h"
#include "action_movesinkinput.h"
#include "action_setcardprofile.h"
#include "action_setdefaultsink.h"

cPulseScriptLine::cPulseScriptLine(void)
{
}
  
cPulseScriptLine::~cPulseScriptLine(void)
{
}
  
bool cPulseScriptLine::Parse(const char *s)
{
  _line = s;
  return true;
}
  
cPulseAction *cPulseScriptLine::ToAction(cPulseLoop &loop) const
{
  if (!*_line || !**_line)
     return NULL;

  if (startswith(*_line, "set-card-profile ")) {
     const char *option = skipspace(*_line + 17);
     cString card;
     const char *profile = NULL;
     if (!option || !*option) {
        esyslog("pulsecontrol: missing name of card and profile");
        return NULL;
        }
     profile = option;
     while ((*profile != 0) && (*profile != ' '))
           profile++;
     card = cString(option, profile);
     if (*profile != 0)
        profile++;
     if (*card == NULL) {
        esyslog("pulsecontrol: missing name of card");
        return NULL;
        }
     if (*profile == 0) {
        esyslog("pulsecontrol: missing name of profile");
        return NULL;
        }
     return new cPulseSetCardProfileAction(loop, *card, profile);
     }

  if (startswith(*_line, "move-sink-input ")) {
     const char *option = skipspace(*_line + 16);
     cString input;
     uint32_t index = PA_INVALID_INDEX;
     const char *sink = NULL;
     if (!option || !*option) {
        esyslog("pulsecontrol: missing name of input and sink");
        return NULL;
        }
     sink = option + strlen(option) - 1;
     while ((sink > option) && (*sink != ' '))
           sink--;
     if (sink == option) {
        cPulseLoop l;
        cPulseListSinkInputsAction inputs(l);
        int ret = l.Run();
        if (ret != 0) {
           esyslog("pulsecontrol: error %d", ret);
           return NULL;
           }
        if (inputs.SinkInputs().Count() != 1) {
           esyslog("pulsecontrol: missing name of input");
           return NULL;
           }
        index = inputs.SinkInputs().First()->Index();
        input = inputs.SinkInputs().First()->Name();
        }
     else {
        input = cString(option, sink);
        if (*input == NULL) {
           esyslog("pulsecontrol: missing name of input");
           return NULL;
           }
        sink++;
        if (*sink == 0) {
           esyslog("pulsecontrol: missing name of sink");
           return NULL;
           }
        if (isnumber(*input))
           index = strtol(*input, NULL, 10);
        else {
           cPulseLoop l;
           cPulseListSinkInputsAction inputs(l);
           int ret = l.Run();
           if (ret != 0) {
              esyslog("pulsecontrol: error %d", ret);
              return NULL;
              }
           const cPulseSinkInput *i = cListHelper<cPulseSinkInput>::Find(inputs.SinkInputs(), *input);
           if (!i) {
              esyslog("pulsecontrol: unknown sink input %s", *input);
              return NULL;
              }
           index = i->Index();
           }
        }
     return new cPulseMoveSinkInputAction(loop, index, sink);
     }
  return NULL;
}


cPulseScript::cPulseScript(void)
{
}

cPulseScript::~cPulseScript(void)
{
}

cPulseScript *cPulseScript::FromFile(const char *filename)
{
  cConfig<cPulseScriptLine> lines;
  if (!lines.Load(filename, true, true)) {
     esyslog("pulsecontrol: error on reading script %s", filename);
     return NULL;
     }
  cPulseScript *script = new cPulseScript();
  for (const cPulseScriptLine *s = lines.First(); s; s = lines.Next(s)) {
      cPulseAction *a = s->ToAction(script->_loop);
      if (a)
         script->_actions.Add(a);
      }
  return script;
}

cPulseScript *cPulseScript::FromLine(const char *line)
{
  cPulseScriptLine l;
  if (!l.Parse(line)) {
     esyslog("pulsecontrol: error on reading line %s", line);
     return NULL;
     }
  cPulseScript *script = new cPulseScript();
  cPulseAction *a = l.ToAction(script->_loop);
  if (a)
     script->_actions.Add(a);
  return script;
}

int cPulseScript::Run(void)
{
  return _loop.Run();
}
