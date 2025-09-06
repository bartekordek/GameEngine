#include <gameengine/Primitives/Sphere.hpp>
#include <CUL/Math/Constants.hpp>

using namespace LOGLW;

CSphere::CSphere( IObject* parent ):
    IObject( "Sphere", false )
{
    setParent( parent );
    init( 24, 72, 10.f );
}

void CSphere::init( std::int32_t inStackCount, std::int32_t inSectorCount, float inRadius )
{
    m_stackCount = inStackCount;
    m_sectorCount = inSectorCount;
    m_radius = inRadius;

    fillIndices();
    fillVerticesNormalsTex();
}

void CSphere::fillIndices()
{
    int k1, k2;
    for( int i = 0; i < m_stackCount; ++i )
    {
        k1 = i * ( m_sectorCount + 1 );  // beginning of current stack
        k2 = k1 + m_sectorCount + 1;     // beginning of next stack

        for( int j = 0; j < m_sectorCount; ++j, ++k1, ++k2 )
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if( i != 0 )
            {
                m_indices.push_back( k1 );
                m_indices.push_back( k2 );
                m_indices.push_back( k1 + 1 );
            }

            // k1+1 => k2 => k2+1
            if( i != ( m_stackCount - 1 ) )
            {
                m_indices.push_back( k1 + 1 );
                m_indices.push_back( k2 );
                m_indices.push_back( k2 + 1 );
            }

            // store m_indices for lines
            // vertical lines for all stacks, k1 => k2
            m_lineIndices.push_back( k1 );
            m_lineIndices.push_back( k2 );
            if( i != 0 )  // horizontal lines except 1st stack, k1 => k+1
            {
                m_lineIndices.push_back( k1 );
                m_lineIndices.push_back( k1 + 1 );
            }
        }
    }
}

void CSphere::fillVerticesNormalsTex()
{
    float x, y, z, xy;                            // vertex position
    float nx, ny, nz, lengthInv = 1.0f / m_radius;  // vertex normal
    float s, t;                                   // vertex texCoord

    float sectorStep = 2 * CUL::Math::PI_F / static_cast<float>( m_sectorCount );
    float stackStep = CUL::Math::PI_F / m_stackCount;
    float sectorAngle, stackAngle;

    for( int i = 0; i <= m_stackCount; ++i )
    {
        stackAngle = CUL::Math::PI_F / 2 - i * stackStep;  // starting from pi/2 to -pi/2
        xy = m_radius * cosf( stackAngle );     // r * cos(u)
        z = m_radius * sinf( stackAngle );      // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for( int j = 0; j <= m_sectorCount; ++j )
        {
            sectorAngle = j * sectorStep;  // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf( sectorAngle );  // r * cos(u) * cos(v)
            y = xy * sinf( sectorAngle );  // r * cos(u) * sin(v)
            m_vertices.push_back( x );
            m_vertices.push_back( y );
            m_vertices.push_back( z );

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            m_normals.push_back( nx );
            m_normals.push_back( ny );
            m_normals.push_back( nz );

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / m_sectorCount;
            t = (float)i / m_stackCount;
            m_texCoords.push_back( s );
            m_texCoords.push_back( t );
        }
    }
}

void CSphere::render()
{
}

CSphere::~CSphere()
{
}