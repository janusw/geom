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
// File:        GEOMAlgo_Builder_2.cxx
// Author:      Peter KURNEV

#include <GEOMAlgo_Builder.hxx>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>

#include <TopAbs_Orientation.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgo_Image.hxx>
#include <BRepTools.hxx>

#include <XIntTools_Context.hxx>
#include <XIntTools_FClass2d.hxx>

#include <XBooleanOperations_OnceExplorer.hxx>
#include <XBOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>
#include <XBOPTools_ListOfPaveBlock.hxx>
#include <XBOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <XBOPTools_CArray1OfSSInterference.hxx>
#include <XBOPTools_SSInterference.hxx>
#include <XBOPTools_SequenceOfCurves.hxx>
#include <XBOPTools_Curve.hxx>
#include <XBOPTools_ListOfPaveBlock.hxx>
#include <XBOPTools_PaveBlock.hxx>
#include <XBOPTools_Tools3D.hxx>
#include <XBOPTools_CArray1OfVSInterference.hxx>
#include <XBOPTools_VSInterference.hxx>
#include <XBOPTools_ESInterference.hxx>
#include <XBOPTools_CArray1OfESInterference.hxx>

#include <NMTDS_ShapesDataStructure.hxx>
#include <NMTDS_InterfPool.hxx>

#include <NMTTools_PaveFiller.hxx>
#include <NMTTools_ListOfCoupleOfShape.hxx>
#include <NMTTools_Tools.hxx>
#include <NMTTools_CoupleOfShape.hxx>
#include <NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx>
#include <NMTTools_Tools.hxx>
#include <NMTTools_ListIteratorOfListOfCommonBlock.hxx>
#include <NMTTools_ListOfCommonBlock.hxx>
#include <NMTTools_CommonBlock.hxx>
#include <NMTTools_IndexedDataMapOfIndexedMapOfInteger.hxx>
//
#include <GEOMAlgo_Tools3D.hxx>
#include <GEOMAlgo_WireEdgeSet.hxx>
#include <GEOMAlgo_BuilderFace.hxx>

#include <GEOMAlgo_ShapeSet.hxx>
//
#include <NMTDS_BoxBndTree.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>

static
  void UpdateCandidates(const Standard_Integer ,
                        const Standard_Integer ,
                        NMTTools_IndexedDataMapOfIndexedMapOfInteger& );

static
  Standard_Boolean IsClosed(const TopoDS_Edge& ,
                            const TopoDS_Face&,
                            Standard_Boolean&  );

//=======================================================================
//function : FillImagesFaces
//purpose  :
//=======================================================================
void GEOMAlgo_Builder::FillImagesFaces()
{
  myErrorStatus=0;
  //
  FillIn2DParts();
  BuildSplitFaces();
  FillSameDomainFaces();
  FillImagesFaces1();
  FillInternalVertices();
}

