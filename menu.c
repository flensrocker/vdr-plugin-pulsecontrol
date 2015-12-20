#include "menu.h"

#include <vdr/skins.h>

#include "action_listcards.h"
#include "action_listsinks.h"
#include "action_listsinkinputs.h"
#include "action_movesinkinput.h"
#include "action_readformats.h"
#include "action_saveformats.h"
#include "action_setcardprofile.h"
#include "action_setdefaultsink.h"
#include "loop.h"


enum eMenuAction { maMoveSinkInput,
                   maSaveFormats,
                   maSetCardProfile,
                   maSetDefaultSink
                 };

class cMenuAction {
public:
  virtual void MenuAction(void)
  {
  }
};

template<class T>
class cPulsecontrolMenuObjectlist : public cOsdMenu, public cMenuAction {
private:
  cList<T> _list;
  T **_dest;

public:
  cPulsecontrolMenuObjectlist(const char *title, const cList<T> &list, T **dest)
   :cOsdMenu(title)
   ,_dest(dest)
  {
    cListHelper<T>::Copy(list, _list);
    for (const T *o = _list.First(); o; o = _list.Next(o)) {
        const cPulseObject *obj = static_cast<const cPulseObject*>(o);
        if (obj)
           Add(new cOsdItem(obj->MenuItemText()));
        }
  }

  virtual ~cPulsecontrolMenuObjectlist(void)
  {
  }

  virtual void MenuAction(void)
  {
    DELETENULL(*_dest);
    int i = Current();
    if ((i >= 0) && (i < _list.Count())) {
       const T *obj = _list.Get(i);
       if (obj)
       *_dest = new T(*obj);
       }
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


static pa_encoding_t encodings[] =
  {
    PA_ENCODING_AC3_IEC61937,
    PA_ENCODING_EAC3_IEC61937,
    PA_ENCODING_MPEG_IEC61937,
    PA_ENCODING_DTS_IEC61937,
    PA_ENCODING_MPEG2_AAC_IEC61937,
    PA_ENCODING_MAX
  };


class cPulsecontrolMenuFormats : public cOsdMenu, public cMenuAction
{
private:
  int _save[PA_ENCODING_MAX];
  cList<cPulseFormat> *_formats;

public:
  cPulsecontrolMenuFormats(const cList<cPulseFormat> &formats, cList<cPulseFormat> *dest)
   :cOsdMenu(tr("select formats"), 20)
   ,_formats(dest)
  {
    for (int i = 0; encodings[i] != PA_ENCODING_MAX; i++) {
        const cPulseFormat *f = cListHelper<cPulseFormat>::Find(formats, encodings[i]);
        if (f) {
           _save[i] = 1;
           Add(new cMenuEditBoolItem(f->Name(), _save + i));
           }
        else {
           cPulseFormat f2(encodings[i]);
           _save[i] = 0;
           Add(new cMenuEditBoolItem(f2.Name(), _save + i));
           }
        }
  }

  virtual void MenuAction(void)
  {
    _formats->Clear();
    _formats->Add(new cPulseFormat(PA_ENCODING_PCM));
    for (int i = 0; encodings[i] != PA_ENCODING_MAX; i++) { 
        if (_save[i])
           _formats->Add(new cPulseFormat(encodings[i]));
        }
  }
};


cPulsecontrolMainMenu::cPulsecontrolMainMenu(void)
 :cOsdMenu("pulsecontrol")
 ,_card(NULL),_profile(NULL)
 ,_sink(NULL),_input(NULL)
{
  Add(new cPulsecontrolMenuItem(maMoveSinkInput, tr("move sink input")));
  Add(new cPulsecontrolMenuItem(maSaveFormats, tr("set passthrough formats")));
  Add(new cPulsecontrolMenuItem(maSetCardProfile, tr("set card profile")));
  Add(new cPulsecontrolMenuItem(maSetDefaultSink, tr("set default sink")));
}

cPulsecontrolMainMenu::~cPulsecontrolMainMenu(void)
{
  Reset();
}

void cPulsecontrolMainMenu::Reset(void)
{
  _cards.Clear();
  _formats.Clear();
  DELETENULL(_card);
  DELETENULL(_profile);
  DELETENULL(_sink);
  DELETENULL(_input);
}

eOSState cPulsecontrolMainMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state != osUnknown)
     return state;

