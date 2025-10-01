#include "MyOpenGLWidget.h"
#include <QMatrix4x4>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) {

}

MyOpenGLWidget::~MyOpenGLWidget()
{
    makeCurrent();
    delete shaderProgram;
    doneCurrent();
}

void MyOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.9f, 0.9f, 0.6f, 0.1f); // Пастельный голубой цвет

    shaderProgram = new QOpenGLShaderProgram(this);

    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex_shader.vert"))
        qDebug() << "Vertex shader error:" << shaderProgram->log();

    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment_shader.frag"))
        qDebug() << "Fragment shader error:" << shaderProgram->log();

    if (!shaderProgram->link())
        qDebug() << "Shader link error:" << shaderProgram->log();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void MyOpenGLWidget::paintGL()
{

}
