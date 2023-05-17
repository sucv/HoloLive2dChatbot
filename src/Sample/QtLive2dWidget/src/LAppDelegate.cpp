/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"
#include <iostream>
#include <QOpenGLFunctions>
#include "QLive2dWidget.hpp"
#include "openglhelper.hpp"
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"

using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
    LAppDelegate* s_instance = NULL;
}

LAppDelegate* LAppDelegate::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppDelegate();
    }

    return s_instance;
}

void LAppDelegate::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

bool LAppDelegate::Initialize(QLive2dWidget * widget)
{
    if (DebugLogEnable)
    {
        LAppPal::PrintLog("START");
    }

    _window = widget;

    //テクスチャサンプリング設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //透過設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    //コールバック関数の登録
//    glfwSetMouseButtonCallback(_window, EventHandler::OnMouseCallBack);
//    glfwSetCursorPosCallback(_window, EventHandler::OnMouseCallBack);

    // ウィンドウサイズ記憶
    _windowWidth = widget->width()*QLive2dWidget::ratio;
    _windowHeight = widget->height()*QLive2dWidget::ratio;

    //AppViewの初期化
    _view->Initialize();

    // Cubism SDK の初期化
    InitializeCubism();

    initialized=true;

    return GL_TRUE;
}

void LAppDelegate::Release()
{

    delete _textureManager;
    delete _view;

    // リソースを解放
    LAppLive2DManager::ReleaseInstance();

    //Cubism SDK の解放
    CubismFramework::Dispose();
}

void LAppDelegate::Run()
{
    if(!initialized)return;

    _window->clear();
    // 時間更新
    LAppPal::UpdateTime();
    //描画更新
    _view->Render();
}

void LAppDelegate::Resize(int w, int h)
{
    _windowWidth = w;
    _windowHeight = h;

    //AppViewの初期化
    _view->Initialize();
    // スプライトサイズを再設定
    _view->ResizeSprite();
    // ビューポート変更
    OpenGLHelper::get()->glViewport(0, 0, _windowWidth, _windowHeight);
}

LAppDelegate::LAppDelegate():
    _cubismOption(),
    _window(NULL),
    _captured(false),
    _mouseX(0.0f),
    _mouseY(0.0f),
    _isEnd(false),
    _windowWidth(0),
    _windowHeight(0)
{
    _view = new LAppView();
    _textureManager = new LAppTextureManager();
}

LAppDelegate::~LAppDelegate()
{

}

void LAppDelegate::InitializeCubism()
{
    //setup cubism
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

//    //load model
//    LAppLive2DManager::GetInstance();

    //default proj
    CubismMatrix44 projection;

    LAppPal::UpdateTime();

    _view->InitializeSprite();
}

void LAppDelegate::mouseMoveEvent(QMouseEvent * event){
    _mouseX = static_cast<float>(event->pos().x()*QLive2dWidget::ratio);
    _mouseY = static_cast<float>(event->pos().y()*QLive2dWidget::ratio);
//    _mouseX= static_cast<float>(_mouseX*2.0/_screenWidth-1.0);
//    _mouseY= static_cast<float>(1.0-_mouseY*2.0/_screenHeight);
        if (!_captured)
        {
            return;
        }
        if (_view == NULL)
        {
            return;
        }

    _view->OnTouchesMoved(_mouseX, _mouseY);
}