//=======================================================================
// function: FillIn2DParts
// purpose:
//=======================================================================
void GEOMAlgo_Builder::FillIn2DParts()
{
  const NMTDS_ShapesDataStructure& aDS=*myPaveFiller->DS();
  NMTTools_PaveFiller* pPF=myPaveFiller;
  NMTDS_InterfPool* pIP=pPF->IP();
  XBOPTools_CArray1OfSSInterference& aFFs=pIP->SSInterferences();
  NMTTools_CommonBlockPool& aCBP=pPF->ChangeCommonBlockPool();
  //
  Standard_Integer  j, nSpIn, nSpSc, aNbCurves;
  Standard_Integer aNbS, nF, aNbCBP, n1, n2, aNbFFs, aNbSpIn;
  TopTools_MapOfShape  aMFence;
  TopTools_ListOfShape aLSpIn;
  TopoDS_Face aF;
  NMTTools_ListIteratorOfListOfCommonBlock aItCB;
  XBOPTools_ListIteratorOfListOfPaveBlock aItPB;
  //
  myInParts.Clear();
  //
  aNbFFs=aFFs.Extent();
  aNbCBP=aCBP.Extent();
  //
  aNbS=aDS.NumberOfShapesOfTheObject();
  for (nF=1; nF<=aNbS; ++nF) {
    if (aDS.GetShapeType(nF)!=TopAbs_FACE) {
      continue;
    }
    //
    aF=TopoDS::Face(aDS.Shape(nF));
    //
    aMFence.Clear();
    aLSpIn.Clear();
    //
    // 1. In Parts
    XBOPTools_ListOfPaveBlock aLPBIn;
    //
    pPF->RealSplitsInFace(nF, aLPBIn);
    //
    aItPB.Initialize(aLPBIn);
    for (; aItPB.More(); aItPB.Next()) {
      const XBOPTools_PaveBlock& aPB1=aItPB.Value();
      nSpIn=aPB1.Edge();
      const TopoDS_Shape& aSpIn=aDS.Shape(nSpIn);
      aLSpIn.Append(aSpIn);
    }
    //
    // 2. Section Parts
    for (j=1; j<=aNbFFs; ++j) {
      XBOPTools_SSInterference& aFF=aFFs(j);
      aFF.Indices(n1, n2);
      if (!(n1==nF || n2==nF)) {
        continue;
      }
      XBOPTools_SequenceOfCurves& aSC=aFF.Curves();
      aNbCurves=aSC.Length();
      if (!aNbCurves) {
        continue;
      }
      //
      const XBOPTools_Curve& aBC=aSC(1);
      const XBOPTools_ListOfPaveBlock& aLPB=aBC.NewPaveBlocks();
      aItPB.Initialize(aLPB);
      for (; aItPB.More(); aItPB.Next()) {
        const XBOPTools_PaveBlock& aPBSc=aItPB.Value();
        nSpSc=aPBSc.Edge();
        const TopoDS_Shape& aSpSc=aDS.Shape(nSpSc);
        if (aMFence.Add(aSpSc)){
          aLSpIn.Append(aSpSc);
        }
      }
    }
    aNbSpIn=aLSpIn.Extent();
    if (aNbSpIn) {
      myInParts.Add(aF, aLSpIn);
    }
  }//for (nF=1; nF<=aNbS; ++nF) {
}

