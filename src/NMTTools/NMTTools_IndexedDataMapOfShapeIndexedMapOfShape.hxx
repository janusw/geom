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
// File:        NMTTools_IndexedDataMapOfIndexedMapOfInteger.hxx
// Created:     Mon Feb 20 09:20:07 2012
// Author:
//              <pkv@BDEURI37616>
// File:        NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx
// Created:     Mon Feb 20 11:59:23 2012
// Author:
//              <pkv@BDEURI37616>


#ifndef NMTTools_IndexedDataMapOfShapeIndexedMapOfShape_HeaderFile
#define NMTTools_IndexedDataMapOfShapeIndexedMapOfShape_HeaderFile


#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#define _NCollection_MapHasher
#include <NCollection_IndexedDataMap.hxx>


typedef NCollection_IndexedDataMap <TopoDS_Shape, TopTools_IndexedMapOfShape, TopTools_ShapeMapHasher> NMTTools_IndexedDataMapOfShapeIndexedMapOfShape;

#undef _NCollection_MapHasher


#endif

