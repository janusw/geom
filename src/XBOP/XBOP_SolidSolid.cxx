// Created on: 2001-05-25
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



#include <XBOP_SolidSolid.ixx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>


#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <TopExp.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <XBooleanOperations_ShapesDataStructure.hxx>
#include <XBooleanOperations_StateOfShape.hxx>
#include <XBooleanOperations_IndexedDataMapOfShapeInteger.hxx>

#include <XBOPTColStd_Dump.hxx>
#include <XBOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>

#include <XBOPTools_SSInterference.hxx>
#include <XBOPTools_InterferencePool.hxx>
#include <XBOPTools_CArray1OfSSInterference.hxx>
#include <XBOPTools_SequenceOfCurves.hxx>
#include <XBOPTools_ListOfPaveBlock.hxx>
#include <XBOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <XBOPTools_PaveBlock.hxx>
#include <XBOPTools_PaveFiller.hxx>
#include <XBOPTools_DSFiller.hxx>
#include <XBOPTools_CommonBlockPool.hxx>
#include <XBOPTools_ListOfCommonBlock.hxx>
#include <XBOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <XBOPTools_CommonBlock.hxx>
#include <XBOPTools_PaveBlock.hxx>

#include <XBOP_WireEdgeSet.hxx>
#include <XBOP_ShellFaceSet.hxx>
#include <XBOP_SolidBuilder.hxx>
#include <XBOP_Draw.hxx>
#include <XBOP_FaceBuilder.hxx>
#include <XBOP_CorrectTolerances.hxx>
#include <XBOP_BuilderTools.hxx>
#include <XBOP_Refiner.hxx>

#include <XBOPTools_Curve.hxx>
#include <XBOPTools_Tools3D.hxx>
#include <XBOPTools_Tools2D.hxx>
#include <XIntTools_Context.hxx>
#include <BRepTools.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pln.hxx>
#include <XBOP_SDFWESFiller.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <XBOP_SolidSolidHistoryCollector.hxx>

static Standard_Boolean CheckFaceIntersection(const Standard_Integer   theFaceIndex,
					      const XBOPTools_DSFiller* theDSFiller);

static Standard_Boolean BuildWESForCommonZone(const Standard_Integer                           theFaceIndex,
					      const TColStd_IndexedMapOfInteger&               theFFIndicesMap,
					      const XBOPTools_DSFiller*                         theDSFiller,
					      const XBOP_Operation&                             theOperation,
					      XBOP_WireEdgeSet&                                 theWES,
					      XBOP_WireEdgeSet&                                 theWESAvoid);

static void LocalAddPartsEFSo (const Standard_Integer nF1, 
			       const Standard_Integer iFF,
			       const TopTools_IndexedDataMapOfShapeListOfShape& aMEFObj,
			       const TopTools_IndexedDataMapOfShapeListOfShape& aMEFTool,
			       const XBOPTools_DSFiller*                         theDSFiller,
			       const XBOP_Operation&                             theOperation,
			       const TopTools_ListOfShape&                      theListOfSDFace,
			       TopTools_IndexedMapOfShape& anEMap,
			       XBOP_WireEdgeSet& aWES);

static Standard_Boolean LocalIsKeepTwice(const TopoDS_Face&          aF1,
					 const TopoDS_Face&          aF2,
					 const TopoDS_Face&          aF2Adj,
					 const TopoDS_Edge&          aSpEF2,
					 const TopTools_ListOfShape& theFacesToAvoid2,
					 const TopTools_ListOfShape& theFacesToAvoidAdj);

//=======================================================================
// function: XBOP_SolidSolid::XBOP_SolidSolid
// purpose: 
//=======================================================================
  XBOP_SolidSolid::XBOP_SolidSolid()
{
}
//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
  void XBOP_SolidSolid::Destroy() {}

//=======================================================================
// function: DoDoWithFiller
// purpose: 
//=======================================================================
  void XBOP_SolidSolid::DoWithFiller(const XBOPTools_DSFiller& aDSFiller) 
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  //
  myResultMap.Clear();
  myModifiedMap.Clear();
  //
  myDSFiller=(XBOPTools_DSFiller*) &aDSFiller;
  //
  try {
    OCC_CATCH_SIGNALS

    if(!myDSFiller->IsDone()) {
      myErrorStatus = 1;
      XBOPTColStd_Dump::PrintMessage("DSFiller is invalid: Can not build result\n");
      return;
    }
    //
    PrepareReferences();
    //
    Standard_Boolean bIsNewFiller;
    bIsNewFiller=aDSFiller.IsNewFiller();
    
    if (bIsNewFiller) {
      Prepare();
      
      PrepareFaceSplits();

      aDSFiller.SetNewFiller(!bIsNewFiller);
    }
    //
    DoNewFaces();
    //
    BuildResult();
    //
    // Treat of internals
    CollectInternals();
    XBOP_Refiner aRefiner;
    aRefiner.SetShape(myResult);
    aRefiner.SetInternals(myInternals);
    aRefiner.Do();
    //
    XBOP_CorrectTolerances::CorrectTolerances(myResult, 0.01);
    //
    FillModified();

    if(!myHistory.IsNull()) {
      Handle(XBOP_SolidSolidHistoryCollector) aSolidHistory = 
	Handle(XBOP_SolidSolidHistoryCollector)::DownCast(myHistory);
      aSolidHistory->SetResult(myResult, myDSFiller);
    }

    myIsDone=Standard_True;
  }
  catch ( Standard_Failure ) {
    myErrorStatus = 1;
    XBOPTColStd_Dump::PrintMessage("Can not build result\n");
  }
}
//=================================================================================
// function: PrepareReferences
// purpose: 
//=================================================================================
void XBOP_SolidSolid::PrepareReferences() 
{
  Standard_Integer i;
  const XBooleanOperations_ShapesDataStructure& aDS = myDSFiller->DS();
  //
  for (i=0; i<2; ++i) {
    const TopoDS_Shape& aS=(!i)? aDS.Object() : aDS.Tool();
    //
    TopoDS_Solid aRefSolid;
    //
    if(aS.ShapeType() == TopAbs_SOLID) {
      aRefSolid = TopoDS::Solid(aS);
    }
    else {
      BRep_Builder aBB;
      TopExp_Explorer aExp;
      //
      aBB.MakeSolid(aRefSolid);
      //
      aExp.Init(aS, TopAbs_SHELL);
      for(; aExp.More(); aExp.Next()) {
	const TopoDS_Shape& aShell = aExp.Current();
	aBB.Add(aRefSolid, aShell);
      }
    }
    if(!i) {
      myRefObject=aRefSolid;
    }
    else {
      myRefTool=aRefSolid;
    }
  }
}
//=================================================================================
// function: BuildResult
// purpose: 
//=================================================================================
  void XBOP_SolidSolid::BuildResult() 
{
  XBOP_ShellFaceSet theSFS(TopoDS::Solid(myShape1));

  PrepareSFS(myNewFaces, theSFS);
  //
  XBOP_SolidBuilder aSOBU;
  
  BRep_Builder aBB;
  TopoDS_Solid aNewSolid;
  TopoDS_Shell aNewShell;
  TopTools_ListOfShape aListOfResult;

  aSOBU.InitSolidBuilder(theSFS, Standard_True);
  for(aSOBU.InitSolid(); aSOBU.MoreSolid(); aSOBU.NextSolid()) {
    aBB.MakeSolid(aNewSolid);

    for(aSOBU.InitShell(); aSOBU.MoreShell(); aSOBU.NextShell()) {

      if(aSOBU.IsOldShell()) {
	aNewShell = aSOBU.OldShell();
      }
      else {
	aBB.MakeShell(aNewShell);

	for(aSOBU.InitFace(); aSOBU.MoreFace(); aSOBU.NextFace()) {
	  TopoDS_Face aFace = aSOBU.Face();
	  aBB.Add(aNewShell, aFace);
	}

	Standard_Boolean isclosed = Standard_False;
	TopTools_IndexedDataMapOfShapeListOfShape aMap;
	TopExp::MapShapesAndAncestors(aNewShell, TopAbs_EDGE, TopAbs_FACE, aMap);
	Standard_Integer nbedge = aMap.Extent();

	for(Standard_Integer eiterator = 1; eiterator <= nbedge; eiterator++) {
	  const TopoDS_Shape& aShape = aMap.FindKey(eiterator);
	  TopAbs_Orientation anOrientation = aShape.Orientation();

	  if((anOrientation == TopAbs_INTERNAL) ||
	     (anOrientation == TopAbs_EXTERNAL) ||
	     (BRep_Tool::Degenerated(TopoDS::Edge(aShape))))
	    continue;
	  Standard_Integer nbface = aMap(eiterator).Extent();

	  if(nbface < 2) { // (here should be additional condition)
	    isclosed = Standard_False;
	    break;
	  }
	}
	aNewShell.Closed(isclosed);
      } // end else of if(aSOBU.IsOldShell())
      aBB.Add(aNewSolid, aNewShell);
    }
    aListOfResult.Append(aNewSolid);
  }

  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  
  TopTools_ListIteratorOfListOfShape aResultIt(aListOfResult);
  
  for(; aResultIt.More(); aResultIt.Next()) {
    aBB.Add(aCompound, aResultIt.Value());
  }
  
  myResult=aCompound;
}