//=======================================================================
// function: BuildSplitFaces
// purpose:
//=======================================================================
void GEOMAlgo_Builder::BuildSplitFaces()
{
  const NMTDS_ShapesDataStructure& aDS=*myPaveFiller->DS();
  NMTTools_PaveFiller* pPF=myPaveFiller;
  NMTDS_InterfPool* pIP=pPF->IP();
  XBOPTools_CArray1OfSSInterference& aFFs=pIP->SSInterferences();
  const Handle(XIntTools_Context)& aCtx= pPF->Context();
  //
  Standard_Boolean bToReverse, bIsClosed, bIsDegenerated;
  Standard_Boolean bFlagClosed;
  Standard_Integer i, aNb, aNbF, nF;
  TopTools_MapOfShape aMFence;
  TColStd_IndexedMapOfInteger aMFP;
  TopExp_Explorer anExp;
  TopoDS_Face aFF;
  TopoDS_Edge aSp, aEE;
  TopTools_ListIteratorOfListOfShape aIt;
  TopAbs_Orientation anOriF, anOriE;
  //
  mySplitFaces.Clear();
  //
  // 1. Select Faces to process (MFP)
  aNb=aDS.NumberOfShapesOfTheObject();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aF=aDS.Shape(i);
    if (aF.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    if (!aMFence.Add(aF)) {
      continue;
    }
    //
    if (myInParts.Contains(aF)) {
      aMFP.Add(i);
      continue;
    }
    //
    anExp.Init(aF, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aE=anExp.Current();
      if (myImages.HasImage(aE)) {
        aMFP.Add(i);
        break;
      }
    }
    //
    //===
    {
      Standard_Integer aNbFFs, aNbSE, j, n1, n2;
      //
      aNbFFs=aFFs.Extent();
      for (j=1; j<=aNbFFs; ++j) {
        XBOPTools_SSInterference& aFFj=aFFs(j);
        aFFj.Indices(n1, n2);
        if (!(n1==i || n2==i)) {
          continue;
        }
        //
        const TColStd_ListOfInteger& aLSE=aFFj.SharedEdges();
        aNbSE=aLSE.Extent();
        if (aNbSE) {
          aMFP.Add(i);
          break;
        }
      }
    }
    //===
    //
  }// for (i=1; i<=aNb; ++i)
  //
  // 2. ProcessFaces
  aNbF=aMFP.Extent();
  for (i=1; i<=aNbF; ++i) {
    nF=aMFP(i);
    const TopoDS_Face& aF=TopoDS::Face(aDS.Shape(nF));
    anOriF=aF.Orientation();
    aFF=aF;
    aFF.Orientation(TopAbs_FORWARD);
    //
    aMFence.Clear();
    //
    // 2.1. Fill WES
    GEOMAlgo_WireEdgeSet aWES;
    aWES.SetFace(aFF);
    //
    //  2.1.1. Add Split parts
    anExp.Init(aFF, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Edge& aE=TopoDS::Edge(anExp.Current());
      anOriE=aE.Orientation();
      //
      if (!myImages.HasImage(aE)) {
        if (anOriE==TopAbs_INTERNAL) {
          aEE=aE;
          aEE.Orientation(TopAbs_FORWARD);
          aWES.AddStartElement(aEE);
          aEE.Orientation(TopAbs_REVERSED);
          aWES.AddStartElement(aEE);
        }
        else {
          aWES.AddStartElement(aE);
        }
        continue;
      }
      //
      bIsDegenerated=BRep_Tool::Degenerated(aE);
      bIsClosed=IsClosed(aE, aF, bFlagClosed);
      //
      const TopTools_ListOfShape& aLIE=myImages.Image(aE);
      aIt.Initialize(aLIE);
      for (; aIt.More(); aIt.Next()) {
        aSp=TopoDS::Edge(aIt.Value());
        //
        if (bIsDegenerated) {
          aSp.Orientation(anOriE);
          aWES.AddStartElement(aSp);
          continue;
        }
        //
        if (anOriE==TopAbs_INTERNAL) {
          aSp.Orientation(TopAbs_FORWARD);
          aWES.AddStartElement(aSp);
          aSp.Orientation(TopAbs_REVERSED);
          aWES.AddStartElement(aSp);
          continue;
        }
        //
        if (bIsClosed){
          if (aMFence.Add(aSp)) {
            //
            if (!BRep_Tool::IsClosed(aSp, aF)){
              XBOPTools_Tools3D::DoSplitSEAMOnFace(aSp, aF);
            }
            //
            aSp.Orientation(TopAbs_FORWARD);
            aWES.AddStartElement(aSp);
            aSp.Orientation(TopAbs_REVERSED);
            aWES.AddStartElement(aSp);
          }
          continue;
        }//  if (bIsClosed){
        //
        //modified by NIZNHY-PKV Wed Nov 28 13:50:34 2012f
        if (!bIsClosed && bFlagClosed) {
          if (!BRep_Tool::IsClosed(aSp, aF)){
            XBOPTools_Tools3D::DoSplitSEAMOnFace(aSp, aF);
          }
        }
        //modified by NIZNHY-PKV Wed Nov 28 13:50:36 2012t
        //
        //
        aSp.Orientation(anOriE);
        bToReverse=XBOPTools_Tools3D::IsSplitToReverse1(aSp, aE, aCtx);
        if (bToReverse) {
          aSp.Reverse();
        }
        aWES.AddStartElement(aSp);
      }// for (; aIt.More(); aIt.Next()) {
    }// for (; anExp.More(); anExp.Next()) {
    //
    // 2.1.2. Add In2D Parts
    if (myInParts.Contains(aF)) {
      const TopTools_ListOfShape& aLE=myInParts.FindFromKey(aF);
      aIt.Initialize(aLE);
      for (; aIt.More(); aIt.Next()) {
        aSp=TopoDS::Edge(aIt.Value());
        //
        aSp.Orientation(TopAbs_FORWARD);
        aWES.AddStartElement(aSp);
        //
        aSp.Orientation(TopAbs_REVERSED);
        aWES.AddStartElement(aSp);
      }
    }
    //
    // 2.2. Build images Faces
    TopTools_ListOfShape aLFR;
    GEOMAlgo_ShapeSet aS1, aS2;
    //
    const TopTools_ListOfShape& aSE=aWES.StartElements();
    aS1.Add(aSE);
    aS2.Add(aFF, TopAbs_EDGE);
    if (aS1.IsEqual(aS2)) {
      aLFR.Append(aF);
    }
    else {
      GEOMAlgo_BuilderFace aBF;
      //
      aBF.SetFace(aFF);
      aBF.SetContext(aCtx);
      aBF.SetShapes(aSE);
      // <-DEB
      aBF.Perform();
      //
      const TopTools_ListOfShape& aLF=aBF.Areas();
      aIt.Initialize(aLF);
      for (; aIt.More(); aIt.Next()) {
        TopoDS_Shape& aFR=aIt.Value();
        if (anOriF==TopAbs_REVERSED) {
          aFR.Orientation(TopAbs_REVERSED);
        }
        aLFR.Append(aFR);
      }
    }
    //
    // 2.3. Collect draft images Faces
    mySplitFaces.Bind(aF, aLFR);
  }//for (i=1; i<=aNbF; ++i)
}

