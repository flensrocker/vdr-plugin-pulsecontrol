#ifndef _tools_h
#define _tools_h

#include <vdr/tools.h>

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

#endif