//=======================================================================
// function: DoNewFaces
// purpose: 
//=======================================================================
  void XBOP_SolidSolid::DoNewFaces() 
{
  const XBooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  XBOPTools_InterferencePool* pIntrPool=(XBOPTools_InterferencePool*)&myDSFiller->InterfPool();
  XBOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  //
  // EF Maps
  const TopoDS_Shape& anObj=aDS.Object();
  const TopoDS_Shape& aTool=aDS.Tool();
  
  TopTools_IndexedMapOfShape anEMap;
  TopTools_IndexedDataMapOfShapeListOfShape aMEFObj, aMEFTool;
  TopExp::MapShapesAndAncestors (anObj, TopAbs_EDGE , TopAbs_FACE , aMEFObj);
  TopExp::MapShapesAndAncestors (aTool, TopAbs_EDGE , TopAbs_FACE , aMEFTool);
  //
  Standard_Boolean bIsTouchCase, bIsTouch, bToReverseFace;    
  Standard_Integer i, aNb, j, aNbj, iFF, nF1, iRank;
  TopTools_ListOfShape aListOfNewFaces;
  TopAbs_Orientation anOriF1;
  TopTools_ListIteratorOfListOfShape anIt;

  const TColStd_DataMapOfIntegerListOfInteger& aMapOfFaceSplits = myDSFiller->SplitFacePool();

  //
  // DoMap
  XBOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aFFMap;
  XBOP_BuilderTools::DoMap(aFFs, aFFMap);
  //
  aNb=aFFMap.Extent();
  for (i=1; i<=aNb; i++) {
    // 
    // a. Prepare info about the Face nF1 and create WES for nF1
    nF1=aFFMap.FindKey(i);
    const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
    
    anOriF1=aF1.Orientation();
    iRank=aDS.Rank(nF1);

    if(aMapOfFaceSplits.IsBound(nF1)) {
      TopoDS_Face aFace = aF1;
      TopTools_ListOfShape aLF;

      const TColStd_ListOfInteger& aListOfSplitIndex = aMapOfFaceSplits.Find(nF1);
      TColStd_ListIteratorOfListOfInteger aSplIt(aListOfSplitIndex);

      for(; aSplIt.More(); aSplIt.Next()) {
	Standard_Integer nFSpl = aSplIt.Value();
	Standard_Boolean bAddFace = Standard_False;

	XBooleanOperations_StateOfShape aSt = aDS.GetState(nFSpl);

	if(aSt != XBooleanOperations_ON) {
	  XBooleanOperations_StateOfShape aStateComp = XBOP_BuilderTools::StateToCompare(iRank, myOperation);
	  
	  if(aStateComp == aSt) {
	    bAddFace = Standard_True;
	  }
	}
	else {
	   if(iRank == 1) {
	     bAddFace = TakeOnSplit(nFSpl, nF1);
	  }
	}

	if(bAddFace) {
 	  const TopoDS_Shape& aFaceSpl = aDS.Shape(nFSpl);
	  aLF.Append(aFaceSpl);
	}
      }

      TopTools_ListOfShape aLFx;
      anIt.Initialize(aLF);

      for (; anIt.More(); anIt.Next()) {
	TopoDS_Shape& aFx=anIt.Value();
	aFx.Orientation(anOriF1);
	bToReverseFace=XBOP_BuilderTools::ToReverseFace(iRank, myOperation);

	if (bToReverseFace) {
	  aFx.Reverse();
	}
	aListOfNewFaces.Append(aFx);
	aLFx.Append(aFx);

	if(!myHistory.IsNull()) {
	  Handle(XBOP_SolidSolidHistoryCollector) aSolidHistory = 
	    Handle(XBOP_SolidSolidHistoryCollector)::DownCast(myHistory);
	  aSolidHistory->AddNewShape(aFace, aFx, myDSFiller);
	}
      }
      //
      // Fill "Modified"
      FillModified(aFace, aLFx); 
      continue;
    }

    myFace=aF1;
    myFace.Orientation(TopAbs_FORWARD);
    XBOP_WireEdgeSet aWES (myFace);
    
    const TColStd_IndexedMapOfInteger& aFFIndicesMap=aFFMap.FindFromIndex(i);
    aNbj=aFFIndicesMap.Extent();
    // 
    // b. The Switch: Same Domain Faces or Non-Same Domain Faces 
    bIsTouchCase=Standard_False;
    for (j=1; j<=aNbj; j++) {
      iFF=aFFIndicesMap(j);
      XBOPTools_SSInterference& aFF=aFFs(iFF);
      bIsTouchCase=aFF.IsTangentFaces();
      if (bIsTouchCase) {
	break;
      }
    }

    Standard_Boolean bIsCommonalgo = CheckFaceIntersection(nF1, myDSFiller);
    XBOP_WireEdgeSet atmpWES (myFace);
    XBOP_WireEdgeSet atmpWESAvoid (myFace);

    if(!bIsCommonalgo) {
      bIsCommonalgo = !BuildWESForCommonZone(nF1, aFFIndicesMap, myDSFiller, 
					    myOperation, atmpWES, atmpWESAvoid);
    }

    if(bIsCommonalgo) {
      //
      // c. Filling the WES for nF1
      if (bIsTouchCase) { 
	//                                               XXXXXXXXXXXXXXXXXXXXXXXXXX
	//                                               X   nF1 has SD face(s)   X
	//                                               XXXXXXXXXXXXXXXXXXXXXXXXXX
	//
	// 1. Add Split Parts having states in accordance with operation
	AddSplitPartsINOUT (nF1, aWES);
	//
	// 2. Add Section Edges to the WES 
	for (j=1; j<=aNbj; j++) {
	  iFF=aFFIndicesMap(j);
	  XBOPTools_SSInterference& aFF=aFFs(iFF);
	  bIsTouch=aFF.IsTangentFaces();

	  if (!bIsTouch) {
	    AddSectionPartsSo(nF1, iFF, aWES);
	  }
	}
	//
	// 3. Add IN2D, ON2D Parts to the WES 
	for (j=1; j<=aNbj; j++) {
	  iFF = aFFIndicesMap(j);
	  XBOPTools_SSInterference& aFF=aFFs(iFF);
	  bIsTouch = aFF.IsTangentFaces();

	  if (bIsTouch) {
	    Standard_Integer nF2;
	    nF2 = aFF.OppositeIndex(nF1);
	    AddINON2DPartsSo(iFF, nF1, nF2, aWES);
	    AddPartsEFSDSo(nF1, iFF, aMEFObj, aMEFTool, aWES);
	    AddPartsEESDSo(nF1, iFF, aMEFObj, aMEFTool, aWES);
	  }
	}
	//
	// 4. On3D parts
	for (j=1; j<=aNbj; j++) {
	  iFF=aFFIndicesMap(j);
	  XBOPTools_SSInterference& aFF=aFFs(iFF);
	  bIsTouch=aFF.IsTangentFaces();

	  if (!bIsTouch) {
	    AddSplitPartsON3DSo (nF1, iFF, aWES);
	  }
	}

	// 5. Add EF parts (E (from F2) on F1 ),
	// where F2 is non-same-domain face to F1
	anEMap.Clear();
	//
	// anEMap will contain all Split parts that has already in aWES
	const TopTools_ListOfShape& aLE=aWES.StartElements();
	anIt.Initialize (aLE);

	for (; anIt.More(); anIt.Next()) {
	  TopoDS_Shape& anE=anIt.Value();
	  anEMap.Add(anE);
	}
	//
	for (j=1; j<=aNbj; j++) {
	  iFF = aFFIndicesMap(j);
	  XBOPTools_SSInterference& aFF=aFFs(iFF);
	  bIsTouch = aFF.IsTangentFaces();

	  if (!bIsTouch) {
	    AddPartsEFNonSDSo (nF1, iFF, aMEFObj, aMEFTool, aFFIndicesMap, anEMap, aWES);
	    AddPartsEENonSDSo (nF1, iFF, aMEFObj, aMEFTool, aFFIndicesMap, anEMap, aWES);
	  }
	}
      } // end of  if (bIsTouchCase)
      //
      else {
	//                                  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//                                  X   nF1 does not have SD face(s)   X
	//                                  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//
	// 1. Add Split Parts having states in accordance with operation
	AddSplitPartsINOUT (nF1, aWES);
	//
	// 2. Add Split Parts with state ON
	AddSplitPartsONSo (nF1, aMEFObj, aMEFTool, aWES);
	//
	// 3. Add Section Edges to the WES 
      
	for (j=1; j<=aNbj; j++) {
	  iFF=aFFIndicesMap(j);
	  AddSectionPartsSo(nF1, iFF, aWES);
	}
	//
	// 4. Add EF parts (E (from F2) on F1 )
	anEMap.Clear();
	for (j=1; j<=aNbj; j++) {
	  iFF=aFFIndicesMap(j);
	  AddPartsEFSo(nF1, iFF, aMEFObj, aMEFTool, anEMap, aWES);
	}
      }
    } // end of if(bIsCommonalgo)
    else {
      TopTools_IndexedMapOfOrientedShape aMapOfSDFaceEdges;
      XBOP_WireEdgeSet aSDWES(myFace);

      if (bIsTouchCase) {
	// Add IN2D, ON2D Parts to the WES 
	for (j=1; j<=aNbj; j++) {
	  iFF = aFFIndicesMap(j);
	  XBOPTools_SSInterference& aFF = aFFs(iFF);
	  bIsTouch = aFF.IsTangentFaces();

	  if (bIsTouch) {
	    Standard_Integer nF2 = aFF.OppositeIndex(nF1);
	    AddINON2DPartsSo(iFF, nF1, nF2, aSDWES);
	    AddPartsEFSDSo(nF1, iFF, aMEFObj, aMEFTool, aSDWES);
	    AddPartsEESDSo(nF1, iFF, aMEFObj, aMEFTool, aSDWES);
	  }
	}
	//
	// On3D parts
	for (j=1; j<=aNbj; j++) {
	  iFF = aFFIndicesMap(j);
	  XBOPTools_SSInterference& aFF = aFFs(iFF);
	  bIsTouch = aFF.IsTangentFaces();

	  if (!bIsTouch) {
	    AddSplitPartsON3DSo (nF1, iFF, aSDWES);
	  }
	}

	TopTools_IndexedMapOfOrientedShape aMapOfWESEdges;

	for(aSDWES.InitStartElements(); aSDWES.MoreStartElements(); aSDWES.NextStartElement()) {
	  aMapOfWESEdges.Add(aSDWES.StartElement());
	}

	// Build SameDomain faces.
	// Only edges from aMapOfWESEdges are represented in these faces
	TopTools_ListOfShape aListOfSDFaces;

	for (j = 1; j <= aNbj; j++) {
	  iFF = aFFIndicesMap(j);
	  XBOPTools_SSInterference& aFF = aFFs(iFF);
	  bIsTouch = aFF.IsTangentFaces();

	  if (bIsTouch) {

	    Standard_Integer nF2 = aFF.OppositeIndex(nF1);
	    TopoDS_Face aF1FWD = aF1;
	    aF1FWD.Orientation (TopAbs_FORWARD);
      
	    XBOP_WireEdgeSet aWEScommon (aF1FWD);
	    XBOP_SDFWESFiller aWESFiller(nF1, nF2, *myDSFiller);
	    aWESFiller.SetSenseFlag(aFF.SenseFlag());
	    aWESFiller.SetOperation(XBOP_COMMON);
	    aWESFiller.Do(aWEScommon);

	    XBOP_FaceBuilder aFB;
	    aFB.Do(aWEScommon);
	    const TopTools_ListOfShape& aLF = aFB.NewFaces();
	    TopTools_ListIteratorOfListOfShape anItLF(aLF);

	    for (; anItLF.More(); anItLF.Next()) {
	      const TopoDS_Shape& aFR = anItLF.Value();

	      if (aFR.ShapeType()==TopAbs_FACE) {
		const TopoDS_Face& aFaceResult=TopoDS::Face(aFR);
		//
		Standard_Boolean bIsValidIn2D  = Standard_False;
		Standard_Boolean bNegativeFlag = Standard_False;
		bIsValidIn2D = XBOPTools_Tools3D::IsValidArea (aFaceResult, bNegativeFlag);

		if (bIsValidIn2D) {

		  // 		if(CheckSameDomainFaceInside(aFaceResult, aF2)) {
		  Standard_Boolean bfound = Standard_True;
		  TopExp_Explorer anExp(aFaceResult, TopAbs_EDGE);

		  for(; anExp.More(); anExp.Next()) {
		    if(!aMapOfWESEdges.Contains(anExp.Current())) {
		      bfound = Standard_False;
		      break;
		    }
		  }

		  if(bfound)
		    aListOfSDFaces.Append(aFaceResult);
		  // 		}
		}
	      }
	    }
	  }
	}
	// end 	for (j = 1; j <= aNbj...

	TopTools_ListIteratorOfListOfShape anItSD(aListOfSDFaces);

	for(; anItSD.More(); anItSD.Next()) {
	  const TopoDS_Shape& aShape = anItSD.Value();
	  TopoDS_Face aFx = TopoDS::Face(aShape);
	  TopExp_Explorer anExp(aFx, TopAbs_EDGE);

	  for(; anExp.More(); anExp.Next()) {
	    aMapOfSDFaceEdges.Add(anExp.Current());
	  }
	  aFx.Orientation(anOriF1);
	  bToReverseFace=XBOP_BuilderTools::ToReverseFace(iRank, myOperation);

	  if (bToReverseFace) {
	    aFx.Reverse();
	  }
	  aListOfNewFaces.Append(aFx);
	}
      }

      // Build Common Zone faces, based on intersection lines and
      // splits with state ON (see BuildWESForCommonZone()).
      TopTools_ListOfShape aListOfCommonZoneFace;
      Standard_Integer awesit = 0;

      for(awesit = 0; awesit < 2; awesit++) {
	XBOP_WireEdgeSet& aCurWEStmp = (awesit == 0) ? atmpWES : atmpWESAvoid;
	XBOP_WireEdgeSet aCurWES(myFace);

	for(aCurWEStmp.InitStartElements(); aCurWEStmp.MoreStartElements(); aCurWEStmp.NextStartElement()) {
	  if(!aMapOfSDFaceEdges.Contains(aCurWEStmp.StartElement())) {
	    aCurWES.AddStartElement(aCurWEStmp.StartElement());
	  }
	}

	if(!aCurWES.StartElements().IsEmpty()) {
	  XBOP_FaceBuilder aFB;
    
	  aFB.SetTreatment(0); // 0 -Do internal edges, 1 -No Internal Edges
	  aFB.SetTreatSDScales(1);
	  aFB.Do(aCurWES);
	  const TopTools_ListOfShape& aLF = aFB.NewFaces();
	  anIt.Initialize(aLF);

	  for (; anIt.More(); anIt.Next()) {
	    TopoDS_Shape& aFx=anIt.Value();
	    aFx.Orientation(anOriF1);

	    if(awesit == 0) {
	      bToReverseFace=XBOP_BuilderTools::ToReverseFace(iRank, myOperation);

	      if (bToReverseFace) {
		aFx.Reverse();
	      }
	      aListOfNewFaces.Append(aFx);
	    }
	    aListOfCommonZoneFace.Append(aFx);
	  }
	}
      }
      // end for(awesit = ...

      // Construct WES to build faces out of common zone
      XBOP_WireEdgeSet aWES2 (myFace);
      AddSplitPartsINOUT (nF1, aWES2);
      //

      if(!bIsTouchCase) {
	AddSplitPartsONSo (nF1, aMEFObj, aMEFTool, aWES2);
      }
      //
      for (j=1; j<=aNbj; j++) {
	iFF=aFFIndicesMap(j);
	AddSectionPartsSo(nF1, iFF, aWES2);
      }
      anEMap.Clear();

      if(bIsTouchCase) {
      	for(aSDWES.InitStartElements(); aSDWES.MoreStartElements(); aSDWES.NextStartElement()) {
	  aWES2.AddStartElement(aSDWES.StartElement());
	}

	for(aWES2.InitStartElements(); aWES2.MoreStartElements(); aWES2.NextStartElement()) {
	  anEMap.Add(aWES2.StartElement());
	}
      }

      for (j=1; j<=aNbj; j++) {
	iFF = aFFIndicesMap(j);

	if(!bIsTouchCase) {
	  LocalAddPartsEFSo(nF1, iFF, aMEFObj, aMEFTool, myDSFiller, 
			    myOperation, aListOfCommonZoneFace, anEMap, aWES2);
	}
	else {
	  XBOPTools_SSInterference& aFF = aFFs(iFF);
	  bIsTouch = aFF.IsTangentFaces();

	  if (!bIsTouch) {
	    AddPartsEFNonSDSo (nF1, iFF, aMEFObj, aMEFTool, aFFIndicesMap, anEMap, aWES2);
	    AddPartsEENonSDSo (nF1, iFF, aMEFObj, aMEFTool, aFFIndicesMap, anEMap, aWES2);
	  }
	}
      }

      aWES.ClearContents();

      aWES2.InitStartElements();

      for (; aWES2.MoreStartElements(); aWES2.NextStartElement()) {
	if(aMapOfSDFaceEdges.Contains(aWES2.StartElement()))
	  continue;
	Standard_Boolean bisequal = Standard_False;
	Standard_Integer wesit = 0;

	for(wesit = 0; wesit < 2; wesit++) {
	  XBOP_WireEdgeSet& acurwes = (wesit == 0) ? atmpWES : atmpWESAvoid;
	  acurwes.InitStartElements();

	  for (; !bisequal && acurwes.MoreStartElements(); acurwes.NextStartElement()) {
	    const TopoDS_Shape& anE = acurwes.StartElement();
	    bisequal = anE.IsEqual(aWES2.StartElement());
	  }
	}

	if(!bisequal) {
	  aWES.AddStartElement(aWES2.StartElement());
	}
      }
      //end for (; aWES2.MoreStartElements...
    }
    
    //
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Display the WES
    if (myDraw)  {
      const TopTools_ListOfShape& aWESL=aWES.StartElements();
      XBOP_Draw::DrawListOfEdgesWithPC (myFace, aWESL, i, "ew_"); 
      XBOP_Draw::Wait();
    }
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //
    // d. Build new Faces from myFace
    XBOP_FaceBuilder aFB;
    
    aFB.SetTreatment(0); // 0 -Do internal edges, 1 -No Internal Edges
    aFB.SetTreatSDScales(1);
    aFB.Do(aWES);

    const TopTools_ListOfShape& aLF=aFB.NewFaces();
    //
    // e. Do Internal Vertices
    DoInternalVertices(nF1, aLF);
    // 
    // f. Orient new faces
    TopTools_ListOfShape aLFx;
    anIt.Initialize(aLF);
    for (; anIt.More(); anIt.Next()) {
      TopoDS_Shape& aFx=anIt.Value();
      aFx.Orientation(anOriF1);
      bToReverseFace=XBOP_BuilderTools::ToReverseFace(iRank, myOperation);
      if (bToReverseFace) {
	aFx.Reverse();
      }
      aListOfNewFaces.Append(aFx);
      aLFx.Append(aFx);

      if(!myHistory.IsNull()) {
	Handle(XBOP_SolidSolidHistoryCollector) aSolidHistory = 
	  Handle(XBOP_SolidSolidHistoryCollector)::DownCast(myHistory);
	aSolidHistory->AddNewShape(aF1, aFx, myDSFiller);
      }
    }
    //
    // Fill "Modified"
    FillModified(aF1, aLFx); 
    //
  }// for (i=1; i<=aNb; i++)

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Display the new Faces
  if (myDraw) { 
    XBOP_Draw::DrawListOfShape(aListOfNewFaces, "fn_");
  }
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  myNewFaces.Clear();
  myNewFaces.Append(aListOfNewFaces);
}