void LAppDelegate::mousePressEvent(QMouseEvent * event){
    _mouseX = static_cast<float>(event->pos().x()*QLive2dWidget::ratio);
    _mouseY = static_cast<float>(event->pos().y()*QLive2dWidget::ratio);
//    _mouseX= static_cast<float>(_mouseX*2.0/_screenWidth-1.0);
//    _mouseY= static_cast<float>(1.0-_mouseY*2.0/_screenHeight);

        if (_view == NULL)
        {
            return;
        }



            _captured = true;
            _view->OnTouchesBegan(_mouseX, _mouseY);


}
void LAppDelegate::mouseReleaseEvent(QMouseEvent * event){
    _mouseX = static_cast<float>(event->pos().x()*QLive2dWidget::ratio);
    _mouseY = static_cast<float>(event->pos().y()*QLive2dWidget::ratio);
//    _mouseX= static_cast<float>(_mouseX*2.0/_screenWidth-1.0);
//    _mouseY= static_cast<float>(1.0-_mouseY*2.0/_screenHeight);
        if (_view == NULL)
        {
            return;
        }

        if (_captured)
        {
            _captured = false;
            _view->OnTouchesEnded(_mouseX, _mouseY);
        }


}
//void LAppDelegate::OnMouseCallBack(GLFWwindow* window, int button, int action, int modify)
//{
//    if (_view == NULL)
//    {
//        return;
//    }
//    if (GLFW_MOUSE_BUTTON_LEFT != button)
//    {
//        return;
//    }

//    if (GLFW_PRESS == action)
//    {
//        _captured = true;
//        _view->OnTouchesBegan(_mouseX, _mouseY);
//    }
//    else if (GLFW_RELEASE == action)
//    {
//        if (_captured)
//        {
//            _captured = false;
//            _view->OnTouchesEnded(_mouseX, _mouseY);
//        }
//    }
//}

//void LAppDelegate::OnMouseCallBack(GLFWwindow* window, double x, double y)
//{
//    _mouseX = static_cast<float>(x);
//    _mouseY = static_cast<float>(y);

//    if (!_captured)
//    {
//        return;
//    }
//    if (_view == NULL)
//    {
//        return;
//    }

//    _view->OnTouchesMoved(_mouseX, _mouseY);
//}

GLuint LAppDelegate::CreateShader()
{
    //バーテックスシェーダのコンパイル
    GLuint vertexShaderId = OpenGLHelper::get()->glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShader =
        "#version 120\n"
        "attribute vec3 position;"
        "attribute vec2 uv;"
        "varying vec2 vuv;"
        "void main(void){"
        "    gl_Position = vec4(position, 1.0);"
        "    vuv = uv;"
        "}";
    OpenGLHelper::get()->glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
    OpenGLHelper::get()->glCompileShader(vertexShaderId);
    if(!CheckShader(vertexShaderId))
    {
        return 0;
    }

    //フラグメントシェーダのコンパイル
    GLuint fragmentShaderId = OpenGLHelper::get()->glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShader =
        "#version 120\n"
        "varying vec2 vuv;"
        "uniform sampler2D texture;"
        "uniform vec4 baseColor;"
        "void main(void){"
        "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
        "}";
    OpenGLHelper::get()->glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
    OpenGLHelper::get()->glCompileShader(fragmentShaderId);
    if (!CheckShader(fragmentShaderId))
    {
        return 0;
    }

    //プログラムオブジェクトの作成
    GLuint programId = OpenGLHelper::get()->glCreateProgram();
    OpenGLHelper::get()->glAttachShader(programId, vertexShaderId);
    OpenGLHelper::get()->glAttachShader(programId, fragmentShaderId);

    // リンク
    OpenGLHelper::get()->glLinkProgram(programId);

    OpenGLHelper::get()->glUseProgram(programId);

    return programId;
}

bool LAppDelegate::CheckShader(GLuint shaderId)
{
    GLint status;
    GLint logLength;
    OpenGLHelper::get()->glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar* log = reinterpret_cast<GLchar*>(CSM_MALLOC(logLength));
        OpenGLHelper::get()->glGetShaderInfoLog(shaderId, logLength, &logLength, log);
        CubismLogError("Shader compile log: %s", log);
        CSM_FREE(log);
    }

    OpenGLHelper::get()->glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        OpenGLHelper::get()->glDeleteShader(shaderId);
        return false;
    }

    return true;
}
