#ifndef _CLOCK_H_57584_
#define _CLOCK_H_57584_

// core
#include <QObject>
#include <QTimer>

// gui
#include <QDropEvent>

// widgets
#include <QWidget>

class Clock : public QWidget
{
public:
    Clock( QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags() );
    virtual ~Clock() override;

    virtual void paintEvent( QPaintEvent* event ) override;
    void enterEvent( QEvent* event ) override;

    bool skipMouse() const;
    void setSkipMouse( bool );

    QColor color() const;
    void setColor( const QColor& );

    Clock( const Clock & ) = delete;
    Clock( Clock && ) = delete;
    Clock& operator = ( const Clock & ) = delete;
    Clock& operator = ( Clock && ) = delete;

private:
    QColor                  color_;
    QTimer*                 timer;
    QMetaObject::Connection conn;
    bool                    skipMouseState;
};


#endif /* end of include guard: _CLOCK_H_57584_ */