//=================================================================================
// function: PrepareSFS
// purpose: 
//=================================================================================
  void XBOP_SolidSolid::PrepareSFS(const TopTools_ListOfShape& theNewFaces,
				  XBOP_ShellFaceSet& theSFS) 
{
  
  const XBooleanOperations_ShapesDataStructure& theDS=myDSFiller->DS();

  Standard_Integer iRank, lastindex, firstindex, i, a;
  XBooleanOperations_StateOfShape aChoosenState;

  for(iRank = 1; iRank <= 2; iRank++) {
    aChoosenState = XBOP_BuilderTools::StateToCompare(iRank, myOperation);
    //
    const TopoDS_Shape& aSolid = (iRank==1) ? myShape1 : myShape2;
    
    firstindex=1;
    lastindex=theDS.NumberOfShapesOfTheObject();
    if (iRank==2) {
      firstindex=lastindex+1;
      lastindex=theDS.NumberOfSourceShapes();
    }
    
    for(i = firstindex; i <= lastindex; i++) {

      if((theDS.GetShapeType(i) != TopAbs_FACE) || (theDS.GetState(i) != aChoosenState))
	continue;

      
      // compute orientation
      TopoDS_Shape aFace=theDS.Shape(i);

      TopAbs_Orientation anOri = TopAbs_FORWARD;
      
      for(a = 1; a <= theDS.NumberOfAncestors(i); a++) {
	const TopoDS_Shape& aShell = theDS.GetShape(theDS.GetAncestor(i, a));

	TopAbs_Orientation anAncestorOrientation = aShell.Orientation();
	Standard_Boolean found = Standard_False;
	
	TopExp_Explorer aSolidExp(aSolid, TopAbs_SHELL);
	for(;  !found && aSolidExp.More(); aSolidExp.Next()) {
	  
	  if(aShell.IsSame(aSolidExp.Current())) {
	    anAncestorOrientation = aSolidExp.Current().Orientation();
	    found = Standard_True;
	  }
	}
	
	if(!found) {
	  // raise the exception: programming error
	  XBOPTColStd_Dump::PrintMessage
	    ("XBOP_SolidSolid::PrepareSFS(). Raises the exception: programming error...\n");
	  continue;
	}
	
	TopoDS_Shape anOrientedShell = aShell;
	anOrientedShell.Orientation(anAncestorOrientation);
	
	TopExp_Explorer aShellExp(anOrientedShell, TopAbs_FACE); 
	for(; aShellExp.More(); aShellExp.Next()) { 
	  
	  if(!aFace.IsSame(aShellExp.Current()))
	    continue;

	  anOri = aShellExp.Current().Orientation();
	  
	  if(XBOP_BuilderTools::ToReverseFace(iRank, myOperation)) {
	    anOri = TopAbs::Complement(anOri);
	  }
	  aFace.Orientation(anOri);
	  
	  theSFS.AddStartElement(aFace);
	} // end shell explorer
      }
    } // end source shapes explorer
  }

  TopTools_ListIteratorOfListOfShape anIt(theNewFaces);

  for(;anIt.More(); anIt.Next()) {
    theSFS.AddStartElement(anIt.Value());
  }
}

