#-------------------------------------------------
#
# Project created by QtCreator 2017-04-05T14:33:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stitching
TEMPLATE = app

CONFIG += C++11

android{

message("android");

#QT += androidextras
#QT += multimedia multimediawidgets

#CONFIG += mobility

#MOBILITY =

ANDROID_OPENCV = /home/zsh/android_env/OpenCV-android-sdk/sdk/native

INCLUDEPATH += \
    $$ANDROID_OPENCV/jni/include/opencv    \
    $$ANDROID_OPENCV/jni/include/opencv2    \
    $$ANDROID_OPENCV/jni/include

LIBS +=\
     #$$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_contrib.a \
     #$$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_legacy.a \
    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_ml.a \
    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_objdetect.a \
    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_calib3d.a \
    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_video.a \
    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_features2d.a \
    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_highgui.a \
     #$$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_androidcamera.a \
     $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_flann.a \
     $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_imgproc.a \
     $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_core.a     \
     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/liblibjpeg.a \
     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/liblibpng.a \
     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/liblibtiff.a \
     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/liblibjasper.a \
     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/libtbb.a

 LIBS +=$$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_java3.so

 LIBS += $$PWD/lib4_in_read.so

 LIBS += $$PWD/libg2d.so
}



SOURCES += main.cpp \
    mainwindow.cpp \
    bottomcamerawindows.cpp \
    g2d_yuyv_bgra.cpp \
    leftcamerawindows.cpp \
    rightcamerawindows.cpp \
    topcamerawindows.cpp \
    iconwidget.cpp \
    looptable.cpp \
    v4l2_work_fourin.cpp \
    g2d_thread.cpp \
    caleloopuptabledata.cpp \
    v4l2_work_left.cpp \
    v4l2_work_bottom.cpp \
    v4l2_work_top.cpp \
    v4l2_work_right.cpp

HEADERS  += mainwindow.h \
    bottomcamerawindows.h \
    g2d_yuyv_bgra.h \
    g2d.h \
    leftcamerawindows.h \
    rightcamerawindows.h \
    topcamerawindows.h \
    iconwidget.h \
    loopuptable.h \
    v4l2_work_fourin.h \
    g2d_thread.h \
    caleloopuptabledata.h \
    v4l2_work_left.h \
    v4l2_work_bottom.h \
    v4l2_work_top.h \
    v4l2_work_right.h

#CONFIG += mobility
#MOBILITY =

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        /home/zsh/android_env/OpenCV-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_java3.so \
        $$PWD/lib4_in_read.so \
        $$PWD/libg2d.so
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
   android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
    resource.qrc
