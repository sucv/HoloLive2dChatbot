#ifndef OPENGLHELPER_H
#define OPENGLHELPER_H
#include <QOpenGLFunctions>

class OpenGLHelper{
public:
    inline static QOpenGLFunctions* get(){
        return instance;
    }
    inline static void release(){
        if(instance!=nullptr)
            delete instance;
        instance=nullptr;
    }
    inline static void init(QOpenGLContext * context)
    {
        if(instance==nullptr)
        {
            instance=new QOpenGLFunctions(context);
            instance->initializeOpenGLFunctions();
        }
    }
private:
    inline OpenGLHelper(){}
    inline ~OpenGLHelper(){}
    inline static QOpenGLFunctions* instance=nullptr;
};

#endif // OPENGLHELPER_H
