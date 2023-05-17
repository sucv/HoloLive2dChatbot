QT       += core gui opengl widgets

# In Qt6 to use QOpenglWidget, we need add QT += openglwidgets.
greaterThan(QT_MAJOR_VERSION, 5){
    message(Building with Qt6 or Higher)
    QT += openglwidgets
}


CONFIG += c++17
TARGET = HoloBot


DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../Core/include
INCLUDEPATH += $$PWD/../Framework/src
INCLUDEPATH += $$PWD/QtLive2dWidget/thirdParty/stb

CONFIG += debug_and_release
linux-g++* {
    message(Compling with linux-g++)
    CONFIG(debug, debug|release){
        message(Debug build)
        LIBS += -L$$PWD/../Core/lib/linux/x86_64 -lFrameworkd
        LIBS += -L$$PWD/../Core/lib/linux/x86_64 -lLive2DCubismCore
        DEPENDPATH += $$PWD/../Core/dll/linux/x86_64
        release

    }

    CONFIG(release, debug|release){
        message(Release build)
        LIBS += -L$$PWD/../Core/lib/linux/x86_64 -lFramework
        LIBS += -L$$PWD/../Core/lib/linux/x86_64 -lLive2DCubismCore

    }

    LIBS += -L$$PWD/AzureSDK/lib/x64 -lMicrosoft.CognitiveServices.Speech.core
    LIBS += -L$$PWD/AzureSDK/lib/x64 -lMicrosoft.CognitiveServices.Speech.extension.audio.sys
    LIBS += -L$$PWD/AzureSDK/lib/x64 -lMicrosoft.CognitiveServices.Speech.extension.codec

    DEFINES += CSM_TARGET_LINUX_GL
}

win32-msvc*{
    message(Compling with win32-msvc)
    CONFIG(debug, debug|release){
        message(Debug build)
        LIBS += -L$$PWD/../Core/lib/windows/x86_64/142 -lLive2DCubismCore_MDd
        LIBS += -L$$PWD/../Core/lib/windows/x86_64/142 -lFrameworkd

    }

    CONFIG(release, debug|release){
        message(Release build)
        LIBS += -L$$PWD/../Core/lib/windows/x86_64/142 -lLive2DCubismCore_MD
        LIBS += -L$$PWD/../Core/lib/windows/x86_64/142 -lFramework

    }

    DEFINES += WIN32
    DEFINES += _WINDOWS
    DEFINES += CSM_TARGET_WIN_GL

    LIBS += -lOpenGL32
#    LIBS += -lUser32
#    LIBS += -lGlu32

    INCLUDEPATH += $$PWD/opencv/include
    DEFINES += CV_IGNORE_DEBUG_BUILD_GUARD=1
    LIBS +=-L$$PWD/opencv/x64/vc16/lib \
            -lopencv_core470        \
            -lopencv_highgui470     \
            -lopencv_imgproc470  \

    LIBS +=-L$$PWD/opencv/x64/vc16/bin \
            -lopencv_core470        \
            -lopencv_highgui470     \
            -lopencv_imgproc470  \
            # OpenAI

    INCLUDEPATH += $$PWD/curl/include

    LIBS += -L$$PWD/curl/bin -llibcurl
    LIBS += -L$$PWD/curl/lib -llibcurl
    DEFINES += CERT_DIR=\\\"$$PWD/curl/bin/cacert.pem\\\"


    INCLUDEPATH += \
        AzureSDK/include/cxx_api \
        AzureSDK/include/c_api
    LIBS += -L$$PWD/AzureSDK/lib/Windows/x64/Release \
        -lMicrosoft.CognitiveServices.Speech.core

    INCLUDEPATH += $$PWD/nlohmann
}

win32-g++*{
    error(Please use MSVC on Win32)
}



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += file_copies



HEADERS += \
    $$PWD/../Framework/src/Rendering/CubismRenderer.hpp \
    $$PWD/../Framework/src/Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp \
    $$PWD/../Framework/src/Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp \
    $$PWD/../Framework/src/Rendering/OpenGL/openglhelper.hpp \
    $$PWD/../Framework/src/Type/csmVector.hpp \
    QtLive2dWidget/src/LAppAllocator.hpp \
    QtLive2dWidget/src/LAppDefine.hpp \
    QtLive2dWidget/src/LAppDelegate.hpp \
    QtLive2dWidget/src/LAppGetModelFolders.hpp \
    QtLive2dWidget/src/LAppLive2DManager.hpp \
    QtLive2dWidget/src/LAppModel.hpp \
    QtLive2dWidget/src/LAppPal.hpp \
    QtLive2dWidget/src/LAppSprite.hpp \
    QtLive2dWidget/src/LAppTextureManager.hpp \
    QtLive2dWidget/src/LAppView.hpp \
    QtLive2dWidget/src/LAppWavFileHandler.hpp \
    QtLive2dWidget/src/QLive2dWidget.hpp \
    QtLive2dWidget/src/TouchManager.hpp \
    QtLive2dWidget/src/openglhelper.hpp \
    editdialog.h \
    hologram.hpp \
    listviewdelegate.h \
    speechapi_cxx.h \
    OpenAI/openai.hpp \
    communicate.h \
    mainwindow.h

SOURCES += \
    $$PWD/../Framework/src/Rendering/CubismRenderer.cpp \
    $$PWD/../Framework/src/Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.cpp \
    $$PWD/../Framework/src/Rendering/OpenGL/CubismRenderer_OpenGLES2.cpp \
    QtLive2dWidget/src/LAppAllocator.cpp \
    QtLive2dWidget/src/LAppDefine.cpp \
    QtLive2dWidget/src/LAppDelegate.cpp \
    QtLive2dWidget/src/LAppLive2DManager.cpp \
    QtLive2dWidget/src/LAppModel.cpp \
    QtLive2dWidget/src/LAppPal.cpp \
    QtLive2dWidget/src/LAppSprite.cpp \
    QtLive2dWidget/src/LAppTextureManager.cpp \
    QtLive2dWidget/src/LAppView.cpp \
    QtLive2dWidget/src/LAppWavFileHandler.cpp \
    QtLive2dWidget/src/QLive2dWidget.cpp \
    QtLive2dWidget/src/TouchManager.cpp \
    editdialog.cpp \
    hologram.cpp \
    listviewdelegate.cpp \
    main.cpp \
    communicate.cpp \
    mainwindow.cpp

DISTFILES += \
    Resources/chat.png \
    Resources/gear.png \
    Resources/hologram.png \
    Resources/minimize.png \
    Resources/microphone.png \
    Resources/switch.png \
    Resources/restart.png \
    Resources/help.png \
    Resources/help.html

resources.files = $$PWD/Resources
resources.path = $$OUT_PWD

COPIES += resources
