TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += .
INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/../Core/include
QT       += core opengl

linux-g++* {
    CONFIG(debug, debug|release){
        message(Debug build)
        TARGET = Frameworkd
        DESTDIR = $$PWD/../Core/lib/linux/x86_64
        LIBS += -L$$PWD/../Core/lib/linux/x86_64 -lLive2DCubismCore
        DEPENDPATH += $$PWD/../Core/dll/linux/x86_64
        release
    }

    CONFIG(release, debug|release){
        message(Release build)
        TARGET = Framework
        DESTDIR = $$PWD/../Core/lib/linux/x86_64
        LIBS += -L$$PWD/../Core/lib/linux/x86_64 -lLive2DCubismCore
    }
    DEFINES += CSM_TARGET_LINUX_GL
}

win32-msvc*{
    CONFIG(debug, debug|release){
        message(Debug build)
        TARGET = Frameworkd
        DESTDIR = $$PWD/../Core/lib/windows/x86_64/142
        LIBS += -L$$PWD/../Core/lib/windows/x86_64/142 -lLive2DCubismCore_MDd

        release

    }

    CONFIG(release, debug|release){
        message(Release build)
        TARGET = Framework
        DESTDIR = $$PWD/../Core/lib/windows/x86_64/142
        LIBS += -L$$PWD/../Core/lib/windows/x86_64/142 -lLive2DCubismCore_MD

    }

    DEFINES += WIN32
    DEFINES += _WINDOWS
    DEFINES += CSM_TARGET_WIN_GL
}

win32-g++*{
    error(Please use MSVC on Win32)
}

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += src/CubismCdiJson.hpp \
           src/CubismDefaultParameterId.hpp \
           src/CubismFramework.hpp \
           src/CubismFrameworkConfig.hpp \
           src/CubismJsonHolder.hpp \
           src/CubismModelSettingJson.hpp \
           src/ICubismAllocator.hpp \
           src/ICubismModelSetting.hpp \
           src/Live2DCubismCore.hpp \
           src/Effect/CubismBreath.hpp \
           src/Effect/CubismEyeBlink.hpp \
           src/Effect/CubismPose.hpp \
           src/Id/CubismId.hpp \
           src/Id/CubismIdManager.hpp \
           src/Math/CubismMath.hpp \
           src/Math/CubismMatrix44.hpp \
           src/Math/CubismModelMatrix.hpp \
           src/Math/CubismTargetPoint.hpp \
           src/Math/CubismVector2.hpp \
           src/Math/CubismViewMatrix.hpp \
           src/Model/CubismMoc.hpp \
           src/Model/CubismModel.hpp \
           src/Model/CubismModelUserData.hpp \
           src/Model/CubismModelUserDataJson.hpp \
           src/Model/CubismUserModel.hpp \
           src/Motion/ACubismMotion.hpp \
           src/Motion/CubismExpressionMotion.hpp \
           src/Motion/CubismMotion.hpp \
           src/Motion/CubismMotionInternal.hpp \
           src/Motion/CubismMotionJson.hpp \
           src/Motion/CubismMotionManager.hpp \
           src/Motion/CubismMotionQueueEntry.hpp \
           src/Motion/CubismMotionQueueManager.hpp \
           src/Physics/CubismPhysics.hpp \
           src/Physics/CubismPhysicsInternal.hpp \
           src/Physics/CubismPhysicsJson.hpp \
           src/Rendering/CubismRenderer.hpp \
           src/Rendering/OpenGL/openglhelper.hpp \
           src/Type/csmMap.hpp \
           src/Type/csmRectF.hpp \
           src/Type/csmString.hpp \
           src/Type/csmVector.hpp \
           src/Type/CubismBasicType.hpp \
           src/Utils/CubismDebug.hpp \
           src/Utils/CubismJson.hpp \
           src/Utils/CubismString.hpp \
           src/Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp \
           src/Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp \

SOURCES += src/CubismCdiJson.cpp \
           src/CubismDefaultParameterId.cpp \
           src/CubismFramework.cpp \
           src/CubismModelSettingJson.cpp \
           src/Effect/CubismBreath.cpp \
           src/Effect/CubismEyeBlink.cpp \
           src/Effect/CubismPose.cpp \
           src/Id/CubismId.cpp \
           src/Id/CubismIdManager.cpp \
           src/Math/CubismMath.cpp \
           src/Math/CubismMatrix44.cpp \
           src/Math/CubismModelMatrix.cpp \
           src/Math/CubismTargetPoint.cpp \
           src/Math/CubismVector2.cpp \
           src/Math/CubismViewMatrix.cpp \
           src/Model/CubismMoc.cpp \
           src/Model/CubismModel.cpp \
           src/Model/CubismModelUserData.cpp \
           src/Model/CubismModelUserDataJson.cpp \
           src/Model/CubismUserModel.cpp \
           src/Motion/ACubismMotion.cpp \
           src/Motion/CubismExpressionMotion.cpp \
           src/Motion/CubismMotion.cpp \
           src/Motion/CubismMotionJson.cpp \
           src/Motion/CubismMotionManager.cpp \
           src/Motion/CubismMotionQueueEntry.cpp \
           src/Motion/CubismMotionQueueManager.cpp \
           src/Physics/CubismPhysics.cpp \
           src/Physics/CubismPhysicsJson.cpp \
           src/Rendering/CubismRenderer.cpp \
           src/Type/csmRectF.cpp \
           src/Type/csmString.cpp \
           src/Utils/CubismDebug.cpp \
           src/Utils/CubismJson.cpp \
           src/Utils/CubismString.cpp \
           src/Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.cpp \
           src/Rendering/OpenGL/CubismRenderer_OpenGLES2.cpp
