QT += widgets
TEMPLATE = app
TARGET = TransperentClock
INCLUDEPATH += .
target.path = /usr/bin
INSTALLS += target

# Input
HEADERS += Clock.h
FORMS += ClockConfig.ui ClockSettings.ui
SOURCES += Clock.cpp Main.cpp
