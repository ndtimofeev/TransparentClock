QT += widgets
TEMPLATE = app
TARGET = TransparentClock
INCLUDEPATH += .
target.path = /usr/bin
INSTALLS += target

# Input
HEADERS += Clock.h
FORMS += ClockConfig.ui ClockSettings.ui
SOURCES += Clock.cpp Main.cpp