// --------------------------------------------------------------------------------------------------------
// static function: CheckFaceIntersection
// purpose: Returns Standard_True if there are no any common zone for given face,
//          Returns Standard_False if there are common zones or probability of existance of the common zone
//          is high.
// --------------------------------------------------------------------------------------------------------
Standard_Boolean CheckFaceIntersection(const Standard_Integer   theFaceIndex,
				       const XBOPTools_DSFiller* theDSFiller) {

  const XBooleanOperations_ShapesDataStructure& aDS              = theDSFiller->DS();
  const XBOPTools_PaveFiller&                   aPaveFiller      = theDSFiller->PaveFiller();
  XBOPTools_PaveFiller*                         pPaveFiller      = (XBOPTools_PaveFiller*)&aPaveFiller;
  XBOPTools_CommonBlockPool&                    aCBPool          = pPaveFiller->ChangeCommonBlockPool();
//   const XBOPTools_SplitShapesPool&              aSplitShapesPool = aPaveFiller.SplitShapesPool();

  XBOPTools_InterferencePool* pIntrPool=
    (XBOPTools_InterferencePool*)&theDSFiller->InterfPool();
  
  XBOPTools_CArray1OfSSInterference& aFFs = pIntrPool->SSInterferences();
  Standard_Integer i = 0;

  for(i = 1; i <= aFFs.Length(); i++) {
    XBOPTools_SSInterference& aFF = aFFs(i);

    if((aFF.Index1() != theFaceIndex) && (aFF.Index2() != theFaceIndex))
      continue;

    Standard_Integer otherindex = aFF.OppositeIndex(theFaceIndex);

    XBOPTools_SequenceOfCurves& aSC = aFF.Curves();
    Standard_Integer aNbCurves = aSC.Length();

    if(aNbCurves > 0) {
      TopoDS_Shape aF1 = aDS.Shape(theFaceIndex);
      TopoDS_Shape aF2 = aDS.Shape(otherindex);
      TopExp_Explorer anExp;

      for(Standard_Integer j = 0; j < 2; j++) {
	Standard_Integer afaceindextocompare = (j == 0) ? otherindex : theFaceIndex;
	Standard_Integer aFaceRank = 1;
	if(j == 0) {
	  anExp.Init(aF1, TopAbs_EDGE);
	  aFaceRank = aDS.Rank(theFaceIndex);
	}
	else {
	  anExp.Init(aF2, TopAbs_EDGE);
	  aFaceRank = aDS.Rank(otherindex);
	}

	for(; anExp.More(); anExp.Next()) {
	  const TopoDS_Edge& aEF = TopoDS::Edge(anExp.Current());
	  Standard_Integer nEF = aDS.ShapeIndex(aEF, aFaceRank);
	  XBOPTools_ListOfCommonBlock& aLCB = aCBPool(aDS.RefEdge(nEF));
	  XBOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);

	  for (; anItCB.More(); anItCB.Next()) {
	    XBOPTools_CommonBlock& aCB=anItCB.Value();
	    Standard_Integer nFace = aCB.Face();

	    if(nFace == afaceindextocompare)
	      return Standard_False;
	  }
	}
      }
    }
  }

  return Standard_True;
}

