/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkQuadEdgeMeshIteratorTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkQuadEdgeMesh.h"
#include <iostream>

int itkQuadEdgeMeshIteratorTest( int , char* [] )
{
  typedef itk::QuadEdgeMesh< double, 3 >   MeshType;
  typedef MeshType::QEPrimal               QEPrimal;
  typedef MeshType::QEPrimal::IteratorGeom IteratorGeom;
  std::string indent = "    ";

  MeshType::Pointer mesh = MeshType::New( );

  //
  //                    p2--------------p1               //
  //                    /                 \              //
  //                   /                   \             //
  //                  /                     \            //
  //                 /                       \           //
  //                /                         \          //
  //               /                           \         //
  //              /                             \        //
  //            p3             FACE              p0      //
  //              \                             /        //
  //               \                           /         //
  //                \                         /          //
  //                 \                       /           //
  //                  \                     /            //
  //                   \                   /             //
  //                    \                 /              //
  //                    p4---------------p5              //
  //
  #define NumPoints 6
  #define NumEdges 6
  MeshType::PixelType a = sqrt( 3.0 ) / 2.0;
  MeshType::PixelType points[ NumPoints ][ 3 ] = { {  1.0, 0.0, 0.0 },
                                                   {  0.5,   a, 0.0 },
                                                   { -0.5,   a, 0.0 },
                                                   { -1.0, 0.0, 0.0 },
                                                   { -0.5,  -a, 0.0 },
                                                   {  0.5,  -a, 0.0 } };
  MeshType::PointType pnts[ NumPoints ];
  MeshType::PointIdentifier pids[ NumPoints ];
  for( int i = 0; i < NumPoints; i++ )
    {
    for( int j = 0; j < 3; j++ )
      {
      pnts[ i ][ j ] = points[ i ][ j ];
      }
    pids[ i ] = mesh->AddPoint( pnts[ i ] );
    }

  MeshType::QEPrimal* edges[ NumEdges ];
  for( int i = 0; i < NumPoints; i++ )
    {
    edges[ i ] = mesh->AddEdge( pids[ i ], pids[ (i + 1 ) % NumPoints ] );
    }
   
  MeshType::QEPrimal* foundEdges[ NumEdges ];
  for( int i = 0; i < NumPoints; i++ )
    {
    foundEdges[ i ] = mesh->FindEdge( pids[ i ] ,
                                      pids[ (i + 1 ) % NumEdges ] );
    if( ! foundEdges[ i ] )
      {
      std::cerr << "Unfound edge [" 
                << pids[ i ] << ", "
                << pids[ (i + 1 ) % NumEdges ] << "]. Failed."
                << std::endl;
      return EXIT_FAILURE;
      }
    }

  // Create the face:
  mesh->AddFace( foundEdges[ 0 ] );

  // Coarse checking with the cyclicity
  if( foundEdges[ 0 ]->IsLnextGivenSizeCyclic( 6 ) == false )
    {
    std::cerr << "Lnext cyclicity is not 6 as expected."
              << std::endl;
    return EXIT_FAILURE;
    }

  //////////////////////////////////////////////////////////
  int edgeNumber = 0;
  std::cout << "Testing Lnext iterator... ";
  for( IteratorGeom itLnext  = foundEdges[ 0 ]->BeginGeomLnext( );
                    itLnext != foundEdges[ 0 ]->EndGeomLnext( );
                    itLnext++, edgeNumber++ )
    {
    MeshType::QEPrimal* expectedEdge = foundEdges[ edgeNumber ];
    MeshType::QEPrimal* currentEdge  = itLnext.Value( );
    if ( currentEdge != expectedEdge )
      {
      std::cout << std::endl
                << "Erroneous edge: was expecting ["
                << expectedEdge->GetOrigin( )
                << ", "
                << expectedEdge->GetDestination( )
                << "], but got ["
                << currentEdge->GetOrigin( ) << ", "
                << currentEdge->GetDestination( ) << "]. Failed."
                << std::endl;
      return EXIT_FAILURE;
      }
    }
  std::cout << "OK" << std::endl;

  //////////////////////////////////////////////////////////
  edgeNumber = 0;
  std::cout << "Testing Rnext iterator... ";
  for( IteratorGeom itRnext  = foundEdges[ 0 ]->BeginGeomRnext( );
                    itRnext != foundEdges[ 0 ]->EndGeomRnext( );
                    itRnext++, edgeNumber++ )
    {
    // We stat with [0,1] and should get [0,5], [5,4], [4,3]...
    MeshType::QEPrimal* expectedEdge = foundEdges[ ( NumEdges - edgeNumber ) % NumEdges ];
    MeshType::QEPrimal* currentEdge  = itRnext.Value( );
    if ( currentEdge != expectedEdge )
      {
      std::cout << std::endl
                << "Erroneous edge: was expecting ["
                << expectedEdge->GetOrigin( )
                << ", "
                << expectedEdge->GetDestination( )
                << "], but got ["
                << currentEdge->GetOrigin( ) << ", "
                << currentEdge->GetDestination( ) << "]. Failed."
                << std::endl;
      return EXIT_FAILURE;
      }
    }
  std::cout << "OK" << std::endl;

  //////////////////////////////////////////////////////////
  // Rotating around destination, there is only one other edge
  // BeginGeomDnext( ) returns the edge itself to begin with
  edgeNumber = 0;
  std::cout << "Testing Dnext iterator... ";
  for( IteratorGeom itDnext  = foundEdges[ 0 ]->BeginGeomDnext( );
                    itDnext != foundEdges[ 0 ]->EndGeomDnext( );
                    itDnext++, edgeNumber++ )
    {
    MeshType::QEPrimal* expectedEdge;
    if( edgeNumber ) 
      {
      expectedEdge = foundEdges[ edgeNumber ]->GetSym( );
      }
    else
      {
      expectedEdge = foundEdges[ edgeNumber ];
      }
    MeshType::QEPrimal* currentEdge  = itDnext.Value( );
    if ( currentEdge != expectedEdge )
      {
      std::cout << std::endl
                << "Erroneous edge: was expecting ["
                << expectedEdge->GetOrigin( )
                << ", "
                << expectedEdge->GetDestination( )
                << "], but got ["
                << currentEdge->GetOrigin( ) << ", "
                << currentEdge->GetDestination( ) << "]. Failed."
                << std::endl;
      return EXIT_FAILURE;
      }
    }
  std::cout << "OK" << std::endl;

  std::cout << "QuadEdgeMeshIteratorTest Passed !" << std::endl;

  return EXIT_SUCCESS;
}
