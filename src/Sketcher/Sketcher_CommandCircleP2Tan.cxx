// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// Original work from OpenCascade Sketcher 
// (http://sourceforge.net/projects/occsketcher/)
//
// Modifications for GEOM and OCAF
// Authored by : Sioutis Fotios (sfotis@gmail.com)

//------------------------------------------------------------------------------
#include "Sketcher_CommandCircleP2Tan.hxx"
//------------------------------------------------------------------------------
#include <TColStd_HSequenceOfTransient.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc_Circ2d3Tan.hxx>
#include <Sketcher_Edge.hxx>
#include <Sketcher_Arc.hxx>
//------------------------------------------------------------------------------
Sketcher_CommandCircleP2Tan::Sketcher_CommandCircleP2Tan()
    : temp_Circ2d(gp::OX2d(),SKETCHER_RADIUS),
    best_Circ(gp::XOY(),SKETCHER_RADIUS)
{
  myCircleP2TanAction = Nothing;
  minDistance = 0;
  curDistance = 0;
  u2 = u_temp = 0;
  mySecondgp_Pnt2d = gp::Origin2d();
  First2dPoint = new Geom2d_CartesianPoint(myFirstgp_Pnt2d);
  tempGeom2d_Circle = new Geom2d_Circle(temp_Circ2d);
  gp_linExist = Standard_False;
  gp_circExist = Standard_False;
}
//------------------------------------------------------------------------------
Sketcher_CommandCircleP2Tan::~Sketcher_CommandCircleP2Tan()
{
}
//------------------------------------------------------------------------------
void Sketcher_CommandCircleP2Tan::Action()
{
  myCircleP2TanAction = Input_1CirclePoint;
}
//------------------------------------------------------------------------------
Standard_Boolean Sketcher_CommandCircleP2Tan::MouseInputEvent(const gp_Pnt2d& thePnt2d)
{
  curPnt2d = myAnalyserSnap->MouseInput(thePnt2d);

  switch (myCircleP2TanAction) {
    case Nothing:
      break;
    case Input_1CirclePoint:
      First2dPoint->SetPnt2d(curPnt2d);
      myRubberLineFirstPoint->SetPnt(ElCLib::To3d(curCoordinateSystem.Ax2(),curPnt2d));
      myCircleP2TanAction = Input_2CircleTan;
      break;
    case Input_2CircleTan:
      SelectCurve();

      if (StoregpObject()) {
        u2 = u_temp;
        mySecondgp_Pnt2d = tempPnt2d;
        mySecond2d_Curve = tempGeom2d_Curve;
        myRubberLineSecondPoint->SetPnt(ElCLib::To3d(curCoordinateSystem.Ax2(),tempPnt2d));
        myCircleP2TanAction = Input_3CircleTan;
      }

      break;
    case Input_3CircleTan:
      SelectCurve();

      if (CheckgpObject()) {
        Geom2dAdaptor_Curve SecondAdaptor_Curve(mySecond2d_Curve);
        Geom2dAdaptor_Curve ThirdAdaptor_Curve(tempGeom2d_Curve);
        Geom2dGcc_QualifiedCurve SecondQualifiedCurve(SecondAdaptor_Curve,GccEnt_unqualified);
        Geom2dGcc_QualifiedCurve ThirdQualifiedCurve(ThirdAdaptor_Curve,GccEnt_unqualified);
        Geom2dGcc_Circ2d3Tan myGcc_Circ2d3Tan(SecondQualifiedCurve,ThirdQualifiedCurve,First2dPoint,1.0e-12,u2,u_temp);

        if (myGcc_Circ2d3Tan.IsDone() && myGcc_Circ2d3Tan.NbSolutions() >0) {
          temp_Circ2d = myGcc_Circ2d3Tan.ThisSolution(1);
          Standard_Integer bestCircleIndex = 1;

          if (SumDistanceToCircle()) {
            minDistance = curDistance;
            best_Circ = ElCLib::To3d(curCoordinateSystem.Ax2(),temp_Circ2d);

            for (Standard_Integer i=2; i<=myGcc_Circ2d3Tan.NbSolutions(); i++) {
              temp_Circ2d = myGcc_Circ2d3Tan.ThisSolution(i);

              if (SumDistanceToCircle() && minDistance > curDistance) {
                minDistance = curDistance;
                bestCircleIndex = i;
                best_Circ = ElCLib::To3d(curCoordinateSystem.Ax2(),temp_Circ2d);
              }
            }

            Handle(Geom2d_Circle) myGeom2d_Circle = new Geom2d_Circle(myGcc_Circ2d3Tan.ThisSolution(bestCircleIndex));
            AddObject(myGeom2d_Circle,CircleSketcherObject);
            RemoveRubberLine();
            RemoveRubberCircle();
            gp_linExist = Standard_False;
            gp_circExist = Standard_False;
            myCircleP2TanAction = Input_1CirclePoint;
          }
        }
      }

      break;
    default:
      break;
  }

  return Standard_False;
}
//------------------------------------------------------------------------------
void Sketcher_CommandCircleP2Tan::MouseMoveEvent(const gp_Pnt2d& thePnt2d)
{
  curPnt2d = myAnalyserSnap->MouseMove(thePnt2d);

  switch (myCircleP2TanAction) {
    case Nothing:
      break;
    case Input_1CirclePoint:
      break;
    case Input_2CircleTan:
      SelectCurve();

      if (minDistance != MINIMUMDISTANCETOCURVE) {
        myRubberLineSecondPoint->SetPnt(ElCLib::To3d(curCoordinateSystem.Ax2(),tempPnt2d));
        DisplayRubberLine();
      }

      break;
    case Input_3CircleTan:
      SelectCurve();

      if (CheckgpObject()) {
        Geom2dAdaptor_Curve SecondAdaptor_Curve(mySecond2d_Curve);
        Geom2dAdaptor_Curve ThirdAdaptor_Curve(tempGeom2d_Curve);
        Geom2dGcc_QualifiedCurve SecondQualifiedCurve(SecondAdaptor_Curve,GccEnt_unqualified);
        Geom2dGcc_QualifiedCurve ThirdQualifiedCurve(ThirdAdaptor_Curve,GccEnt_unqualified);
        Geom2dGcc_Circ2d3Tan myGcc_Circ2d3Tan(SecondQualifiedCurve,ThirdQualifiedCurve,First2dPoint,1.0e-12,u2,u_temp);

        if (myGcc_Circ2d3Tan.IsDone() && myGcc_Circ2d3Tan.NbSolutions() >0) {
          temp_Circ2d = myGcc_Circ2d3Tan.ThisSolution(1);

          if (SumDistanceToCircle()) {
            minDistance = curDistance;
            best_Circ = ElCLib::To3d(curCoordinateSystem.Ax2(),temp_Circ2d);

            for (Standard_Integer i=2; i<=myGcc_Circ2d3Tan.NbSolutions(); i++) {
              temp_Circ2d = myGcc_Circ2d3Tan.ThisSolution(i);

              if (SumDistanceToCircle() && minDistance > curDistance) {
                minDistance = curDistance;
                best_Circ = ElCLib::To3d(curCoordinateSystem.Ax2(),temp_Circ2d);
              }
            }

            myRubberCircleGeometry->SetCirc(best_Circ);
            DisplayRubberCircle();
          }
        }
      }

      break;
    default:
      break;
  }
}
//------------------------------------------------------------------------------
void Sketcher_CommandCircleP2Tan::CancelEvent()
{
  switch (myCircleP2TanAction) {
    case Nothing:
      break;
    case Input_1CirclePoint:
      break;
    case Input_2CircleTan:
      RemoveRubberLine();
      break;
    case Input_3CircleTan:
      RemoveRubberLine();
      RemoveRubberCircle();
      break;
    default:
      break;
  }

  gp_linExist = Standard_False;
  gp_circExist = Standard_False;
  myCircleP2TanAction = Nothing;
}
//------------------------------------------------------------------------------
Sketcher_ObjectTypeOfMethod Sketcher_CommandCircleP2Tan::GetTypeOfMethod()
{
  return CircleP2Tan_Method;
}
//------------------------------------------------------------------------------
void Sketcher_CommandCircleP2Tan::SelectCurve()
{
  minDistance = MINIMUMDISTANCETOCURVE;

  for (Standard_Integer i=1; i<=data->Length(); i++) {
    mySObject = Handle(Sketcher_Object)::DownCast(data->Value(i));

    switch (mySObject->GetGeometryType()) {
      case PointSketcherObject:
        break;
      case LineSketcherObject: {
          Handle(Sketcher_Edge) curSketcher_Edge = Handle(Sketcher_Edge)::DownCast(mySObject->GetGeometry());
          myProjectOnCurve.Init(curPnt2d,curSketcher_Edge,curSketcher_Edge->StartParameter(),curSketcher_Edge->EndParameter());

          if (myProjectOnCurve.NbPoints() > 0) {
            curDistance = curPnt2d.Distance(myProjectOnCurve.NearestPoint());

            if (minDistance > curDistance) {
              minDistance = curDistance;
              tempPnt2d = myProjectOnCurve.NearestPoint();
              u_temp = ElCLib::Parameter(curSketcher_Edge->Lin2d(),tempPnt2d);
              Handle(Geom2d_Line) temp_Line = new Geom2d_Line(curSketcher_Edge->Lin2d());
              tempGeom2d_Curve = temp_Line;
            }
          }
        }
        break;
      case CircleSketcherObject: {
          Handle(Geom2d_Circle) curGeom2d_Circle = Handle(Geom2d_Circle)::DownCast(mySObject->GetGeometry());
          myProjectOnCurve.Init(curPnt2d,curGeom2d_Circle);

          if (myProjectOnCurve.NbPoints() > 0) {
            curDistance = curPnt2d.Distance(myProjectOnCurve.NearestPoint());

            if (minDistance > curDistance) {
              minDistance = curDistance;
              tempPnt2d = myProjectOnCurve.NearestPoint();
              u_temp = ElCLib::Parameter(curGeom2d_Circle->Circ2d(),tempPnt2d);
              tempGeom2d_Curve = curGeom2d_Circle;
            }
          }
        }
        break;
      case ArcSketcherObject: {
          Handle(Sketcher_Arc) curSketcher_Arc = Handle(Sketcher_Arc)::DownCast(mySObject->GetGeometry());
          myProjectOnCurve.Init(curPnt2d,curSketcher_Arc);

          if (myProjectOnCurve.NbPoints() > 0) {
            curDistance = curPnt2d.Distance(myProjectOnCurve.NearestPoint());

            if (minDistance > curDistance) {
              minDistance = curDistance;
              tempPnt2d = myProjectOnCurve.NearestPoint();
              u_temp = ElCLib::Parameter(curSketcher_Arc->Circ2d(),tempPnt2d);
              Handle(Geom2d_Circle) temp_Circle = new Geom2d_Circle(curSketcher_Arc->Circ2d());
              tempGeom2d_Curve = temp_Circle;
            }
          }
        }
        break;
      default:
        break;
    }
  }
}
//------------------------------------------------------------------------------
Standard_Boolean Sketcher_CommandCircleP2Tan::SumDistanceToCircle()
{
  curDistance = 0;
  tempGeom2d_Circle->SetCirc2d(temp_Circ2d);
  myProjectOnCurve.Init(mySecondgp_Pnt2d,tempGeom2d_Circle);

  if (myProjectOnCurve.NbPoints() > 0)
    curDistance = mySecondgp_Pnt2d.Distance(myProjectOnCurve.NearestPoint());
  else
    return Standard_False;

  myProjectOnCurve.Init(tempPnt2d,tempGeom2d_Circle);

  if (myProjectOnCurve.NbPoints() > 0)
    curDistance += tempPnt2d.Distance(myProjectOnCurve.NearestPoint());
  else
    return Standard_False;

  return Standard_True;
}
//------------------------------------------------------------------------------
Standard_Boolean Sketcher_CommandCircleP2Tan::CheckgpObject()
{
  if (minDistance != MINIMUMDISTANCETOCURVE) {
    if (tempGeom2d_Curve->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      Handle(Geom2d_Line) tnl = Handle(Geom2d_Line)::DownCast(tempGeom2d_Curve);

      if (gp_linExist)
        if (gp_lin.Position().IsParallel(tnl->Lin2d().Position(),1.0e-12) && gp_lin.Contains(tnl->Lin2d().Location(),1.0e-12))
          return Standard_False;
    }
    else if (tempGeom2d_Curve->IsKind(STANDARD_TYPE(Geom2d_Circle))) {
      Handle(Geom2d_Circle) tnc = Handle(Geom2d_Circle)::DownCast(tempGeom2d_Curve);

      if (gp_circExist) {
        if (gp_circ.Location().IsEqual(tnc->Circ2d().Location(),1.0e-12) && gp_circ.Radius() == tnc->Circ2d().Radius())
          return Standard_False;
      }
    }

    return Standard_True;
  }
  else return Standard_False;
}
//------------------------------------------------------------------------------
Standard_Boolean Sketcher_CommandCircleP2Tan::StoregpObject()
{
  if (minDistance != MINIMUMDISTANCETOCURVE) {
    if (tempGeom2d_Curve->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      Handle(Geom2d_Line) tnl = Handle(Geom2d_Line)::DownCast(tempGeom2d_Curve);
      gp_lin = tnl->Lin2d();
      gp_linExist = Standard_True;
    }
    else if (tempGeom2d_Curve->IsKind(STANDARD_TYPE(Geom2d_Circle))) {
      Handle(Geom2d_Circle) tnc = Handle(Geom2d_Circle)::DownCast(tempGeom2d_Curve);
      gp_circ = tnc->Circ2d();
      gp_circExist = Standard_True;
    }

    return Standard_True;
  }
  else return Standard_False;
}
//------------------------------------------------------------------------------
IMPLEMENT_STANDARD_HANDLE(Sketcher_CommandCircleP2Tan,Sketcher_Command)
IMPLEMENT_STANDARD_RTTI(Sketcher_CommandCircleP2Tan)
IMPLEMENT_STANDARD_TYPE(Sketcher_CommandCircleP2Tan)
IMPLEMENT_STANDARD_SUPERTYPE(Sketcher_Command)
IMPLEMENT_STANDARD_SUPERTYPE(MMgt_TShared)
IMPLEMENT_STANDARD_SUPERTYPE(Standard_Transient)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(Sketcher_Command)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(MMgt_TShared)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(Standard_Transient)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(Sketcher_CommandCircleP2Tan)
//------------------------------------------------------------------------------
