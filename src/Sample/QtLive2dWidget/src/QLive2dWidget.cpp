#include "QLive2dWidget.hpp"
#include <QApplication>
#include <QTimer>
#include "openglhelper.hpp"
#include "LAppDelegate.hpp"
#include "LAppModel.hpp"
#include "LAppLive2DManager.hpp"


#ifdef WIN32
#include <Windows.h>
#endif


QLive2dWidget::QLive2dWidget(QWidget *parent):
    QOpenGLWidget(parent)
{
    this->setAttribute(Qt::WA_AlwaysStackOnTop);
    this->setAttribute(Qt::WA_TranslucentBackground);

    elapsedTimer.start();

    //Add the render into Timer to make the animation
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &QLive2dWidget::updateMotions);
    timer->start();

    // the OpenGL in OpenGLWidget won't scale by HDPI setting, we need to scale manually.
    ratio = parent->devicePixelRatio();
}

void QLive2dWidget::initializeGL()
{
    this->makeCurrent();
    OpenGLHelper::init(this->context());

    LAppDelegate::GetInstance()->Initialize(this);
    LAppDelegate::GetInstance()->Run();

//    OpenGLHelper::get()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    OpenGLHelper::get()->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    OpenGLHelper::get()->glEnable(GL_BLEND);
//    OpenGLHelper::get()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
void QLive2dWidget::resizeGL(int width, int height)
{
    LAppDelegate::GetInstance()->Resize(width*ratio,height*ratio);
}

void QLive2dWidget::paintGL()
{
    LAppDelegate::GetInstance()->Run();

}

void QLive2dWidget::clear()
{
    OpenGLHelper::get()->glViewport(0, 0, width()*ratio, height()*ratio);
    OpenGLHelper::get()->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    OpenGLHelper::get()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OpenGLHelper::get()->glClearDepthf(1.0);
}


void QLive2dWidget::updateMotions()
{
    update();
}

void QLive2dWidget::react(std::string expression, std::string motion)
{

    qDebug() << "expression" << expression << "\n";
    qDebug() << "motion" << motion << "\n";
    LAppModel* currentModel = LAppLive2DManager::GetInstance()->GetModel(0);
    LAppLive2DManager::GetInstance()->StartMotionAndExpression(currentModel, expression, motion);
}

void QLive2dWidget::mouseMoveEvent(QMouseEvent * event){
    if (event->buttons() & Qt::MiddleButton) {
        int dx = event->globalPosition().x() - mouseLastPos.x();
        int dy = event->globalPosition().y() - mouseLastPos.y();
        mouseLastPos = event->globalPosition();

        if (QWidget* topLevelWidget = window()) {
            topLevelWidget->move(topLevelWidget->pos() + QPoint(dx, dy));
        }

    }
    else if (event->buttons() & Qt::LeftButton) {
        LAppDelegate::GetInstance()->mouseMoveEvent(event);
    }

}

void QLive2dWidget::mousePressEvent(QMouseEvent * event){
    if (event->button() == Qt::MiddleButton) {
        mouseLastPos = event->globalPosition();
        qDebug() << "MiddleButton!"<< mouseLastPos;
    }
    else if (event->button() == Qt::LeftButton) {
        LAppDelegate::GetInstance()->mousePressEvent(event);
    }
}

void QLive2dWidget::mouseReleaseEvent(QMouseEvent * event){
    LAppDelegate::GetInstance()->mouseReleaseEvent(event);
}

void QLive2dWidget::wheelEvent(QWheelEvent* event)
{
    // Get the angle delta of the scroll event
    int delta = event->angleDelta().y();

    // Modify the size of the widget based on the scroll delta
    QSize currentSize = size();
    int newWidth = currentSize.width() + delta;
    int newHeight = currentSize.height() + delta;
    parentWidget()->parentWidget()->resize(newWidth, newHeight);

    // Accept the event to prevent it from being propagated further
    event->accept();
}
