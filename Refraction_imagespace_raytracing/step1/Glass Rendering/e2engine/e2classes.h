#pragma once

namespace et
{ 

 template <class t_class>
 class Ce2Singleton
 {
  public:

   static t_class& instance()
   {
    static t_class _instance;
    return _instance;
   }

  protected: 
   Ce2Singleton(){};
   Ce2Singleton(const Ce2Singleton&){};
   Ce2Singleton& operator = (const Ce2Singleton&){};
 };

 template <typename contentType, int count>
 struct Ce2StaticArray
 {
  contentType data[count];

  contentType& operator [](int i) {return data[i];}
  contentType* ptr() {return data;}
 };

}