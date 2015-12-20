#ifndef _menu_h
#define _menu_h

#include <vdr/menuitems.h>

class cPulseCard;
class cPulseFormat;
class cPulseProfile;
class cPulseSink;
class cPulseSinkInput;

class cPulsecontrolMainMenu : public cOsdMenu
{
private:
  cList<cPulseCard> _cards;
  cPulseCard *_card;
  cList<cPulseFormat> _formats;
  cPulseProfile *_profile;
  cPulseSink *_sink;
  cPulseSinkInput *_input;

public:
  cPulsecontrolMainMenu(void);
  virtual ~cPulsecontrolMainMenu(void);
  virtual eOSState ProcessKey(eKeys Key);
  
  eOSState SelectCard(void);
  eOSState SelectDevice(void);
  eOSState SelectFormats(void);
  eOSState SelectProfile(void);
  eOSState SelectSink(void);
  eOSState SelectSinkInput(void);
};

#endif

