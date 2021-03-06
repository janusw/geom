// This file is generated by WOK (CPPExt).
// Please do not edit this file; modify original file instead.
// The copyright and license terms as defined for the original file apply to 
// this header file considered to be the "object code" form of the original source.

#ifndef _XBOPTools_IndexedMapNodeOfIMapOfPaveBlock_HeaderFile
#define _XBOPTools_IndexedMapNodeOfIMapOfPaveBlock_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineHandle_HeaderFile
#include <Standard_DefineHandle.hxx>
#endif
#ifndef _Handle_XBOPTools_IndexedMapNodeOfIMapOfPaveBlock_HeaderFile
#include <Handle_XBOPTools_IndexedMapNodeOfIMapOfPaveBlock.hxx>
#endif

#ifndef _XBOPTools_PaveBlock_HeaderFile
#include <XBOPTools_PaveBlock.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _TCollection_MapNodePtr_HeaderFile
#include <TCollection_MapNodePtr.hxx>
#endif
#ifndef _TCollection_MapNode_HeaderFile
#include <TCollection_MapNode.hxx>
#endif
class XBOPTools_PaveBlock;
class XBOPTools_PaveBlockMapHasher;
class XBOPTools_IMapOfPaveBlock;



class XBOPTools_IndexedMapNodeOfIMapOfPaveBlock : public TCollection_MapNode {

public:

  
      XBOPTools_IndexedMapNodeOfIMapOfPaveBlock(const XBOPTools_PaveBlock& K1,const Standard_Integer K2,const TCollection_MapNodePtr& n1,const TCollection_MapNodePtr& n2);
  
        XBOPTools_PaveBlock& Key1() const;
  
        Standard_Integer& Key2() const;
  
        TCollection_MapNodePtr& Next2() const;




  DEFINE_STANDARD_RTTI(XBOPTools_IndexedMapNodeOfIMapOfPaveBlock)

protected:




private: 


XBOPTools_PaveBlock myKey1;
Standard_Integer myKey2;
TCollection_MapNodePtr myNext2;


};

#define TheKey XBOPTools_PaveBlock
#define TheKey_hxx <XBOPTools_PaveBlock.hxx>
#define Hasher XBOPTools_PaveBlockMapHasher
#define Hasher_hxx <XBOPTools_PaveBlockMapHasher.hxx>
#define TCollection_IndexedMapNode XBOPTools_IndexedMapNodeOfIMapOfPaveBlock
#define TCollection_IndexedMapNode_hxx <XBOPTools_IndexedMapNodeOfIMapOfPaveBlock.hxx>
#define Handle_TCollection_IndexedMapNode Handle_XBOPTools_IndexedMapNodeOfIMapOfPaveBlock
#define TCollection_IndexedMapNode_Type_() XBOPTools_IndexedMapNodeOfIMapOfPaveBlock_Type_()
#define TCollection_IndexedMap XBOPTools_IMapOfPaveBlock
#define TCollection_IndexedMap_hxx <XBOPTools_IMapOfPaveBlock.hxx>

#include <TCollection_IndexedMapNode.lxx>

#undef TheKey
#undef TheKey_hxx
#undef Hasher
#undef Hasher_hxx
#undef TCollection_IndexedMapNode
#undef TCollection_IndexedMapNode_hxx
#undef Handle_TCollection_IndexedMapNode
#undef TCollection_IndexedMapNode_Type_
#undef TCollection_IndexedMap
#undef TCollection_IndexedMap_hxx


// other Inline functions and methods (like "C++: function call" methods)


#endif