// ----------------------------------------------------------------------------------------------------------------
// static function: BuildWESForCommonZone
// purpose: Finds common zones which contains boundaries represented by new geometry
//          Fills theWES if zones could be included in result,
//          Fills theWESAvoid if zones could not be included in result.
//          Returns true if common zones are found, otherwise returns false.
// ----------------------------------------------------------------------------------------------------------------
Standard_Boolean BuildWESForCommonZone(const Standard_Integer                           theFaceIndex,
				       const TColStd_IndexedMapOfInteger&               theFFIndicesMap,
				       const XBOPTools_DSFiller*                         theDSFiller,
				       const XBOP_Operation&                             theOperation,
				       XBOP_WireEdgeSet&                                 theWES,
				       XBOP_WireEdgeSet&                                 theWESAvoid) {

  const XBooleanOperations_ShapesDataStructure& aDS              = theDSFiller->DS();
//   const XBOPTools_PaveFiller&                   aPaveFiller      = theDSFiller->PaveFiller();
//   XBOPTools_PaveFiller*                         pPaveFiller      = (XBOPTools_PaveFiller*)&aPaveFiller;
//   XBOPTools_CommonBlockPool&                    aCBPool          = pPaveFiller->ChangeCommonBlockPool();
//   const XBOPTools_SplitShapesPool&              aSplitShapesPool = aPaveFiller.SplitShapesPool();
  
  XBOPTools_InterferencePool* pIntrPool=
    (XBOPTools_InterferencePool*)&theDSFiller->InterfPool();
  
  XBOPTools_CArray1OfSSInterference& aFFs = pIntrPool->SSInterferences();

  TopoDS_Face aFace = TopoDS::Face(aDS.Shape(theFaceIndex));
  TopoDS_Face myFace = aFace;
  myFace.Orientation(TopAbs_FORWARD);
//   Standard_Integer iFaceRank = aDS.Rank(theFaceIndex);
  TopTools_IndexedDataMapOfShapeListOfShape aMapFCommonZones, aMapFCommonZones2;
  Standard_Integer j = 0, k = 0;

  for (j = 1; j <= theFFIndicesMap.Extent(); j++) {
    XBOP_WireEdgeSet atmpWES(myFace);

    Standard_Integer iFF= theFFIndicesMap(j);
    XBOPTools_SSInterference& aFF = aFFs(iFF);
    Standard_Integer aFaceIndex2 = aFF.OppositeIndex(theFaceIndex);
    TopoDS_Face aFace2 = TopoDS::Face(aDS.Shape(aFaceIndex2));
//     Standard_Integer iFaceRank2 = aDS.Rank(aFaceIndex2);

    XBOPTools_SequenceOfCurves& aSC = aFF.Curves();
    Standard_Integer aNbCurves = aSC.Length();
    Standard_Integer i = 0;

    for (i = 1; i <= aNbCurves; i++) {
      const XBOPTools_Curve& aBC = aSC(i);
      const XBOPTools_ListOfPaveBlock& aLPB = aBC.NewPaveBlocks();
      XBOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);

      for (; anIt.More(); anIt.Next()) {
	const XBOPTools_PaveBlock& aPB=anIt.Value();
	Standard_Integer nE = aPB.Edge();
	const TopoDS_Edge& aE = TopoDS::Edge(aDS.Shape(nE));

	TopoDS_Edge aES=aE;
	atmpWES.AddStartElement(aES);
	aES.Reverse();
	atmpWES.AddStartElement(aES);
      }
    }
    XBOP_SDFWESFiller aWESFiller(theFaceIndex, aFaceIndex2, *theDSFiller);
    Standard_Integer iSenseFlag = 0;
    Standard_Integer iRankF1 = aDS.Rank(theFaceIndex);

    // compute iSenseFlag. begin
    gp_Dir aDNF1, aDNF2;
    const XBOPTools_ListOfPaveBlock& aLPB = aFF.PaveBlocks();

    if (aLPB.IsEmpty()) {
      continue;
    }
    const XBOPTools_PaveBlock& aPB = aLPB.First();
    const TopoDS_Edge& aSpE = TopoDS::Edge(aDS.Shape(aPB.Edge()));
    XBOPTools_Tools3D::GetNormalToFaceOnEdge (aSpE, aFace, aDNF1); 
    XBOPTools_Tools3D::GetNormalToFaceOnEdge (aSpE, aFace2, aDNF2);
    Standard_Real aScPr = aDNF1*aDNF2;

    if (aScPr<0.) {
      iSenseFlag = -1;
    }
    else if (aScPr>0.) {
      iSenseFlag = 1;
    }
    // compute iSenseFlag. end

    aWESFiller.SetSenseFlag(iSenseFlag);
    aWESFiller.SetOperation(XBOP_COMMON);
    aWESFiller.Do(atmpWES);

    XBOP_FaceBuilder aFB;
    aFB.Do(atmpWES);
    const TopTools_ListOfShape& aLF = aFB.NewFaces();
    
    if(aLF.IsEmpty()) {
      continue;
    }

    TopTools_ListIteratorOfListOfShape anIt(aLF);
    
    for(; anIt.More(); anIt.Next()) {
      const TopoDS_Face& aCurFace = TopoDS::Face(anIt.Value());
      // check common zone.begin
      Standard_Boolean IsSameDomain = Standard_True;
      Standard_Boolean bIsValidIn2D = Standard_False, bNegativeFlag = Standard_False;
      bIsValidIn2D = XBOPTools_Tools3D::IsValidArea (aCurFace, bNegativeFlag);

      if(bIsValidIn2D) {
	Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aCurFace);
	Standard_Real aTolerance = BRep_Tool::Tolerance(aFace) + BRep_Tool::Tolerance(aFace2);
	XIntTools_Context aContext;
	Standard_Real umin, umax, vmin, vmax;
	BRepTools::UVBounds(aCurFace, umin, umax, vmin, vmax);
	Standard_Real deltau = (umax - umin) * 0.1;
	Standard_Real deltav = (vmax - vmin) * 0.1;

	for(Standard_Integer uit = 1; IsSameDomain && (uit < 9); uit++) {
	  Standard_Real U = umin + uit * deltau;

	  for(Standard_Integer vit = 1; vit < 9; vit++) {
	    Standard_Real V = vmin + vit * deltav;

	    if(aContext.IsPointInOnFace(aCurFace, gp_Pnt2d(U, V))) {
	      gp_Pnt aCurPoint = aSurface->Value(U, V);

	      if(!aContext.IsValidPointForFace(aCurPoint, aFace2, aTolerance)) {
		IsSameDomain = Standard_False;
		break;
	      }
	    }
	  }
	}
      }
      // check common zone.end

      if(IsSameDomain) {
	Standard_Integer addcommonzone = Standard_False;

	if (iSenseFlag==1) {
	  switch (theOperation) {
	  
	  case XBOP_FUSE: 
	    if (iRankF1==1) {
	      addcommonzone = Standard_True;
	    }
	    break;
	  
	  case XBOP_COMMON:
	    if (iRankF1==1) {
	      addcommonzone = Standard_True;
	    }
	    break;
      
	  case XBOP_CUT:   
	  case XBOP_CUT21: 
	    break;
	  
	  default:
	    break;
	  }
	}
	else if (iSenseFlag==-1) { // iSenseFlag<0
	  switch (theOperation) {
	  
	  case XBOP_FUSE:   
	  case XBOP_COMMON: 
	    break;
	  
	  case XBOP_CUT: 
	    if (iRankF1==1) {
	      addcommonzone = Standard_True;
	    }
	    break;

	  case XBOP_CUT21:
	    if (iRankF1==2) {
	      addcommonzone = Standard_True;
	    }
	  default:
	    break;
	  }
	}
        TopTools_ListOfShape thelist;
	if(addcommonzone) {
	  if(!aMapFCommonZones.Contains(aFace2))	    
	    aMapFCommonZones.Add(aFace2, thelist);
	  aMapFCommonZones.ChangeFromKey(aFace2).Append(aCurFace);
	}
	else {
	  if(!aMapFCommonZones2.Contains(aFace2))
	    aMapFCommonZones2.Add(aFace2, thelist);
	  aMapFCommonZones2.ChangeFromKey(aFace2).Append(aCurFace);
	}
      }
    }
  }

  if(aMapFCommonZones.IsEmpty() && aMapFCommonZones2.IsEmpty()) {
    return Standard_False;
  }
  Standard_Integer amapit = 0;

  for(amapit = 0; amapit < 2; amapit++) {
    const TopTools_IndexedDataMapOfShapeListOfShape& aMap = 
      (amapit == 0) ? aMapFCommonZones : aMapFCommonZones2;
    XBOP_WireEdgeSet& aWES = (amapit == 0) ? theWES : theWESAvoid;

    for(k = 1; k <= aMap.Extent(); k++) {
      const TopTools_ListOfShape& alf = aMap(k);
      TopTools_ListIteratorOfListOfShape anIt(alf);

      for(; anIt.More(); anIt.Next()) {
	TopExp_Explorer anExp(anIt.Value(), TopAbs_EDGE);

	for(; anExp.More(); anExp.Next()) {
	  aWES.AddStartElement(anExp.Current());
	}
      }
    }
  }
  //end for(amapit = ...

  return Standard_True;
}

