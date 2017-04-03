#-------------------------------------------------
#
# Project created by QtCreator 2015-08-20T16:48:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ShipCAD
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        pointdialog.cpp \
    viewportcontainer.cpp \
    insertplanepointsdialog.cpp \
    intersectlayersdialog.cpp \
    extrudeedgedialog.cpp

HEADERS  += mainwindow.h \
        pointdialog.h \
    viewportcontainer.h \
    insertplanepointsdialog.h \
    intersectlayersdialog.h \
    extrudeedgedialog.h

FORMS    += mainwindow.ui \
        pointdialog.ui \
    insertplanepointsdialog.ui \
    intersectlayersdialog.ui \
    extrudeedgedialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ShipCADlib/release/ -lShipCADlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ShipCADlib/debug/ -lShipCADlib
else:unix: LIBS += -L$$OUT_PWD/../ShipCADlib/ -lShipCADlib

INCLUDEPATH += $$PWD/../ShipCADlib
DEPENDPATH += $$PWD/../ShipCADlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ShipCADlib/release/libShipCADlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ShipCADlib/debug/libShipCADlib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ShipCADlib/release/ShipCADlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ShipCADlib/debug/ShipCADlib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../ShipCADlib/libShipCADlib.a
