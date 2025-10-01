QT += core gui widgets openglwidgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


LIBS += -L"$$PWD/vcpkg/installed/x64-mingw-dynamic/lib" -lassimp
INCLUDEPATH += $$PWD/vcpkg/installed/x64-mingw-dynamic/include

SOURCES += \
    MyOpenGLWidget.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp

HEADERS += \
    MyOpenGLWidget.h \
    mainwindow.h \
    model.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    fragment_shader.frag \
    vertex_shader.vert

RESOURCES += \
    resources.qrc
