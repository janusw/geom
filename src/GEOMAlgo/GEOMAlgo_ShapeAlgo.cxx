// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:        GEOMAlgo_ShapeAlgo.cxx
// Created:     Tue Dec  7 12:06:54 2004
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#include <GEOMAlgo_ShapeAlgo.hxx>
#include <XIntTools_Context.hxx>

//=======================================================================
//function : GEOMAlgo_ShapeAlgo
//purpose  :
//=======================================================================
GEOMAlgo_ShapeAlgo::GEOMAlgo_ShapeAlgo()
:
  GEOMAlgo_Algo()
{
  myTolerance=0.0001;
}

//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_ShapeAlgo::~GEOMAlgo_ShapeAlgo()
{
}
//=======================================================================
//function : SetContext
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeAlgo::SetContext(const Handle(XIntTools_Context)& theContext)
{
  myContext=theContext;
}
//=======================================================================
//function : Context
//purpose  :
//=======================================================================
const Handle(XIntTools_Context)& GEOMAlgo_ShapeAlgo::Context()const
{
  return myContext;
}
//=======================================================================
//function : SetShape
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeAlgo::SetShape(const TopoDS_Shape& aS)
{
  myShape=aS;
}
//=======================================================================
//function : Shape
//purpose  :
//=======================================================================
const TopoDS_Shape& GEOMAlgo_ShapeAlgo::Shape()const
{
  return myShape;
}
//=======================================================================
//function : SetTolerance
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeAlgo::SetTolerance(const Standard_Real aT)
{
  myTolerance=aT;
}
//=======================================================================
//function : Tolerance
//purpose  :
//=======================================================================
Standard_Real GEOMAlgo_ShapeAlgo::Tolerance()const
{
  return myTolerance;
}
//=======================================================================
//function : Result
//purpose  :
//=======================================================================
const TopoDS_Shape& GEOMAlgo_ShapeAlgo::Result()const
{
  return myResult;
}
//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeAlgo::Perform()
{
  if (myContext.IsNull()) {
    myContext=new XIntTools_Context;
  }
}
