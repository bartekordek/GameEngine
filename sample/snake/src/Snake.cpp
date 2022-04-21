#include "Snake.hpp"

Snake::Snake( int row, int col )
{
    std::lock_guard<std::mutex> lockGuard( m_snakeMtx );
    m_snakePositions.push_front( { row, col } );
}

bool Snake::nextMoveIsGameOver( int row, int col ) const
{
    auto newPostion = calculateNextPosition();

    if( newPostion.row == -1 ||
        newPostion.row == row ||
        newPostion.col == -1 ||
        newPostion.col == col )
    {
        return true;
    }

    for( const auto& snakeSegment: m_snakePositions )
    {
        if( snakeSegment == newPostion )
        {
            return true;
        }
    }

    return false;
}

void Snake::move( bool increase )
{
    std::lock_guard<std::mutex> lockGuard( m_snakeMtx );
    Pos newPos;
    const Pos& lastPos = m_snakePositions.front();
    newPos = lastPos;

    if( m_headDirection == Snake::HeadDirection::NORTH )
    {
        ++newPos.row;
    }
    else if ( m_headDirection == Snake::HeadDirection::SOUTH )
    {
        --newPos.row;
    }
    else if( m_headDirection == Snake::HeadDirection::EAST )
    {
        ++newPos.col;
    }
    else if( m_headDirection == Snake::HeadDirection::WEST )
    {
        --newPos.col;
    }

    m_snakePositions.push_front( newPos );


    if( !increase )
    {
        m_snakePositions.pop_back();
    }
}

bool Snake::isFieldSnake( const Pos& position ) const
{
    std::lock_guard<std::mutex> lockGuard( m_snakeMtx );
    for( const auto& segment: m_snakePositions )
    {
        if( position == segment )
        {
            return true;
        }
    }
    return false;
}

Snake::Pos Snake::calculateNextPosition() const
{
    Pos nextPosition = m_snakePositions.front();

    if( m_headDirection == Snake::HeadDirection::NORTH )
    {
        ++nextPosition.row;
    }
    else if( m_headDirection == Snake::HeadDirection::SOUTH )
    {
        --nextPosition.row;
    }
    else if( m_headDirection == Snake::HeadDirection::EAST )
    {
        ++nextPosition.col;
    }
    else if( m_headDirection == Snake::HeadDirection::WEST )
    {
        --nextPosition.col;
    }

    return nextPosition;
}


void Snake::reset()
{
    std::lock_guard<std::mutex> lockGuard( m_snakeMtx );
    m_snakePositions.clear();
}

Snake::~Snake()
{

}