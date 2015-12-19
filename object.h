#ifndef _object_h
#define _object_h

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

template<class T> class cListHelper {
public:
  static void Convert(const cList<T> &src, cList<cPulseObject> &dst)
  {
    for (const T *o = src.First(); o; o = src.Next(o)) {
        const cPulseObject *obj = (const cPulseObject*)o;
        if (obj)
           dst.Add(new cPulseObject(obj->Index(), obj->Name()));
        }
  }

  static void Move(cList<T> &src, cList<T> &dst)
  {
    dst.Clear();
    T *o;
    while ((o = src.First()) != NULL) {
        src.Del(o, false);
        dst.Add(o);
        }
  }

  static const T *Find(const cList<T> &list, uint32_t index)
  {
    for (const T *o = list.First(); o; o = list.Next(o)) {
        const cPulseObject *obj = (const cPulseObject*)o;
        if (obj->Index() == index)
           return o;
        }
    return NULL;
  }

  static const T *Find(const cList<T> &list, const char *name)
  {
    if (name) {
       for (const T *o = list.First(); o; o = list.Next(o)) {
           const cPulseObject *obj = (const cPulseObject*)o;
           if (strcasecmp(obj->Name(), name) == 0)
              return o;
           }
       }
    return NULL;
  }
  }; 
#endif
