QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calibraterelaywindow.cpp \
    controldali.cpp \
    controlscpi.cpp \
    jsoncontroller.cpp \
    logger.cpp \
    main.cpp \
    mainwindow.cpp \
    settingswindow.cpp \
    updatewindow.cpp \
    variantmaptablemodel.cpp

HEADERS += \
    calibraterelaywindow.h \
    controldali.h \
    controlscpi.h \
    jsoncontroller.h \
    logger.h \
    mainwindow.h \
    scpicommands.h \
    settingswindow.h \
    updatewindow.h \
    variantmaptablemodel.h

FORMS += \
    calibraterelaywindow.ui \
    mainwindow.ui \
    settingswindow.ui \
    updatewindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
	
