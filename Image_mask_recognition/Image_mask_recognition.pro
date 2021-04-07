QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    custom/blog.cpp \
    custom/bpdfcreater.cpp \
    custom/btablewidget.cpp \
    custom/globalfun.cpp \
    custom/gpdf.cpp \
    dialog/about_us_dialog.cpp \
    dialog/customer_engineer_dialog.cpp \
    dialog/exportreport_dialog.cpp \
    dialog/result_setting_dialog.cpp \
    dialog/slice_analysis_dialog.cpp \
    dialog/unit_setting_dialog.cpp \
    dialog/limit_setting_dialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    classlib/include/algorithmMain.h \
    classlib/include/baseFunc.h \
    classlib/include/edgedetector.h \
    classlib/include/filter.h \
    classlib/include/psf.h \
    classlib/include/psi.h \
    classlib/include/pztcalibrate.h \
    classlib/include/result.h \
    classlib/include/splice.h \
    classlib/include/unwrap.h \
    classlib/include/zernike.h \
    custom/blog.h \
    custom/bpdfcreater.h \
    custom/btablewidget.h \
    custom/globalfun.h \
    custom/gpdf.h \
    dialog/about_us_dialog.h \
    dialog/customer_engineer_dialog.h \
    dialog/exportreport_dialog.h \
    dialog/result_setting_dialog.h \
    dialog/slice_analysis_dialog.h \
    dialog/unit_setting_dialog.h \
    dialog/limit_setting_dialog.h \
    mainwindow.h

FORMS += \
    dialog/about_us_dialog.ui \
    dialog/customer_engineer_dialog.ui \
    dialog/exportreport_dialog.ui \
    dialog/result_setting_dialog.ui \
    dialog/slice_analysis_dialog.ui \
    dialog/unit_setting_dialog.ui \
    dialog/limit_setting_dialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#******************************************************************************************************************

TARGET = Sirius-Pro_Free

DESTDIR = ./

CONFIG += debug_and_release

QT += charts printsupport datavisualization opengl

RC_ICONS = Image_mask_recognition.ico

LIBS += -luser32

include("../Draw_Figure/draw_figure.pri")
include("../BPlot/bplot.pri")

RESOURCES += \
    image.qrc \
    source.qrc

INCLUDEPATH += $$PWD \
INCLUDEPATH += $$PWD/custom \
INCLUDEPATH += $$PWD/classlib/include/

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/classlib/win64/debug/ -lAlgorithmLib_64
} else {
    LIBS += -L$$PWD/classlib/win64/release/ -lAlgorithmLib_64
}