  cPulsecontrolMenuItem *item = dynamic_cast<cPulsecontrolMenuItem*>(Get(Current()));
  if (item && (Key == kOk)) {
     cPulseLoop loop;

     if (HasSubMenu()) {
        cMenuAction *ma = dynamic_cast<cMenuAction*>(SubMenu());
        if (ma)
           ma->MenuAction();
        CloseSubMenu();
        }
     else
        Reset();

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
                cString text = cString::sprintf(tr("error %d on moving sink input"), ret);
                Skins.QueueMessage(mtError, *text);
                state = osEnd;
                }
             Reset();
             }
           break;
         }
       case maSaveFormats:
         {
           if (!_sink)
              state = SelectSink();
           else if (_formats.Count() == 0)
              state = SelectFormats();
           else {
             cPulseSaveFormatsAction action(loop, _sink->Index(), _formats);
             int ret = loop.Run();
             if (ret != 0) {
                cString text = cString::sprintf(tr("error %d on saving formats"), ret);
                Skins.QueueMessage(mtError, *text);
                state = osEnd;
                }
             Reset();
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
                cString text = cString::sprintf(tr("error %d on setting card profile"), ret);
                Skins.QueueMessage(mtError, *text);
                state = osEnd;
                }
             Reset();
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
                cString text = cString::sprintf(tr("error %d on setting default sink"), ret);
                Skins.QueueMessage(mtError, *text);
                state = osEnd;
                }
             Reset();
             }
           break;
         }
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
     cString text = cString::sprintf(tr("error %d on querying cards"), ret);
     Skins.QueueMessage(mtError, *text);
     state = osEnd;
     }
  else {
     cListHelper<cPulseCard>::Copy(cards.Cards(), _cards);
     state = AddSubMenu(new cPulsecontrolMenuObjectlist<cPulseCard>(tr("select card"), cards.Cards(), &_card));
     }
  return state;
}

eOSState cPulsecontrolMainMenu::SelectFormats(void)
{
  eOSState state = osUnknown;
  if (_sink) {
     cPulseLoop loop;
     cPulseReadFormatsAction read(loop, _sink->Index(), PA_DEVICE_TYPE_SINK);
     int ret = loop.Run();
     if (ret != 0) {
        cString text = cString::sprintf(tr("error %d on querying sink formats"), ret);
        Skins.QueueMessage(mtError, *text);
        state = osEnd;
        }
     else {
        state = AddSubMenu(new cPulsecontrolMenuFormats(read.Formats(), &_formats));
        }
     }
  return state;
}

eOSState cPulsecontrolMainMenu::SelectProfile(void)
{
  eOSState state = osUnknown;
  if (_card) {
     const cPulseCard *card = cListHelper<cPulseCard>::Find(_cards, _card->Index());
     if (card) {
        state = AddSubMenu(new cPulsecontrolMenuObjectlist<cPulseProfile>(tr("select profile"), card->Profiles(), &_profile));
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
     cString text = cString::sprintf(tr("error %d on querying sinks"), ret);
     Skins.QueueMessage(mtError, *text);
     state = osEnd;
     }
  else {
     state = AddSubMenu(new cPulsecontrolMenuObjectlist<cPulseSink>(tr("select sink"), sinks.Sinks(), &_sink));
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
     cString text = cString::sprintf(tr("error %d on querying sink inputs"), ret);
     Skins.QueueMessage(mtError, *text);
     state = osEnd;
     }
  else {
     state = AddSubMenu(new cPulsecontrolMenuObjectlist<cPulseSinkInput>(tr("select input"), inputs.SinkInputs(), &_input));
     }
  return state;
}
