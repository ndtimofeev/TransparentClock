// core
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QObject>
#include <QSettings>
#include <QTextStream>

// gui
#include <QPixmap>
#include <QPainter>

// widgets
#include <QApplication>
#include <QColorDialog>
#include <QDebug>
#include <QDesktopWidget>
#include <QDialog>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

// internal
#include "Clock.h"
#include "ui_ClockConfig.h"
#include "ui_ClockSettings.h"

void setLabelColor( QLabel* label, const QColor &color )
{
    QPixmap pix( label->size() );
    pix.fill( color );
    label->setPixmap( pix );
}

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    QCommandLineParser parser;
    parser.setApplicationDescription( "Desktop clock for composite windows manager." );
    auto helpOpt = parser.addHelpOption();
    auto skipOpt = QCommandLineOption({{ "s", "skip" }, "Skip existing config file"  });
    parser.addOption( skipOpt );

    if ( ! parser.parse( QCoreApplication::arguments() ) )
    {
        QTextStream stream( stdout );
        stream << parser.errorText() << '\n' << parser.helpText() << '\n';
        return -1;
    }

    if ( parser.isSet( helpOpt ) )
    {
        QTextStream stream( stdout );
        stream << parser.helpText() << '\n';
        return 0;
    }

    QSettings setting( "Noname", "TransparentClock" );

    auto winFlags = Qt::Window
        | Qt::FramelessWindowHint
        | Qt::WindowStaysOnTopHint
        | Qt::BypassWindowManagerHint
        | Qt::WindowDoesNotAcceptFocus;

    QVariant geom = setting.value( "Geometry" );
    if ( geom.isNull() || parser.isSet( skipOpt ) )
    {
        auto config   = new QWidget;
        auto ui       = new Ui::ClockConfig;
        ui->setupUi( config );
        ui->prototype->setColor( setting.value( "Color", QColor( Qt::red ) ).value<QColor>() );
        config->show();

        QObject::connect( ui->settingsButton, &QAbstractButton::clicked,
            [=, &winFlags, &setting]()
            {
                auto oldColor   = ui->prototype->color();
                auto settings   = new QDialog;
                auto settingsUi = new Ui::ClockSettings;
                settingsUi->setupUi( settings );
                if ( setting.value( "SkipMouseMode", true ).toBool() )
                    settingsUi->hideButton->setChecked( true );
                else
                    settingsUi->transparentForInputButton->setChecked( true );
                setLabelColor( settingsUi->colorButtonInner, oldColor );

                QObject::connect( settingsUi->colorButton, &QAbstractButton::clicked,
                    [=]()
                    {
                        auto oldColor = ui->prototype->color();
                        QColorDialog colorDialog( oldColor, settings );
                        QObject::connect( &colorDialog, &QColorDialog::currentColorChanged,
                            [=]( QColor c )
                            {
                                ui->prototype->setColor( c );
                                ui->prototype->repaint();
                            }
                        );

                        if ( colorDialog.exec() == 0 )
                        {
                            ui->prototype->setColor( oldColor );
                            ui->prototype->repaint();
                        }
                        else
                            setLabelColor( settingsUi->colorButtonInner, colorDialog.selectedColor() );
                    } );

                QObject::connect( settingsUi->buttonBox, &QDialogButtonBox::accepted,
                    [settingsUi, &winFlags]()
                    {
                        if ( settingsUi->transparentForInputButton->isChecked() )
                            winFlags |= Qt::WindowTransparentForInput;
                    } );

                QObject::connect( settingsUi->buttonBox, &QDialogButtonBox::rejected,
                    [=]()
                    {
                        ui->prototype->setColor( oldColor );
                        ui->prototype->repaint();
                    } );

                settings->show();
            } );

        QObject::connect( ui->lockButton, &QAbstractButton::clicked,
            [=, &winFlags, &setting]()
            {
                auto width =
                    ui->prototype->geometry().width() > ui->prototype->geometry().height()
                    ? ui->prototype->geometry().height() : ui->prototype->geometry().width() ;

                auto clock = new Clock( nullptr, winFlags );

                clock->setColor( ui->prototype->color() );
                clock->setSkipMouse( ! winFlags.testFlag( Qt::WindowTransparentForInput ) );
                clock->setAttribute( Qt::WA_TranslucentBackground );
                clock->setGeometry(
                    QRect(
                        QPoint( 0, 0 ),
                        QSize( width, width )
                    )
                );

                clock->move(
                    config->geometry().topLeft() +
                    ui->prototype->geometry().topLeft() +
                    QPoint(
                        ( ui->prototype->geometry().width() - width ) / 2,
                        ( ui->prototype->geometry().height() - width ) / 2
                    )
                );

                setting.setValue( "Color", ui->prototype->color() );
                setting.setValue( "Geometry", clock->geometry() );
                setting.setValue( "SkipMouseMode", ! winFlags.testFlag( Qt::WindowTransparentForInput ) );
                config->hide();
                clock->show();
            } );

    }
    else
    {
        auto clock = new Clock( nullptr, setting.value( "SkipMouseMode", true ).toBool() ? winFlags : winFlags | Qt::WindowTransparentForInput );
        clock->setColor( setting.value( "Color", QColor( Qt::red ) ).value<QColor>() );
        clock->setSkipMouse( true );
        clock->setAttribute( Qt::WA_TranslucentBackground );
        clock->setGeometry( geom.toRect() );
        clock->show();
    }

    return app.exec();
}
