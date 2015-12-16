#ifndef _action_setcardprofile_h
#define _action_setcardprofile_h

#include "action.h"
#include "card.h"

class cPulseSetCardProfileAction : public cPulseAction {
private:
  cString _card;
  cString _profile;
  bool _success;

  static void set_card_profile_callback(pa_context *c, int success, void *userdata)
  {
    cPulseSetCardProfileAction *action = (cPulseSetCardProfileAction*)userdata;
    action->_success = (success != 0);
    action->SignalReady();
  }

protected:
  virtual pa_operation *Action(pa_context *context)
  {
    _success = false;
    if (isnumber(*_card)) {
       uint32_t index = (uint32_t)strtol(*_card, NULL, 10); 
       return pa_context_set_card_profile_by_index(context, index, *_profile, set_card_profile_callback, this);
       } 
    return pa_context_set_card_profile_by_name(context, *_card, *_profile, set_card_profile_callback, this);
  }

public:
  cPulseSetCardProfileAction(cPulseLoop &loop, const char *card, const char *profile)
   :cPulseAction(loop, "SetCardProfile")
   ,_card(card)
   ,_profile(profile)
   ,_success(false)
  {
  }

  virtual ~cPulseSetCardProfileAction(void)
  {
  }

  bool Success(void) const
  {
    return _success;
  }
  };

#endif
