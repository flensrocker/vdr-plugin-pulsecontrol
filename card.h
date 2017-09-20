#ifndef _card_h
#define _card_h

#include "cardport.h"
#include "profile.h"

class cPulseCard : public cPulseObject {
private:
  cString _alsa_name;
  cString _description;
  cList<cPulseCardPort> _ports;
  cList<cPulseProfile> _profiles;
  const cPulseProfile *_active_profile;
  
public:
  cPulseCard(uint32_t index, const char *name, const char *alsa_name, const char *description)
   :cPulseObject(index, name)
   ,_alsa_name(alsa_name)
   ,_description(description)
   ,_active_profile(NULL)
  {
  }

  cPulseCard(const cPulseCard &card)
   :cPulseObject(card)
   ,_alsa_name(card._alsa_name)
   ,_description(card._description)
   ,_active_profile(NULL)
  {
    cListHelper<cPulseCardPort>::Copy(card.Ports(), _ports);
    cListHelper<cPulseProfile>::Copy(card.Profiles(), _profiles);
    if (card.ActiveProfile())
       _active_profile = cListHelper<cPulseProfile>::Find(_profiles, card.ActiveProfile()->Name());
  }

  virtual ~cPulseCard(void)
  {
  }

  virtual cString MenuItemText(void) const
  {
    cString pluggedText = "";
    if (ActiveProfile() != NULL)
       pluggedText = cString::sprintf(" (%s)", PluggedText(ActiveProfile()));
    if (*_description && **_description)
       return cString::sprintf("%d - %s%s", Index(), Description(), *pluggedText);
    if (*_alsa_name && **_alsa_name)
       return cString::sprintf("%d - %s%s", Index(), AlsaCardName(), *pluggedText);
    return cString::sprintf("%d - %s%s", Index(), Name(), *pluggedText);
  }

  void AddPort(cPulseCardPort *port)
  {
    _ports.Add(port);
  }

  void AddProfile(const char *name, int available, bool isactive)
  {
    cPulseProfile *p = new cPulseProfile(name, available);
    if (isactive)
       _active_profile = p; 
    _profiles.Add(p);
  }

  const char *AlsaCardName(void) const
  {
    return *_alsa_name;
  }
  
  const char *Description(void) const
  {
    return *_description;
  }

  const cList<cPulseCardPort> &Ports(void) const
  {
    return _ports;
  }
  
  const cPulseProfile *ActiveProfile(void) const
  {
    return _active_profile;
  }

  const cList<cPulseProfile> &Profiles(void) const
  {
    return _profiles;
  }

  const char *PluggedText(const cPulseProfile *profile) const
  {
    bool hasNo = false;
    bool hasOther = false;

    for (const cPulseCardPort *port = Ports().First(); port; port = Ports().Next(port)) {
        const cPulseProfile *pr = cListHelper<cPulseProfile>::Find(port->Profiles(), profile->Name());
        if (pr == NULL)
           continue;

        if (port->Available() == PA_PORT_AVAILABLE_NO)
           hasNo = true;
        else {
           hasOther = true;
           break;
           }
        }

    if (hasNo && !hasOther)
       return "unplugged";
    if (!profile->Available())
       return "unavailable";
    return "plugged in";
  }
  };

#endif
