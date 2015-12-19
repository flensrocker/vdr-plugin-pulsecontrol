#ifndef _menu_h
#define _menu_h

#include <vdr/menuitems.h>

class cPulseObject;
class cPulseCard;

class cPulsecontrolMainMenu : public cOsdMenu
{
private:
  cPulseObject *_card;
  cPulseObject *_profile;
  cPulseObject *_sink;
  cPulseObject *_input;
  cList<cPulseCard> _cards;

public:
  cPulsecontrolMainMenu(void);
  virtual ~cPulsecontrolMainMenu(void);
  virtual eOSState ProcessKey(eKeys Key);
  
  eOSState SelectCard(void);
  eOSState SelectProfile(void);
  eOSState SelectSink(void);
  eOSState SelectSinkInput(void);
};

#endif

