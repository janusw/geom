// Created on: 2001-09-12
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <XBOPTools_DEInfo.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

//=======================================================================
// function: XBOPTools_DEInfo::XBOPTools_DEInfo
// purpose: 
//=======================================================================
XBOPTools_DEInfo::XBOPTools_DEInfo()
{}

//=======================================================================
// function: SetVertex
// purpose: 
//=======================================================================
  void XBOPTools_DEInfo::SetVertex(const Standard_Integer nV)
{
  myVertex=nV;
}

//=======================================================================
// function: Vertex
// purpose: 
//=======================================================================
  Standard_Integer XBOPTools_DEInfo::Vertex()const  
{
  return myVertex;
}
//=======================================================================
// function: SetFaces
// purpose: 
//=======================================================================
  void XBOPTools_DEInfo::SetFaces(const TColStd_ListOfInteger& aLI)
{
  myFaces.Clear();

  TColStd_ListIteratorOfListOfInteger anIt(aLI);
  for (; anIt.More();  anIt.Next()) {
    Standard_Integer nF=anIt.Value();
    myFaces.Append(nF);
  }
}
//=======================================================================
// function: Faces
// purpose: 
//=======================================================================
  const TColStd_ListOfInteger& XBOPTools_DEInfo::Faces()const 
{
  return myFaces;
}
//=======================================================================
// function: ChangeFaces
// purpose: 
//=======================================================================
  TColStd_ListOfInteger& XBOPTools_DEInfo::ChangeFaces() 
{
  return myFaces;
}