//=======================================================================
// function: FillSameDomainFaces
// purpose:
//=======================================================================
void GEOMAlgo_Builder::FillSameDomainFaces()
{
  Standard_Boolean bIsSDF, bHasImage1, bHasImage2, bForward;
  Standard_Integer i, j, aNbFF, nF1, nF2, aNbPBInOn, aNbC, aNbSE;
  Standard_Integer aNbF1, aNbF2, i2s, aNbSD;
  TopTools_MapOfShape aMFence;
  TopTools_ListOfShape aLX1, aLX2;
  TopTools_ListIteratorOfListOfShape aItF1, aItF2;
  NMTTools_ListOfCoupleOfShape aLCS;
  //
  const NMTDS_ShapesDataStructure& aDS=*myPaveFiller->DS();
  NMTTools_PaveFiller* pPF=myPaveFiller;
  NMTDS_InterfPool* pIP=pPF->IP();
  XBOPTools_CArray1OfSSInterference& aFFs=pIP->SSInterferences();
  const Handle(XIntTools_Context)& aCtx= pPF->Context();
  //
  //
  //mySameDomainShapes.Clear();
  //
  // 1. For each FF find among images of faces
  //    all pairs of same domain faces (SDF) [=> aLCS]
  aNbFF=aFFs.Extent();
  for (i=1; i<=aNbFF; ++i) {
    XBOPTools_SSInterference& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
    const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));
    //
    // if there are no in/on 2D split parts the faces nF1, nF2
    // can not be SDF
    const XBOPTools_ListOfPaveBlock& aLPBInOn=aFF.PaveBlocks();
    aNbPBInOn=aLPBInOn.Extent();
    //
    //===
    const TColStd_ListOfInteger& aLSE=aFF.SharedEdges();
    aNbSE=aLSE.Extent();
    if (!aNbPBInOn && !aNbSE) {
      continue;
    }
    //===
    //
    // if there is at least one section edge between faces nF1, nF2
    // they can not be SDF
    XBOPTools_SequenceOfCurves& aSC=aFF.Curves();
    aNbC=aSC.Length();
    if (aNbC) {
      continue;
    }
    //
    // the faces are suspected to be SDF.
    // Try to find SDF among images of nF1, nF2
    aMFence.Clear();
    //
    //--------------------------------------------------------
    bHasImage1=mySplitFaces.HasImage(aF1);
    bHasImage2=mySplitFaces.HasImage(aF2);
    //
    aLX1.Clear();
    if (!bHasImage1) {
      aLX1.Append(aF1);
    }
    //
    aLX2.Clear();
    if (!bHasImage2) {
      aLX2.Append(aF2);
    }
    //
    const TopTools_ListOfShape& aLF1r=(bHasImage1)? mySplitFaces.Image(aF1) : aLX1;
    const TopTools_ListOfShape& aLF2r=(bHasImage2)? mySplitFaces.Image(aF2) : aLX2;
    //
    TopTools_DataMapOfIntegerShape aMIS;
    TColStd_ListIteratorOfListOfInteger aItLI;
    NMTDS_BoxBndTreeSelector aSelector;
    NMTDS_BoxBndTree aBBTree;
    NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
    //
    aNbF1=aLF1r.Extent();
    aNbF2=aLF2r.Extent();
    bForward=(aNbF1<aNbF2);
    //
    const TopTools_ListOfShape& aLF1=bForward ? aLF1r : aLF2r;
    const TopTools_ListOfShape& aLF2=bForward ? aLF2r : aLF1r;
    //
    // 1. aTreeFiller
    aItF2.Initialize(aLF2);
    for (i2s=1; aItF2.More(); aItF2.Next(), ++i2s) {
      Bnd_Box aBoxF2s;
      //
      const TopoDS_Face& aF2s=*((TopoDS_Face*)(&aItF2.Value()));
      //
      BRepBndLib::Add(aF2s, aBoxF2s);
      //
      aMIS.Bind(i2s, aF2s);
      //
      aTreeFiller.Add(i2s, aBoxF2s);
    }//for (i2s=1; aItF2.More(); aItF2.Next(), ++i2s) {
    //
    aTreeFiller.Fill();
    //
    // 2.
    aItF1.Initialize(aLF1);
    for (j=1; aItF1.More(); aItF1.Next(), ++j) {
      Bnd_Box aBoxF1x;
      //
      const TopoDS_Face& aF1x=*((TopoDS_Face*)(&aItF1.Value()));
      //
      BRepBndLib::Add(aF1x, aBoxF1x);
      //
      aSelector.Clear();
      aSelector.SetBox(aBoxF1x);
      aNbSD=aBBTree.Select(aSelector);
      if (!aNbSD) {
        continue;
      }
      //
      const TColStd_ListOfInteger& aLI=aSelector.Indices();
      aItLI.Initialize(aLI);
      for (; aItLI.More(); aItLI.Next()) {
        i2s=aItLI.Value();
        const TopoDS_Face& aF2y=*((TopoDS_Face*)(&aMIS.Find(i2s)));
        //
        bIsSDF=NMTTools_Tools::AreFacesSameDomain(aF1x, aF2y, aCtx);
        if (bIsSDF) {
          if (aMFence.Contains(aF1x) || aMFence.Contains(aF2y)) {
            continue;
          }
          aMFence.Add(aF1x);
          aMFence.Add(aF2y);
          //
          NMTTools_CoupleOfShape aCS;
          //
          aCS.SetShape1(aF1x);
          aCS.SetShape2(aF2y);
          aLCS.Append(aCS);
          //
          if (bForward) {
            if (aF1x==aF1) {
              if (!mySplitFaces.HasImage(aF1)) {
                mySplitFaces.Bind(aF1, aF1);
              }
            }
            if (aF2y==aF2) {
              if (!mySplitFaces.HasImage(aF2)) {
                mySplitFaces.Bind(aF2, aF2);
              }
            }
          }
          else {
            if (aF1x==aF2) {
              if (!mySplitFaces.HasImage(aF2)) {
                mySplitFaces.Bind(aF2, aF2);
              }
            }
            if (aF2y==aF1) {
              if (!mySplitFaces.HasImage(aF1)) {
                mySplitFaces.Bind(aF1, aF1);
              }
            }
          }
          //
          break;
        }//if (bIsSDF) {
      }//for (; aItLI.More(); aItLI.Next()) {
    }//for (; aItF1.More(); aItF1.Next()) {
  }//for (i=1; i<=aNbFF; ++i)
  //-------------------------------------------------------------
  aNbC=aLCS.Extent();
  if (!aNbC) {
    return;
  }
  //
  // 2. Find Chains
  NMTTools_IndexedDataMapOfShapeIndexedMapOfShape aMC;
  //
  NMTTools_Tools::FindChains(aLCS, aMC);
  //
  Standard_Boolean bIsImage;
  Standard_Integer aIx, aIxMin, aNbMSDF, k, aNbMFj;
  TopoDS_Shape aFOld, aFSDmin;
  TopTools_IndexedMapOfShape aMFj;
  TopTools_DataMapOfShapeInteger aDMSI;
  //
  aItF1.Initialize(myShapes);
  for (j=1; aItF1.More(); aItF1.Next(), ++j) {
    const TopoDS_Shape& aSj=aItF1.Value();
    aMFj.Clear();
    TopExp::MapShapes(aSj, TopAbs_FACE, aMFj);
    aNbMFj=aMFj.Extent();
    for (k=1; k<=aNbMFj; ++k) {
      const TopoDS_Shape& aFk=aMFj(k);
      if (!aDMSI.IsBound(aFk)) {
        aDMSI.Bind(aFk, j);
      }
    }
  }
  //
  // 3. Fill the map of SDF mySameDomainFaces
  aNbC=aMC.Extent();
  for (i=1; i<=aNbC; ++i) {
   // const TopoDS_Shape& aF=aMC.FindKey(i);
    const TopTools_IndexedMapOfShape& aMSDF=aMC(i);
    //
    aNbMSDF=aMSDF.Extent();
    for (j=1; j<=aNbMSDF; ++j) {
      const TopoDS_Shape& aFSD=aMSDF(j);
      bIsImage=mySplitFaces.IsImage(aFSD);
      aFOld=aFSD;
      if (bIsImage) {
        aFOld=mySplitFaces.ImageFrom(aFSD);
      }
      //
      aIx=aDMSI.Find(aFOld);
      if (j==1) {
        aIxMin=aIx;
        aFSDmin=aFSD;
        continue;
      }
      else {
        if (aIx<aIxMin) {
          aIxMin=aIx;
          aFSDmin=aFSD;
        }
      }
    }
    //
    for (j=1; j<=aNbMSDF; ++j) {
      const TopoDS_Shape& aFSD=aMSDF(j);
      mySameDomainShapes.Add(aFSD, aFSDmin);
    }
  }
  //
}

