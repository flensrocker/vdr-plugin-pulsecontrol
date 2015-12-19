#include "menu.h"

#include "action_listcards.h"
#include "action_listsinks.h"
#include "action_listsinkinputs.h"
#include "action_movesinkinput.h"
#include "action_setcardprofile.h"
#include "action_setdefaultsink.h"
#include "loop.h"


enum eMenuAction { maMoveSinkInput,
                   maSetCardProfile,
                   maSetDefaultSink
                 };

class cPulsecontrolMenuObjectlist : public cOsdMenu {
private:
  cList<cPulseObject> *_list;
  cPulseObject **_dest;

public:
  cPulsecontrolMenuObjectlist(const char *title, cList<cPulseObject> *list, cPulseObject **dest)
   :cOsdMenu(title)
   ,_list(list)
   ,_dest(dest)
  {
    for (const cPulseObject *obj = _list->First(); obj; obj = _list->Next(obj)) {
        if (obj) {
           cString text = cString::sprintf("%d - %s", obj->Index(), obj->Name());
           Add(new cOsdItem(*text));
           }
        }
  }

  virtual ~cPulsecontrolMenuObjectlist(void)
  {
    delete _list;
  }

  void SelectedObject() const
  {
    DELETENULL(*_dest);
    int i = Current();
    if ((i < 0) || (i >= _list->Count()))
       return;
    const cPulseObject *obj = _list->Get(i);
    if (obj)
       *_dest = new cPulseObject(obj->Index(), obj->Name());
  }
};

class cPulsecontrolMenuItem : public cOsdItem
{
public:
  const eMenuAction Action;

  cPulsecontrolMenuItem(eMenuAction action, const char *Text)
   :cOsdItem(Text)
   ,Action(action)
  {
  }
};


cPulsecontrolMainMenu::cPulsecontrolMainMenu(void)
 :cOsdMenu("pulsecontrol")
 ,_card(NULL),_profile(NULL)
 ,_sink(NULL),_input(NULL)
{
  Add(new cPulsecontrolMenuItem(maMoveSinkInput, tr("move sink input")));
  Add(new cPulsecontrolMenuItem(maSetCardProfile, tr("set card profile")));
  Add(new cPulsecontrolMenuItem(maSetDefaultSink, tr("set default sink")));
}

cPulsecontrolMainMenu::~cPulsecontrolMainMenu(void)
{
  delete _card;
  delete _profile;
  delete _sink;
  delete _input;
}

eOSState cPulsecontrolMainMenu::ProcessKey(eKeys Key)
{
  cPulsecontrolMenuItem *item = dynamic_cast<cPulsecontrolMenuItem*>(Get(Current()));
  if (item == NULL)
     return cOsdMenu::ProcessKey(Key);
  eOSState state = osUnknown;
  cPulseLoop loop;
  switch (Key) {
    case kOk:
     {
       if (HasSubMenu()) {
          cPulsecontrolMenuObjectlist *ol = dynamic_cast<cPulsecontrolMenuObjectlist*>(SubMenu());
          if (ol)
             ol->SelectedObject();
          CloseSubMenu();
          }
          
       switch (item->Action) {
         case maMoveSinkInput:
           {
             if (!_input)
                state = SelectSinkInput();
             else if (!_sink)
                state = SelectSink();
             else {
               cPulseMoveSinkInputAction action(loop, _input->Index(), _sink->Index());
               int ret = loop.Run();
               if (ret != 0) {
                   }
               DELETENULL(_sink);
               DELETENULL(_input);
               }
             break;
           }
         case maSetCardProfile:
           {
             if (!_card)
                state = SelectCard();
             else if (!_profile)
                state = SelectProfile();
             else {
               cPulseSetCardProfileAction action(loop, _card->Index(), _profile->Name());
               int ret = loop.Run();
               if (ret != 0) {
                   }
               DELETENULL(_card);
               DELETENULL(_profile);
               }
             break;
           }
         case maSetDefaultSink:
           {
             if (!_sink)
                state = SelectSink();
             else {
               cPulseSetDefaultSinkAction action(loop, _sink->Name());
               int ret = loop.Run();
               if (ret != 0) {
                   }
               DELETENULL(_sink);
               }
             break;
           }
          }
       break;
     }
    default:
     {
      state = cOsdMenu::ProcessKey(Key);
      break;
     }
    }
  return state;
}

eOSState cPulsecontrolMainMenu::SelectCard(void)
{
  eOSState state = osUnknown;
  cPulseLoop loop;
  cPulseListCardsAction cards(loop);
  int ret = loop.Run();
  if (ret != 0) {
     }
  else {
     cListHelper<cPulseCard>::Move(cards.Cards(), _cards);
     cList<cPulseObject> *objs = new cList<cPulseObject>();
     cListHelper<cPulseCard>::Convert(_cards, *objs);
     state = AddSubMenu(new cPulsecontrolMenuObjectlist(tr("select card"), objs, &_card));
     }
  return state;
}

eOSState cPulsecontrolMainMenu::SelectProfile(void)
{
  eOSState state = osUnknown;
  if (_card) {
     const cPulseCard *card = cListHelper<cPulseCard>::Find(_cards, _card->Index());
     if (card) {
        cList<cPulseObject> *objs = new cList<cPulseObject>();
        cListHelper<cPulseProfile>::Convert(card->Profiles(), *objs);
        state = AddSubMenu(new cPulsecontrolMenuObjectlist(tr("select profile"), objs, &_profile));
        }
     }
  return state;
}

eOSState cPulsecontrolMainMenu::SelectSink(void)
{
  eOSState state = osUnknown;
  cPulseLoop loop;
  cPulseListSinksAction sinks(loop);
  int ret = loop.Run();
  if (ret != 0) {
     }
  else {
     cList<cPulseObject> *objs = new cList<cPulseObject>();
     cListHelper<cPulseSink>::Convert(sinks.Sinks(), *objs);
     state = AddSubMenu(new cPulsecontrolMenuObjectlist(tr("select sink"), objs, &_sink));
     }
  return state;
}

eOSState cPulsecontrolMainMenu::SelectSinkInput(void)
{
  eOSState state = osUnknown;
  cPulseLoop loop;
  cPulseListSinkInputsAction inputs(loop);
  int ret = loop.Run();
  if (ret != 0) {
     }
  else {
     cList<cPulseObject> *objs = new cList<cPulseObject>();
     cListHelper<cPulseSinkInput>::Convert(inputs.SinkInputs(), *objs);
     state = AddSubMenu(new cPulsecontrolMenuObjectlist(tr("select input"), objs, &_input));
     }
  return state;
}
