#ifndef _tools_h
#define _tools_h

#include <vdr/tools.h>

class cPulseObject : public cListObject {
private:
  uint32_t _index;
  cString _name;

public:
  cPulseObject(uint32_t index, const char *name)
   :_index(index)
   ,_name(name)
  {
  }

  uint32_t Index() const { return _index; }
  const char *Name() const { return *_name; }
 
  virtual int Compare(const cListObject &ListObject) const
  {
    const cPulseObject &rhs = (const cPulseObject&)ListObject;
    return strcmp(Name(), rhs.Name());
  }
  };

#endif