// ----------------------------------------------------------------------------------------------------------------
// static function: LocalAddPartsEFSo
// purpose: Adds split parts of edges, which lay on face nF1, to theWES.
//          It uses found common zones (theListOfSDFace) to check
//          if a split part should be represented in theWES twice or not.
// ----------------------------------------------------------------------------------------------------------------
void LocalAddPartsEFSo (const Standard_Integer nF1, 
			const Standard_Integer iFF,
			const TopTools_IndexedDataMapOfShapeListOfShape& theMEFObj,
			const TopTools_IndexedDataMapOfShapeListOfShape& theMEFTool,
			const XBOPTools_DSFiller*                         theDSFiller,
			const XBOP_Operation&                             theOperation,
			const TopTools_ListOfShape&                      theListOfSDFace,
			TopTools_IndexedMapOfShape&                      theEMap,
			XBOP_WireEdgeSet&                                 theWES) {

  const XBooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  XBOPTools_InterferencePool* pIntrPool=(XBOPTools_InterferencePool*)&theDSFiller->InterfPool();
  XBOPTools_CArray1OfSSInterference& aFFs=pIntrPool->SSInterferences();
  
  const XBOPTools_PaveFiller& aPF    = theDSFiller->PaveFiller();
  XBOPTools_PaveFiller* pPaveFiller  =(XBOPTools_PaveFiller*)&aPF;
  XBOPTools_CommonBlockPool& aCBPool = pPaveFiller->ChangeCommonBlockPool();
  XBOPTools_SSInterference& aFF=aFFs(iFF);
  Standard_Integer nF2 = aFF.OppositeIndex(nF1);
  const TopoDS_Face& aFace  =TopoDS::Face(aDS.Shape(nF1));
  const TopoDS_Face& aFace2 =TopoDS::Face(aDS.Shape(nF2));

  Standard_Integer anindextocompare = nF1;
  Standard_Integer iFaceRank1, iFaceRank2;
  iFaceRank1 = aDS.Rank(nF1);
  iFaceRank2 = aDS.Rank(nF2);
  Standard_Integer aFaceRank = iFaceRank2;
  const TopTools_IndexedDataMapOfShapeListOfShape& aMEF = (aFaceRank == 1) ? theMEFObj : theMEFTool;
  TopExp_Explorer anExp(aFace2, TopAbs_EDGE);

  for(; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& aEF = TopoDS::Edge(anExp.Current());
    Standard_Integer nEF   = aDS.ShapeIndex(aEF, aFaceRank);
    XBOPTools_ListOfCommonBlock& aLCB = aCBPool(aDS.RefEdge(nEF));
    XBOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);

    for (; anItCB.More(); anItCB.Next()) {
      XBOPTools_CommonBlock& aCB=anItCB.Value();
      Standard_Integer nFace = aCB.Face();

      if (nFace == anindextocompare) {
	XBOPTools_PaveBlock& aPB = aCB.PaveBlock1(nEF);

	Standard_Integer nSplit = aPB.Edge();
	const TopoDS_Shape& aSplit = aDS.Shape(nSplit);

	if (theEMap.Contains(aSplit)) {
	  continue;// next CB
	}
	theEMap.Add(aSplit);
	TopoDS_Edge aSS = TopoDS::Edge(aSplit);
	//
	TopoDS_Face aFAdj;
	Standard_Boolean bIsAdjExists = Standard_False;

	bIsAdjExists = XBOPTools_Tools3D::GetAdjacentFace(aFace2, aEF, aMEF, aFAdj);
	Standard_Boolean bIsKeepTwice = Standard_False;

	if(bIsAdjExists) {
	  bIsKeepTwice = LocalIsKeepTwice(aFace, aFace2, aFAdj, aSS, theListOfSDFace, theListOfSDFace);
	}
	else {
	  bIsKeepTwice = XBOPTools_Tools3D::IsTouchCase(aSS, aFace, aFace2);
	}
	
	if(bIsKeepTwice) {
	  theWES.AddStartElement(aSS);
	  aSS.Reverse();
	  theWES.AddStartElement(aSS);
	}
	else {
	  aSS.Orientation(TopAbs_FORWARD);
	  TopoDS_Face myFace = aFace;
	  myFace.Orientation(TopAbs_FORWARD);
	  XBOP_BuilderTools::OrientSectionEdgeOnF1
	    (myFace, aFace2, iFaceRank1, theOperation, aSS);

	  theWES.AddStartElement(aSS);
	}
      }
    }
  }
}

