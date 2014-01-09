// This file is generated by WOK (CPPExt).
// Please do not edit this file; modify original file instead.
// The copyright and license terms as defined for the original file apply to 
// this header file considered to be the "object code" form of the original source.

#ifndef _XBOPTools_ListIteratorOfListOfPave_HeaderFile
#define _XBOPTools_ListIteratorOfListOfPave_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _Standard_Address_HeaderFile
#include <Standard_Address.hxx>
#endif
#ifndef _Handle_XBOPTools_ListNodeOfListOfPave_HeaderFile
#include <Handle_XBOPTools_ListNodeOfListOfPave.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class XBOPTools_ListOfPave;
class XBOPTools_Pave;
class XBOPTools_ListNodeOfListOfPave;



class XBOPTools_ListIteratorOfListOfPave  {
public:

  void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  void* operator new(size_t size) 
  {
    return Standard::Allocate(size); 
  }
  void  operator delete(void *anAddress) 
  {
    if (anAddress) Standard::Free((Standard_Address&)anAddress); 
  }

  
  Standard_EXPORT   XBOPTools_ListIteratorOfListOfPave();
  
  Standard_EXPORT   XBOPTools_ListIteratorOfListOfPave(const XBOPTools_ListOfPave& L);
  
  Standard_EXPORT     void Initialize(const XBOPTools_ListOfPave& L) ;
  
        Standard_Boolean More() const;
  
  Standard_EXPORT     void Next() ;
  
  Standard_EXPORT     XBOPTools_Pave& Value() const;


friend class XBOPTools_ListOfPave;



protected:





private:



Standard_Address current;
Standard_Address previous;


};

#define Item XBOPTools_Pave
#define Item_hxx <XBOPTools_Pave.hxx>
#define TCollection_ListNode XBOPTools_ListNodeOfListOfPave
#define TCollection_ListNode_hxx <XBOPTools_ListNodeOfListOfPave.hxx>
#define TCollection_ListIterator XBOPTools_ListIteratorOfListOfPave
#define TCollection_ListIterator_hxx <XBOPTools_ListIteratorOfListOfPave.hxx>
#define Handle_TCollection_ListNode Handle_XBOPTools_ListNodeOfListOfPave
#define TCollection_ListNode_Type_() XBOPTools_ListNodeOfListOfPave_Type_()
#define TCollection_List XBOPTools_ListOfPave
#define TCollection_List_hxx <XBOPTools_ListOfPave.hxx>

#include <TCollection_ListIterator.lxx>

#undef Item
#undef Item_hxx
#undef TCollection_ListNode
#undef TCollection_ListNode_hxx
#undef TCollection_ListIterator
#undef TCollection_ListIterator_hxx
#undef Handle_TCollection_ListNode
#undef TCollection_ListNode_Type_
#undef TCollection_List
#undef TCollection_List_hxx


// other Inline functions and methods (like "C++: function call" methods)


#endif