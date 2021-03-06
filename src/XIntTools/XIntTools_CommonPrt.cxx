// Created on: 2000-10-27
// Created by: Peter KURNEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <XIntTools_CommonPrt.ixx>
#include <XIntTools_Range.hxx>
#include <XIntTools_SequenceOfRanges.hxx>

//=======================================================================
//function : XIntTools_CommonPrt::XIntTools_CommonPr
//purpose  : 
//=======================================================================
XIntTools_CommonPrt::XIntTools_CommonPrt() 
  :
  myType(TopAbs_SHAPE),
  myAllNullFlag(Standard_False)
  
{
  //
  myPnt1.SetCoord(0.,0.,0.);
  myPnt2.SetCoord(0.,0.,0.);
  //
  // modified by NIZHNY-MKK  Wed Jun  8 16:47:04 2005.BEGIN
  myVertPar1 = 0.;
  myVertPar2 = 0.;
  // modified by NIZHNY-MKK  Wed Jun  8 16:47:07 2005.END
}

//=======================================================================
//function : XIntTools_CommonPrt::XIntTools_CommonPrt
//purpose  : 
//=======================================================================
  XIntTools_CommonPrt::XIntTools_CommonPrt(const XIntTools_CommonPrt& Other) 
:
  myEdge1(Other.myEdge1),
  myEdge2(Other.myEdge2),
  myType (Other.myType),
  myRange1 (Other.myRange1),
  myVertPar1(Other.myVertPar1),
  myVertPar2(Other.myVertPar2),
  myAllNullFlag(Standard_False),
//
  myPnt1(Other.myPnt1),
  myPnt2(Other.myPnt2)
//
{
  Standard_Integer i, aNb=Other.myRanges2.Length();
  for (i=1; i<=aNb; i++) {
    myRanges2.Append(Other.myRanges2(i));
  }
}

//=======================================================================
//function : XIntTools_CommonPrt::Assign
//purpose  : 
//=======================================================================
  XIntTools_CommonPrt& XIntTools_CommonPrt::Assign(const XIntTools_CommonPrt& Other) 
{
  myEdge1=Other.myEdge1;
  myEdge2=Other.myEdge2;
  myType =Other.myType;
  myRange1 =Other.myRange1;
  myVertPar1=Other.myVertPar1;
  myVertPar2=Other.myVertPar2;
  //
  myPnt1=Other.myPnt1;
  myPnt2=Other.myPnt2;
  //
  Standard_Integer i, aNb=Other.myRanges2.Length();
  for (i=1; i<=aNb; i++) {
    myRanges2.Append(Other.myRanges2(i));
  }
  myAllNullFlag=Other.myAllNullFlag;
  return *this;
}

//=======================================================================
//function : SetEdge1
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetEdge1(const TopoDS_Edge& anEdge)
{
  myEdge1=anEdge;
}

//=======================================================================
//function : SetEdge2
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetEdge2(const TopoDS_Edge& anEdge)
{
  myEdge2=anEdge;
}

//=======================================================================
//function : SetType
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetType (const TopAbs_ShapeEnum aType)
{
  myType=aType;
}

//=======================================================================
//function : SetRange1
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetRange1 (const XIntTools_Range& aRange)
{
  myRange1=aRange;
}

//=======================================================================
//function : SetRange1
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetRange1 (const Standard_Real tf,
				      const Standard_Real tl)
{
  myRange1.SetFirst(tf);
  myRange1.SetLast (tl);
}

//=======================================================================
//function : AppendRange2
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::AppendRange2 (const XIntTools_Range& aRange)
{
  myRanges2.Append(aRange);
}
//=======================================================================
//function : AppendRange2
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::AppendRange2 (const Standard_Real tf,
					 const Standard_Real tl)
{
  XIntTools_Range aRange(tf, tl);
  myRanges2.Append(aRange);
}
//=======================================================================
//function : SetVertexParameter1
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetVertexParameter1(const Standard_Real tV)
{
  myVertPar1=tV;
}
//=======================================================================
//function : SetVertexParameter2
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetVertexParameter2(const Standard_Real tV)
{
  myVertPar2=tV;
}

//=======================================================================
//function : Edge1
//purpose  : 
//=======================================================================
  const TopoDS_Edge& XIntTools_CommonPrt::Edge1() const 
{
  return myEdge1;
}

//=======================================================================
//function : Edge2
//purpose  : 
//=======================================================================
  const TopoDS_Edge& XIntTools_CommonPrt::Edge2() const 
{
  return myEdge2;
}

//=======================================================================
//function : TopAbs_ShapeEnum
//purpose  : 
//=======================================================================
  TopAbs_ShapeEnum XIntTools_CommonPrt::Type() const 
{
  return myType;
}

//=======================================================================
//function : Range1
//purpose  : 
//=======================================================================
  const XIntTools_Range&  XIntTools_CommonPrt::Range1() const 
{ 
  return myRange1;
}

//=======================================================================
//function : Range1
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::Range1(Standard_Real& tf,
				  Standard_Real& tl) const 
{ 
  tf=myRange1.First();
  tl=myRange1.Last();
}
//=======================================================================
//function : Ranges2
//purpose  : 
//=======================================================================
   const XIntTools_SequenceOfRanges& XIntTools_CommonPrt::Ranges2() const 
{ 
  return myRanges2;
}
//=======================================================================
//function : ChangeRanges2
//purpose  : 
//=======================================================================
  XIntTools_SequenceOfRanges& XIntTools_CommonPrt::ChangeRanges2()
{ 
  return myRanges2;
}

//=======================================================================
//function : VertexParameter1
//purpose  : 
//=======================================================================
  Standard_Real XIntTools_CommonPrt::VertexParameter1() const 
{ 
  return myVertPar1;
}
//=======================================================================
//function : VertexParameter2
//purpose  : 
//=======================================================================
  Standard_Real XIntTools_CommonPrt::VertexParameter2() const 
{ 
  return myVertPar2;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================
   void XIntTools_CommonPrt::Copy(XIntTools_CommonPrt& aCP) const 
{ 
  aCP.SetEdge1(Edge1());
  aCP.SetEdge2(Edge2());
  aCP.SetType (Type());
  aCP.SetRange1(Range1());
  aCP.SetVertexParameter1(myVertPar1);
  aCP.SetVertexParameter2(myVertPar2);

  XIntTools_SequenceOfRanges aSeqRanges;
  Standard_Integer i, aNb;
  aNb=myRanges2.Length();
  for (i=1; i<=aNb; i++) {
    aCP.AppendRange2(myRanges2(i));
  }
}

//=======================================================================
//function : SetAllNullFlag
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetAllNullFlag(const Standard_Boolean aFlag) 
{
  myAllNullFlag=aFlag;
} 

//=======================================================================
//function : AllNullFlag
//purpose  : 
//=======================================================================
  Standard_Boolean XIntTools_CommonPrt::AllNullFlag()const 
{
  return myAllNullFlag;
} 

//
//=======================================================================
//function : SetBoundingPoints
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::SetBoundingPoints(const gp_Pnt& aP1, 
					     const gp_Pnt& aP2)
{
  myPnt1=aP1;
  myPnt2=aP2;
} 
//=======================================================================
//function : BoundingPoints
//purpose  : 
//=======================================================================
  void XIntTools_CommonPrt::BoundingPoints(gp_Pnt& aP1, 
					  gp_Pnt& aP2) const
{
  aP1=myPnt1;
  aP2=myPnt2;
} 
//