// --------------------------------------------------------------------------------------------------------
// static function: LocalIsKeepTwice
// purpose:
// --------------------------------------------------------------------------------------------------------
Standard_Boolean LocalIsKeepTwice(const TopoDS_Face&          aF1,
				  const TopoDS_Face&          aF2,
				  const TopoDS_Face&          aF2Adj,
				  const TopoDS_Edge&          aSpEF2,
				  const TopTools_ListOfShape& theFacesToAvoid2,
				  const TopTools_ListOfShape& theFacesToAvoidAdj) {

  Standard_Real aT1, aT2, aT, dt=1.e-7, A, B, C, D, d2, d2Adj;
  gp_Dir aDNF1, aDNF2, DBF2, aDNF2Adj, DBF2Adj;
  gp_Vec aD1Sp;
  gp_Pnt aP, aPF2, aPF2Adj;

  Handle(Geom_Curve) aC3D=BRep_Tool::Curve(aSpEF2, aT1, aT2);
  aT=XBOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  XBOPTools_Tools3D::GetNormalToFaceOnEdge (aSpEF2, aF1, aT, aDNF1);
  
  //
  aC3D->D1(aT, aP, aD1Sp);
  gp_Dir aDD1Sp(aD1Sp); 
  
  if (aSpEF2.Orientation()==TopAbs_REVERSED) {
    aDD1Sp.Reverse();
  }
  // Split Normal on F2
  XBOPTools_Tools3D::GetNormalToFaceOnEdge (aSpEF2, aF2, aT, aDNF2);
  if (aF2.Orientation()==TopAbs_REVERSED) {
    aDNF2.Reverse();
  }
  // Binormal on F2
  DBF2=aDNF2^aDD1Sp;

  TopTools_ListIteratorOfListOfShape anIt;
  XIntTools_Context aContext;
  Standard_Boolean dtfound = Standard_False;
  Standard_Real acurdt = dt;

  while(!dtfound) {
    dtfound = Standard_True;

    aPF2.SetCoord(aP.X() + acurdt*DBF2.X(), 
		  aP.Y() + acurdt*DBF2.Y(),
		  aP.Z() + acurdt*DBF2.Z());
    
    for(anIt.Initialize(theFacesToAvoid2); anIt.More(); anIt.Next()) {
      const TopoDS_Face& aFace = TopoDS::Face(anIt.Value());

      if(aContext.IsValidPointForFace(aPF2, aFace, BRep_Tool::Tolerance(aFace))) {
	dtfound = Standard_False;
	break;
      }
    }

    if(!aContext.IsValidPointForFace(aPF2, aF2, BRep_Tool::Tolerance(aF2))) {
      acurdt = dt;
      break;
    }
    acurdt *= 2.;
  }

  if(!dtfound) {
    dt = 1.e-07;
    // Point near aP
    aPF2.SetCoord(aP.X()+dt*DBF2.X(), 
		  aP.Y()+dt*DBF2.Y(),
		  aP.Z()+dt*DBF2.Z());
  }
  //
  aDD1Sp.Reverse();

  // Split Normal on F2Adj
  XBOPTools_Tools3D::GetNormalToFaceOnEdge (aSpEF2, aF2Adj, aT, aDNF2Adj);
  if (aF2Adj.Orientation()==TopAbs_REVERSED) {
    aDNF2Adj.Reverse();
  }
  // Binormal on F2Adj
  DBF2Adj=aDNF2Adj^aDD1Sp;

  dt = 1.e-07;
  acurdt = dt;
  dtfound = Standard_False;

  while(!dtfound) {
    dtfound = Standard_True;
    aPF2Adj.SetCoord(aP.X() + acurdt*DBF2Adj.X(), 
		     aP.Y() + acurdt*DBF2Adj.Y(),
		     aP.Z() + acurdt*DBF2Adj.Z());
    
    for(anIt.Initialize(theFacesToAvoidAdj); anIt.More(); anIt.Next()) {
      const TopoDS_Face& aFace = TopoDS::Face(anIt.Value());

      if(aContext.IsValidPointForFace(aPF2Adj, aFace, BRep_Tool::Tolerance(aFace))) {
	dtfound = Standard_False;
	break;
      }
    }

    if(!aContext.IsValidPointForFace(aPF2Adj, aF2Adj, BRep_Tool::Tolerance(aF2Adj))) {
      acurdt = dt;
      break;
    }
    acurdt *= 2.;
  }

  if(!dtfound) {
    dt = 1.e-07;
    aPF2Adj.SetCoord(aP.X()+dt*DBF2Adj.X(), 
		     aP.Y()+dt*DBF2Adj.Y(),
		     aP.Z()+dt*DBF2Adj.Z());
  }
  //
  // Tangent Plane on F1
  gp_Pln aPlnN1(aP, aDNF1);
  aPlnN1.Coefficients(A, B, C, D);
  //
  d2   = A*aPF2.X()    + B*aPF2.Y()   + C*aPF2.Z()    + D;
  d2Adj= A*aPF2Adj.X() + B*aPF2Adj.Y()+ C*aPF2Adj.Z() + D;
  //
  if (fabs(d2)<1.e-10) {
    d2=0.;
  }
  if (fabs(d2Adj)<1.e-10) {
    d2Adj=0.;
  }
  //
  aT=d2*d2Adj;
  //
  return  (aT >= 0.);
}

//=================================================================================
// function: SetHistoryCollector
// purpose: 
//=================================================================================
void XBOP_SolidSolid::SetHistoryCollector(const Handle(XBOP_HistoryCollector)& theHistory) 
{
  if(theHistory.IsNull() ||
     !theHistory->IsKind(STANDARD_TYPE(XBOP_SolidSolidHistoryCollector)))
    myHistory.Nullify();
  else
    myHistory = theHistory;
}