//=======================================================================
// function: FillImagesFaces1
// purpose:
//=======================================================================
void GEOMAlgo_Builder::FillImagesFaces1()
{
  Standard_Integer i, aNb, iSense, aNbLFx;
  TopoDS_Face aF, aFSp, aFSD;
  TopTools_ListOfShape aLFx;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  const NMTDS_ShapesDataStructure& aDS=*myPaveFiller->DS();
  //
  aNb=aDS.NumberOfShapesOfTheObject();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aS=aDS.Shape(i);
    if (aS.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    //
    if (!mySplitFaces.HasImage(aS)) {
      continue;
    }
    //
    aF=*((TopoDS_Face*)&aS);
    //
    aLFx.Clear();
    const TopTools_ListOfShape& aLF=mySplitFaces.Image(aF);
    aIt.Initialize(aLF);
    for (; aIt.More(); aIt.Next()) {
      aFSp=*((TopoDS_Face*)(&aIt.Value()));
      if (!mySameDomainShapes.Contains(aFSp)) {
        aLFx.Append(aFSp);
      }
      else {
        const TopoDS_Shape& aSx=mySameDomainShapes.FindFromKey(aFSp);
        aFSD=*((TopoDS_Face*)(&aSx));
        iSense=GEOMAlgo_Tools3D::Sense(aFSp, aFSD);
        if (iSense<0) {
          aFSD.Reverse();
        }
        aLFx.Append(aFSD);
      }
    }
    //
    if (!myImages.HasImage(aF)) {
      aNbLFx=aLFx.Extent();
      if (aNbLFx==1) {
        const TopoDS_Shape& aFx=aLFx.First();
        if (aF.IsSame(aFx)) {
          continue;
        }
      }
      myImages.Bind(aF, aLFx);
    }
  }
}

