// This file is generated by WOK (CPPExt).
// Please do not edit this file; modify original file instead.
// The copyright and license terms as defined for the original file apply to 
// this header file considered to be the "object code" form of the original source.

#ifndef _XIntTools_MapOfSurfaceSample_HeaderFile
#define _XIntTools_MapOfSurfaceSample_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _TCollection_BasicMap_HeaderFile
#include <TCollection_BasicMap.hxx>
#endif
#ifndef _Handle_XIntTools_StdMapNodeOfMapOfSurfaceSample_HeaderFile
#include <Handle_XIntTools_StdMapNodeOfMapOfSurfaceSample.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class Standard_DomainError;
class XIntTools_SurfaceRangeSample;
class XIntTools_SurfaceRangeSampleMapHasher;
class XIntTools_StdMapNodeOfMapOfSurfaceSample;
class XIntTools_MapIteratorOfMapOfSurfaceSample;



class XIntTools_MapOfSurfaceSample  : public TCollection_BasicMap {
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

  
  Standard_EXPORT   XIntTools_MapOfSurfaceSample(const Standard_Integer NbBuckets = 1);
  
  Standard_EXPORT     XIntTools_MapOfSurfaceSample& Assign(const XIntTools_MapOfSurfaceSample& Other) ;
    XIntTools_MapOfSurfaceSample& operator =(const XIntTools_MapOfSurfaceSample& Other) 
{
  return Assign(Other);
}
  
  Standard_EXPORT     void ReSize(const Standard_Integer NbBuckets) ;
  
  Standard_EXPORT     void Clear() ;
~XIntTools_MapOfSurfaceSample()
{
  Clear();
}
  
  Standard_EXPORT     Standard_Boolean Add(const XIntTools_SurfaceRangeSample& aKey) ;
  
  Standard_EXPORT     Standard_Boolean Contains(const XIntTools_SurfaceRangeSample& aKey) const;
  
  Standard_EXPORT     Standard_Boolean Remove(const XIntTools_SurfaceRangeSample& aKey) ;





protected:





private:

  
  Standard_EXPORT   XIntTools_MapOfSurfaceSample(const XIntTools_MapOfSurfaceSample& Other);




};





// other Inline functions and methods (like "C++: function call" methods)


#endif