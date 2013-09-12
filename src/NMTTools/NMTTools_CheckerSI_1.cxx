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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:        NMTTools_CheckerSI.cxx
// Created:     Mon Feb 19 11:32:08 2007
// Author:      Peter KURNEV
//
#include <NMTTools_CheckerSI.hxx>
#include <NMTDS_ShapesDataStructure.hxx>
#include <NMTDS_IteratorCheckerSI.hxx>

#include <NMTDS_InterfPool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BOPTools_PaveSet.hxx>
#include <BOPTools_PaveBlockIterator.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <IntTools_Range.hxx>
#include <BOPTools_Pave.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <BOPTColStd_Failure.hxx>
#include <BOPTColStd_Dump.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>

//=======================================================================
// function: PreparePaveBlocks
// purpose:
//=======================================================================
  void NMTTools_CheckerSI::PreparePaveBlocks(const TopAbs_ShapeEnum aType1,
                                             const TopAbs_ShapeEnum aType2)
{
  NMTTools_PaveFiller::PreparePaveBlocks(aType1, aType2);
}
//=======================================================================
// function: PreparePaveBlocks
// purpose:
//=======================================================================
  void NMTTools_CheckerSI::PreparePaveBlocks(const Standard_Integer nE)
{
  //Standard_Boolean bIsValid;
  Standard_Integer nV1, nV2, iErr;
  Standard_Real aT1, aT2;
  TopoDS_Edge aE;
  TopoDS_Vertex aV1, aV2;
  // 
  myIsDone=Standard_False;
  //
  BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(nE));
  // Edge
  aE=TopoDS::Edge(myDS->Shape(nE));
  if (BRep_Tool::Degenerated(aE)) {
    myIsDone=Standard_True;
    return;
  }
  //
  BOPTools_PaveSet& aPS=myPavePool(myDS->RefEdge(nE));

  BOPTools_PaveBlockIterator aPBIt(nE, aPS);
  for (; aPBIt.More(); aPBIt.Next()) {
    BOPTools_PaveBlock& aPB=aPBIt.Value();
    const IntTools_Range& aRange=aPB.Range();
    //
    const BOPTools_Pave& aPave1=aPB.Pave1();
    nV1=aPave1.Index();
    aV1=TopoDS::Vertex(myDS->Shape(nV1));
    aT1=aPave1.Param();
    //
    const BOPTools_Pave& aPave2=aPB.Pave2();
    nV2=aPave2.Index();
    aV2=TopoDS::Vertex(myDS->Shape(nV2));
    aT2=aPave2.Param();
    //
    //modified by NIZNHY-PKV Thu Nov 01 13:46:00 2012f
    //w/a: http://salome.mantis.opencascade.com/view.php?id=21835 
    /*
    bIsValid=Standard_True;
    if (nV1==nV2) {
      bIsValid=IsValid(aE, aV1, aT1, aT2);
      if (!bIsValid) {
        myStopStatus=1;
      }
    }
    */
    //modified by NIZNHY-PKV Thu Nov 01 13:46:09 2012t
    //
    IntTools_ShrunkRange aSR (aE, aV1, aV2, aRange, myContext);
    iErr=aSR.ErrorStatus();
    if (!aSR.IsDone()) {
      aSR.SetShrunkRange(aRange);
    }
    else if (iErr!=6) {
      CorrectShrunkRanges (0, aPave1, aSR);
      CorrectShrunkRanges (1, aPave2, aSR);
    }
    aPB.SetShrunkRange(aSR);
    aLPB.Append(aPB);
  } //for (; aPBIt.More(); aPBIt.Next())
  myIsDone=Standard_True;
}
//modified by NIZNHY-PKV Thu Nov 01 13:46:55 2012f
//w/a: http://salome.mantis.opencascade.com/view.php?id=21835 
/*
//=======================================================================
//function : IsValid
//purpose  :
//=======================================================================
Standard_Boolean IsValid(const TopoDS_Edge& aE,
                         const TopoDS_Vertex& aV,
                         const Standard_Real aTV1,
                         const Standard_Real aTV2)
{
  Standard_Boolean bRet;
  Standard_Integer i, aNbP, aNbP1;
  Standard_Real aTolV2, aTC1, aTC2, dT, aTC, aD2;
  Handle(Geom_Curve) aC;
  gp_Pnt aPV, aPC;
  //
  bRet=Standard_False;
  aTolV2=BRep_Tool::Tolerance(aV);
  aTolV2=aTolV2*aTolV2;
  aPV=BRep_Tool::Pnt(aV);
  aC=BRep_Tool::Curve(aE, aTC1, aTC2);
  aNbP=7;
  aNbP1=aNbP-1;
  dT=(aTV2-aTV1)/aNbP1;
  //
  for (i=1; i<aNbP-1 && !bRet ; ++i) {
    aTC=aTV1+dT*i;
    aC->D0(aTC, aPC);
    aD2=aPV.SquareDistance(aPC);
    bRet=aD2>aTolV2;
  }
  return bRet;
}
*/
//modified by NIZNHY-PKV Thu Nov 01 13:47:07 2012t