//=======================================================================
// function: FillInternalVertices
// purpose:
//=======================================================================
void GEOMAlgo_Builder::FillInternalVertices()
{
  const NMTDS_ShapesDataStructure& aDS=*myPaveFiller->DS();
  NMTTools_PaveFiller* pPF=myPaveFiller;
  NMTDS_InterfPool* pIP=pPF->IP();
  const Handle(XIntTools_Context)& aCtx= pPF->Context();
  //
  XBOPTools_CArray1OfVSInterference& aVFs=pIP->VSInterferences();
  XBOPTools_CArray1OfESInterference& aEFs=pIP->ESInterferences();
  const NMTTools_IndexedDataMapOfIndexedMapOfInteger& aMAV=pPF->AloneVertices();
  //
  Standard_Boolean bHasImage;
  Standard_Integer i, j, nF, aNbS, nV, nVSD, n1, n2, iFlag;
  Standard_Integer aNbVFs, aNbAVF, aNbEFs, aNbVC, aNbE, aNbV;
  Standard_Real aU1, aU2, aTol;
  NMTTools_IndexedDataMapOfIndexedMapOfInteger aMFMV;
  TopTools_MapOfShape aMFence;
  TopTools_ListIteratorOfListOfShape aIt, aItV;
  BRep_Builder aBB;
  //
  // 1. Collect face-vertex candidates [aMFMV]
  //
  // 1.1. VFs
  aNbVFs=aVFs.Extent();
  for (i=1; i<=aNbVFs; ++i) {
    const XBOPTools_VSInterference& aVS=aVFs(i);
    aVS.Indices(n1, n2);
    nF=n2;
    nV=n1;
    if (aDS.Shape(n1).ShapeType()==TopAbs_FACE) {
      nF=n1;
      nV=n2;
    }
    nVSD=pPF->FindSDVertex(nV);
    if (nVSD) {
      nV=nVSD;
    }
    //
    UpdateCandidates(nF, nV, aMFMV);
  }
  //
  // 1.2 EFs
  aNbEFs=aEFs.Extent();
  for (i=1; i<=aNbEFs; ++i) {
    const XBOPTools_ESInterference& aEF=aEFs(i);
    aEF.Indices(n1, n2);
    nV=aEF.NewShape();
    if (!nV) {
      continue;
    }
    const TopoDS_Shape& aSnew=aDS.Shape(nV);
    if (aSnew.ShapeType()!=TopAbs_VERTEX) {
      continue;
    }
    //
    nF=(aDS.Shape(n1).ShapeType()==TopAbs_FACE) ? n1 : n2;
    nVSD=pPF->FindSDVertex(nV);
    if (nVSD) {
      nV=nVSD;
    }
    UpdateCandidates(nF, nV, aMFMV);
  }
  //
  aNbS=aDS.NumberOfShapesOfTheObject();
  for (nF=1; nF<=aNbS; ++nF) {
    const TopoDS_Shape& aF=aDS.Shape(nF);
    //
    if (aF.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    if (!aMFence.Add(aF)) {
      continue;
    }
    //
    const TopoDS_Face& aFF=TopoDS::Face(aF);
    aTol=BRep_Tool::Tolerance(aFF);
    //
    // 1.3 FFs
    if (aMAV.Contains(nF)) {
      const TColStd_IndexedMapOfInteger& aMAVF=aMAV.FindFromKey(nF);
      aNbAVF=aMAVF.Extent();
      for (j=1; j<=aNbAVF; ++j) {
        nV=aMAVF(j);
        nVSD=pPF->FindSDVertex(nV);
        if (nVSD) {
          nV=nVSD;
        }
        //
        UpdateCandidates(nF, nV, aMFMV);
      }
    }
    //
    // 1.4 Internal vertices of the face nF
    XBooleanOperations_OnceExplorer aExp(aDS);
    aExp.Init(nF, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      nV=aExp.Current();
      const TopoDS_Shape& aV=aDS.Shape(nV);
      if (aV.Orientation()==TopAbs_INTERNAL) {
        nVSD=pPF->FindSDVertex(nV);
        if (nVSD) {
          nV=nVSD;
        }
        //
        UpdateCandidates(nF, nV, aMFMV);
      }
    }
    //
    // 2. Process face nF
    if (!aMFMV.Contains(nF)) {
      continue;
    }
    //
    const TColStd_IndexedMapOfInteger& aMVC=aMFMV.FindFromKey(nF);
    aNbVC=aMVC.Extent();
    if (!aNbVC) {
      continue;
    }
    //
    // 2.1 Refine candidates
    TopTools_IndexedDataMapOfShapeListOfShape aMVE;
    TopTools_ListOfShape aLV;
    //
    bHasImage=myImages.HasImage(aF);
    if (bHasImage) {
      const TopTools_ListOfShape& aLFx=myImages.Image(aF);
      aIt.Initialize(aLFx);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aFx=aIt.Value();
        TopExp::MapShapesAndAncestors(aFx, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
      }
    }
    else {
      Standard_Boolean bFaceToProcess;
      //
      TopExp::MapShapesAndAncestors(aF, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
      bFaceToProcess=Standard_False;
      for (j=1; j<=aNbVC; ++j) {
        nV=aMVC(j);
        const TopoDS_Shape& aV=aDS.Shape(nV);
        if (!aMVE.Contains(aV)) {
          bFaceToProcess=!bFaceToProcess;
          break;
        }
      }
      if (!bFaceToProcess) {
        continue;
      }
    }// else
    //
    for (j=1; j<=aNbVC; ++j) {
      nV=aMVC(j);
      const TopoDS_Shape& aV=aDS.Shape(nV);
      if (aMVE.Contains(aV)) {
        const TopTools_ListOfShape& aLE=aMVE.FindFromKey(aV);
        aNbE=aLE.Extent();
        if (aNbE) {
          continue;
        }
      }
      aLV.Append(aV);
    }
    //
    aNbV=aLV.Extent();
    if (aNbV) {
      //  3. Try to put vertices into the face(s)
      aItV.Initialize(aLV);
      for (; aItV.More(); aItV.Next()) {
        TopoDS_Vertex aV=TopoDS::Vertex(aItV.Value());
        aV.Orientation(TopAbs_INTERNAL);
        //
        bHasImage=myImages.HasImage(aF);
        if (bHasImage) {
          const TopTools_ListOfShape& aLFx=myImages.Image(aF);
          aIt.Initialize(aLFx);
          for (; aIt.More(); aIt.Next()) {
            TopoDS_Face aFx=TopoDS::Face(aIt.Value());
            // update classifier
            XIntTools_FClass2d& aClsf=aCtx->FClass2d(aFx);
            aClsf.Init(aFx, aTol);
            //
            iFlag=aCtx->ComputeVS (aV, aFx, aU1, aU2);
            if (!iFlag) {
              aBB.Add(aFx, aV);
              break;
            }
          }
        }
        else {
          const TopoDS_Face& aFx=TopoDS::Face(aF);
          // update classifier
          XIntTools_FClass2d& aClsf=aCtx->FClass2d(aFx);
          aClsf.Init(aFx, aTol);
          //
          iFlag=aCtx->ComputeVS (aV, aFx, aU1, aU2);
          if (!iFlag) {
            TopoDS_Face aFz;
            //
            GEOMAlgo_Tools3D::CopyFace(aFx, aFz);
            aBB.Add(aFz, aV);
            myImages.Bind(aF, aFz);
          }
        }
      }// for (; aItV.More(); aItV.Next()) {
    }// if (aNbV) {
  }// for (nF=1; nF<=aNb; ++nF) {
}

//=======================================================================
// function: UpdateCandidates
// purpose:
//=======================================================================
void UpdateCandidates(const Standard_Integer theNF,
                      const Standard_Integer theNV,
                       NMTTools_IndexedDataMapOfIndexedMapOfInteger& theMFMV)
{
  if (theMFMV.Contains(theNF)) {
    TColStd_IndexedMapOfInteger& aMV=theMFMV.ChangeFromKey(theNF);
    aMV.Add(theNV);
  }
  else {
    TColStd_IndexedMapOfInteger aMV;
    aMV.Add(theNV);
    theMFMV.Add(theNF, aMV);
  }
}

//=======================================================================
//function : IsClosed
//purpose  :
//=======================================================================
Standard_Boolean IsClosed(const TopoDS_Edge& aE,
                          const TopoDS_Face& aF,
                          Standard_Boolean& bFlag)
{
  Standard_Boolean bRet;
  //
  bRet=BRep_Tool::IsClosed(aE, aF);
  bFlag=bRet;
  if (bRet) {
    Standard_Integer iCnt;
    TopoDS_Shape aE1;
    //
    bRet=!bRet;
    iCnt=0;
    TopExp_Explorer aExp(aF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aEx=aExp.Current();
      //
      if (aEx.IsSame(aE)) {
        ++iCnt;
        if (iCnt==1) {
          aE1=aEx;
        }
        else if (iCnt==2){
          aE1.Reverse();
          bRet=(aE1==aEx);
          break;
        }
      }
    }
  }
  return bRet;
}

/*
    {
      TopoDS_Compound aCx;
      BRep_Builder aBBx;
      TopTools_ListIteratorOfListOfShape aItx;
      //
      aBBx.MakeCompound(aCx);
      aBBx.Add(aCx, aFF);
      aItx.Initialize(aSE);
      for (; aItx.More(); aItx.Next()) {
        TopoDS_Shape& aEx=aItx.Value();
        aBBx.Add(aCx, aEx);
      }
      int a=0;
    }
    */
