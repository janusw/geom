// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:        NMTDS_BndSphere.hxx
// Created:     
// Author:      Peter KURNEV
//              <pkv@irinox>

#ifndef _NMTDS_BndSphere_HeaderFile
#define _NMTDS_BndSphere_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

//=======================================================================
//class : NMTDS_BndSphere
//purpose  : 
//=======================================================================
class NMTDS_BndSphere  {
 public:
  Standard_EXPORT   
    NMTDS_BndSphere();
  
  Standard_EXPORT 
    virtual ~NMTDS_BndSphere();
  
  void SetCenter(const gp_Pnt& theP) ;
  
  const gp_Pnt& Center() const;
  
  void SetRadius(const Standard_Real theR) ;
  
  Standard_Real Radius() const;
  
  void SetGap(const Standard_Real theGap) ;
  
  Standard_Real Gap() const;
  
  void Add(const NMTDS_BndSphere& theOther) ;
  
  Standard_EXPORT 
    Standard_Boolean IsOut(const NMTDS_BndSphere& theOther) const;
  
  Standard_Real SquareExtent() const;

 protected:
  gp_Pnt myCenter;
  Standard_Real myRadius;
  Standard_Real myGap;
};

//=======================================================================
//function : SetCenter
//purpose  : 
//=======================================================================
  inline void NMTDS_BndSphere::SetCenter(const gp_Pnt& theP)
//
{
  myCenter=theP;
}
//=======================================================================
//function : Center
//purpose  : 
//=======================================================================
  inline const gp_Pnt& NMTDS_BndSphere::Center()const
{
  return myCenter;
}
//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================
  inline void NMTDS_BndSphere::SetRadius(const Standard_Real theR)
{
  myRadius=theR;
}
//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================
  inline Standard_Real NMTDS_BndSphere::Radius()const
{
  return myRadius;
}
//=======================================================================
//function : SetGap
//purpose  : 
//=======================================================================
  inline void NMTDS_BndSphere::SetGap(const Standard_Real theGap)
{
  myGap=theGap;
}
//=======================================================================
//function : Gap
//purpose  : 
//=======================================================================
  inline Standard_Real NMTDS_BndSphere::Gap()const
{
  return myGap;
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
  inline void NMTDS_BndSphere::Add(const NMTDS_BndSphere& theOther)
{
  Standard_Real aTmax, aR, aT, aTOther;
  gp_Pnt aPc;
  //
  aPc.SetXYZ(0.5*(myCenter.XYZ()+theOther.myCenter.XYZ()));
  //
  aR=aPc.Distance(myCenter);
  //
  aT=myRadius+myGap;
  aTOther=theOther.myRadius+theOther.myGap;
  aTmax=(aT>aTOther) ? aT: aTOther;
  //
  aR=aR+aTmax;
  //
  myCenter=aPc;
  myRadius=aR;
}
//=======================================================================
//function : SquareExtent
//purpose  : 
//=======================================================================
  inline Standard_Real NMTDS_BndSphere::SquareExtent()const
{
  Standard_Real aD;
  //
  aD=myRadius+myGap;
  aD=aD+aD;
  aD=aD*aD;
  //
  return aD;
}

#endif
