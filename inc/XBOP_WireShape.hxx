// This file is generated by WOK (CPPExt).
// Please do not edit this file; modify original file instead.
// The copyright and license terms as defined for the original file apply to 
// this header file considered to be the "object code" form of the original source.

#ifndef _XBOP_WireShape_HeaderFile
#define _XBOP_WireShape_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _TopTools_ListOfShape_HeaderFile
#include <TopTools_ListOfShape.hxx>
#endif
#ifndef _XBOP_Builder_HeaderFile
#include <XBOP_Builder.hxx>
#endif



//!  The Root class to perform a Boolean Operations (BO) <br>
//!  Common,Cut,Fuse  between arguments when one of them is <br>
//!  a wire <br>
class XBOP_WireShape  : public XBOP_Builder {
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

  
//! Empty constructor; <br>
  Standard_EXPORT   XBOP_WireShape();
  
  Standard_EXPORT     void AddSplitPartsINOUT() ;
  
  Standard_EXPORT     void AddSplitPartsON() ;
  
//! Constructs the result of the BO <br>
  Standard_EXPORT     void MakeResult() ;





protected:



TopTools_ListOfShape myLS;


private:





};





// other Inline functions and methods (like "C++: function call" methods)


#endif