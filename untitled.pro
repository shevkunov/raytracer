#-------------------------------------------------
#
# Project created by QtCreator 2017-03-10T17:45:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    engine.cpp \
    src/canvas.cpp \
    src/scene.cpp \
    src/obj_loader.cpp \
    src/sphere.cpp \
    src/tracer.cpp \
    src/triangle.cpp \
    src/kdtree.cpp \
    src/color.cpp \
    src/utils.cpp \
    src/camera.cpp \
    src/quadrangle.cpp

HEADERS  += mainwindow.h \
    include/canvas.h \
    include/color.h \
    include/kdtree.h \
    include/obj_loader.h \
    include/queue.h \
    engine.h \
    include/utils.h \
    include/objects.h \
    include/sphere.h \
    include/triangle.h \
    include/scene.h \
    include/fog.h \
    include/camera.h \
    include/quadrangle.h

FORMS    += mainwindow.ui

DISTFILES += \
    work.txt
