// -------------------------------------------------------------------------
// itkQELineCell.h
// $Revision $
// $Author $
// $Name:  $
// $Date: 2007-01-09 00:58:17 $
// -------------------------------------------------------------------------
// This code is an implementation of the well known quad edge (QE) data
// structure in the ITK library. Although the original QE can handle non
// orientable 2-manifolds and its dual and its mirror, this implementation
// is specifically dedicated to handle orientable 2-manifolds along with
// their dual.
//
// Any comment, criticism and/or donation is welcome.
//
// Please contact any member of the team:
//
// - The frog master (Eric Boix)       eboix@ens-lyon.fr
// - The duck master (Alex Gouaillard) alexandre.gouaillard@sun.com
// - The cow  master (Leonardo Florez) florez@creatis.insa-lyon.fr
// -------------------------------------------------------------------------

#ifndef __ITKQUADEDGEMESH__LINECELL__H__
#define __ITKQUADEDGEMESH__LINECELL__H__

#include <itkAutoPointer.h>
#include "itkQEQuadEdgeGeom.h"

namespace itkQE
{

/**
 * Class that connects the QE with itk
 *
 * \param TCellInterface Basic type for the itk*Cell. This usually comes
 *        from the MeshTraits.
 */
template< class TCellInterface >
    class LineCell
    : public TCellInterface, public TCellInterface::CellTraits::QuadEdgeType
{
    public:
    /** Standard class typedefs. */
    // itkCellCommonTypedefs
    typedef LineCell                       Self;
    typedef itk::AutoPointer< const Self > ConstSelfAutoPointer;
    typedef itk::AutoPointer< Self >       SelfAutoPointer;
    typedef Self*                          RawPointer;
    typedef const Self*                    ConstRawPointer;

    // itkCellInheritedTypedefs
    typedef TCellInterface                                Superclass;
    typedef typename Superclass::PixelType                PixelType;
    typedef typename Superclass::CellType                 CellType;
    typedef typename Superclass::CellAutoPointer          CellAutoPointer;
    typedef typename Superclass::CellConstAutoPointer     CellConstAutoPointer;
    typedef typename Superclass::CellRawPointer           CellRawPointer;
    typedef typename Superclass::CellConstRawPointer      CellConstRawPointer;
    typedef typename Superclass::CellTraits               CellTraits;
    typedef typename Superclass::CoordRepType             CoordRepType;
    typedef typename Superclass::InterpolationWeightType  InterpolationWeightType;
    typedef typename Superclass::PointIdentifier          PointIdentifier;
    typedef typename Superclass::CellIdentifier           CellIdentifier;
    typedef typename Superclass::CellFeatureIdentifier    CellFeatureIdentifier;
    typedef typename Superclass::CellFeatureIdentifier    CellFeatureCount;
    typedef typename Superclass::PointType                PointType;
    typedef typename Superclass::PointsContainer          PointsContainer;
    typedef typename Superclass::UsingCellsContainer      UsingCellsContainer;
    typedef typename Superclass::CellGeometry             CellGeometry;
    typedef typename Superclass::ParametricCoordArrayType ParametricCoordArrayType;
    typedef typename Superclass::ShapeFunctionsArrayType  ShapeFunctionsArrayType;
    itkStaticConstMacro( PointDimension, unsigned int, Superclass::PointDimension );
    itkStaticConstMacro( CellDimension, unsigned int, 2 );

    /** Multivisitor type. */
    typedef typename CellType::MultiVisitor MultiVisitor;

    /** QE types. */
    typedef typename Superclass::CellTraits::QuadEdgeType  QEType;
    typedef typename QEType::OrgRefType                    VertexRefType;
    typedef typename QEType::DualOrgRefType                FaceRefType;
    typedef typename QEType::PrimalDataType                PrimalDataType;
    typedef typename QEType::DualDataType                  DualDataType;
    typedef typename QEType::Dual                          QEDual;

    /** Iterator types. */
    typedef typename QEType::IteratorGeom      PointIdIterator;
    typedef typename QEType::ConstIteratorGeom PointIdConstIterator;

    public:
    /** Standard part of every itk Object. */
    itkTypeMacro( LineCell, TCellInterface );

    /** Methods for a QECell. */
//    itkQEMakeEdgeMacro( Self, QEDual );
     virtual void MakeEdge( )           
    {                                   
        Self* e2 = new Self( false );   
        QEDual* e1 = new QEDual( );             
        QEDual* e3 = new QEDual( );             
        this->SetRot( e1 );             
        e1->SetRot( e2 );               
        e2->SetRot( e3 );               
        e3->SetRot( this );             
        this->SetOnext( this );         
        e1->SetOnext( e3 );             
        e2->SetOnext( e2 );             
        e3->SetOnext( e1 );             
    }
   
    itkQEAccessorsMacro( QEType, Self, QEDual );

    public:
    /** Object memory management methods. */
    LineCell( bool makeEdge = true );
    virtual ~LineCell( ) { }

    /** Accessors for m_Ident. */
    void SetIdent( CellIdentifier cid ) { m_Ident = cid; }
    CellIdentifier GetIdent( )          { return( m_Ident ); }

    /** Implement the standard CellInterface. */
    SelfAutoPointer New( );

    /** TCellInterface abstract methods definition. */
    virtual void Accept( unsigned long cellId, MultiVisitor* mv );
    virtual CellGeometry GetType( ) const { return( Superclass::LINE_CELL ); }

    /** itk topology related methods. */
    static int GetTopologyId( )
    { return( Superclass::LINE_CELL ); }
    virtual unsigned int GetDimension( ) const
    { return( Self::CellDimension ); }
    virtual unsigned int GetNumberOfPoints( ) const
    { return( 2 ); }
    virtual CellFeatureCount GetNumberOfBoundaryFeatures( int dimension ) const;
    virtual bool GetBoundaryFeature( int dimension,
                                     CellFeatureIdentifier cellId,
                                     CellAutoPointer& cell );

    /** Useless methods. */
    virtual void MakeCopy( CellAutoPointer& cell ) const { (void)cell; }

    /** Iterator-related methods. */
    virtual void SetPointIds( PointIdConstIterator first );
    virtual void SetPointIds( PointIdConstIterator first,
                              PointIdConstIterator last );
    virtual void SetPointId( int localId, PointIdentifier pId );

    virtual PointIdIterator PointIdsBegin( );
    virtual PointIdIterator PointIdsEnd( );

    virtual PointIdConstIterator GetPointIds( ) const;
    virtual PointIdConstIterator PointIdsBegin( ) const;
    virtual PointIdConstIterator PointIdsEnd( ) const;

    private:
    LineCell( const Self& );       // Not impl.
    void operator=( const Self& ); // Not impl.

    private:
    /**
     * In order to have constant time access at the itk level instead of 
     * of doing a search in the Mesh::Cell container.
     */
    CellIdentifier m_Ident;
};

} // enamespace

#include "itkQELineCell.txx"

#endif // __ITKQUADEDGEMESH__LINECELL__H__

// eof - itkQELineCell.h
