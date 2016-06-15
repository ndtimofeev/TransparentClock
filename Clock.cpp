// core
#include <QDebug>
#include <QObject>
#include <QTime>
#include <QTimer>
#include <QtMath>

// gui
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>

// widgets
#include <QWidget>

// internal
#include "Clock.h"

Clock::Clock( QWidget* parent, Qt::WindowFlags flags )
    : QWidget( parent, flags )
    , color_( Qt::red )
    , timer( new QTimer )
    , skipMouseState( false )
{
    QObject::connect( this->timer, &QTimer::timeout, [this](){ this->repaint(); } );
    this->timer->start( 500 );
    this->setMouseTracking( true );
}

Clock::~Clock()
{}

void Clock::paintEvent( QPaintEvent* event )
{
    auto rect     = this->rect();
    auto width    = rect.width() > rect.height() ? rect.height() : rect.width();
    rect.setWidth( width );
    rect.setHeight( width );
    rect.moveCenter( this->rect().center() );
    auto c        = this->color_;

    auto time     = QTime::currentTime();
    int  secs     = time.second();
    int  mins     = time.minute();
    int  hour     = time.hour() > 11 ? time.hour() - 12 : time.hour();
    int  penW     = 10;
    auto secPoint = QRect( 0, 0, penW, penW );
    auto secArea  = rect.marginsRemoved( QMargins( 1.25 * penW, 1.25 * penW, 1.25 * penW, 1.25 * penW ) );
    double angle  = qDegreesToRadians( (double)( 90 - secs * 6 ) );
    int  xPos     = secArea.center().x() + qCos( angle ) * secArea.width() / 2;
    int  yPos     = secArea.center().y() - qSin( angle ) * secArea.height() / 2;

    c.setAlpha( 127 );
    QPainter painter( this );
    painter.setPen( QPen( c, penW, Qt::SolidLine, Qt::RoundCap ) );

    painter.drawArc(
        rect.marginsRemoved( QMargins( penW / 2, penW / 2, penW / 2, penW / 2 ) ),
        90 * 16,
        -6 * mins * 16
    );

    c.setAlpha( 170 );
    painter.setPen( QPen( c, penW, Qt::SolidLine, Qt::RoundCap ) );
    painter.drawArc(
        rect.marginsRemoved( QMargins( 2 * penW, 2 * penW, 2 * penW, 2 * penW ) ),
        90 * 16,
        -30 * hour * 16
    );

    secPoint.moveCenter( QPoint( xPos, yPos ) );
    painter.drawEllipse( secPoint );

    QFont font;
    font.setPixelSize( 50 );
    font.setBold( true );
    font.setStyleHint( QFont::Monospace );
    painter.setFont( font );
    painter.drawText( rect, Qt::AlignCenter, time.toString( "HH:mm" ) );

    QWidget::paintEvent( event );
}

void Clock::mouseMoveEvent( QMouseEvent* )
{
    if ( this->skipMouseState )
    {
        this->hide();
        this->conn = QObject::connect( this->timer, &QTimer::timeout,
            [=]()
            {
                if ( ! this->geometry().contains( QCursor::pos() ) )
                {
                    this->show();
                    QObject::disconnect( this->conn );
                }
            } );
    }
}

bool Clock::skipMouse() const
{
    return this->skipMouseState;
}

void Clock::setSkipMouse( bool b )
{
    this->skipMouseState = b;
}

QColor Clock::color() const
{
    return this->color_;
}

void Clock::setColor( const QColor &c )
{
    this->color_ = c;
}
