#ifndef _card_h
#define _card_h

#include "profile.h"

class cPulseCard : public cPulseObject {
private:
  cList<cPulseProfile> _profiles;
  cPulseProfile *_active_profile;

public:
  cPulseCard(uint32_t index, const char *name)
   :cPulseObject(index, name)
   ,_active_profile(NULL)
  {
  }

  virtual ~cPulseCard(void)
  {
  }

  void AddProfile(const char *name, bool isactive)
  {
    cPulseProfile *p = new cPulseProfile(name);
    if (isactive)
       _active_profile = p; 
    _profiles.Add(p);
  }

  const cPulseProfile *ActiveProfile(void) const
  {
    return _active_profile;
  }

  const cList<cPulseProfile> &Profiles(void) const
  {
    return _profiles;
  }
  };

#endif
