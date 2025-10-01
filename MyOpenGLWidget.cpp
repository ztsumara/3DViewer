#include "MyOpenGLWidget.h"
#include <QDebug>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QtMath>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

MyOpenGLWidget::~MyOpenGLWidget()
{
    makeCurrent();

    for (auto& mesh : meshes) {
        if (mesh.VAO) glDeleteVertexArrays(1, &mesh.VAO);
        if (mesh.VBO) glDeleteBuffers(1, &mesh.VBO);
    }

    delete shaderProgram;

    doneCurrent();
}

void MyOpenGLWidget::openModel(std::string path)
{
    makeCurrent();
    for (auto &mesh : meshes) {
        if(mesh.VAO) glDeleteVertexArrays(1, &mesh.VAO);
        if(mesh.VBO) glDeleteBuffers(1, &mesh.VBO);
    }
    meshes.clear();

    for (auto m : models) delete m;
    models.clear();

    Model* model = new Model();
    if(model->loadModel(path)) {
        addModel(model);
    } else {
        delete model;
    }
    doneCurrent();
}

// Добавление модели на сцену
void MyOpenGLWidget::addModel(Model* model)
{
    if (!model) return;
    models.push_back(model);
    Mesh mesh = createMeshFromModel(*model);
    meshes.push_back(mesh);
    update();
}

Mesh MyOpenGLWidget::createMeshFromModel(const Model& model)
{
    Mesh mesh;
    mesh.indexCount = model.indices.size();

    std::vector<float> data;
    for (const auto& v : model.vertices) {
        data.push_back(v.position.x());
        data.push_back(v.position.y());
        data.push_back(v.position.z());
        data.push_back(v.normal.x());
        data.push_back(v.normal.y());
        data.push_back(v.normal.z());
    }

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indexCount*sizeof(unsigned int),
                 model.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return mesh;
}

void MyOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    setupShaders();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void MyOpenGLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(!shaderProgram) return;
    shaderProgram->bind();

    QMatrix4x4 view;
    view.translate(0,0,zoom);

    QMatrix4x4 projection;
    projection.perspective(45.0f,float(width())/float(height()),0.1f,100.0f);

    QMatrix4x4 modelMatrix;
    modelMatrix.translate(translation);
    modelMatrix.rotate(currentRotation);

    for(size_t i=0;i<meshes.size();++i){
        QMatrix4x4 finalModel = modelMatrix * models[i]->modelMatrix;
        shaderProgram->setUniformValue("model", finalModel);
        shaderProgram->setUniformValue("view", view);
        shaderProgram->setUniformValue("projection", projection);
        shaderProgram->setUniformValue("lightPosKey", QVector3D(3.0f, 3.0f, 3.0f));
        shaderProgram->setUniformValue("lightPosFill", QVector3D(-3.0f, 2.0f, 1.0f));
        shaderProgram->setUniformValue("viewPos", QVector3D(0,0,-zoom));
        shaderProgram->setUniformValue("objectColor", QVector3D(0.7f,0.7f,0.7f));

        glBindVertexArray(meshes[i].VAO);
        glDrawElements(GL_TRIANGLES, meshes[i].indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    shaderProgram->release();
    glDepthRange(0.0, 0.01);
    drawTriad();
    glDepthRange(0.0, 1.0);
}

void MyOpenGLWidget::setupShaders(){
    shaderProgram = new QOpenGLShaderProgram(this);

    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex_shader.vert"))
        qDebug() << "Vertex shader error:" << shaderProgram->log();

    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment_shader.frag"))
        qDebug() << "Fragment shader error:" << shaderProgram->log();

    if (!shaderProgram->link())
        qDebug() << "Shader link error:" << shaderProgram->log();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent* event){
    lastMousePos = event->pos();
    if(event->button() == Qt::LeftButton) {
        leftButtonPressed = true;
        initialMousePos = event->pos();
    }
    if(event->button() == Qt::MiddleButton) {
        middleButtonPressed = true;
        setCursor(Qt::SizeAllCursor);
    }
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent* event){
    if(event->button() == Qt::LeftButton) {
        leftButtonPressed = false;
        QCursor::setPos(mapToGlobal(initialMousePos));
        unsetCursor();
    }
    if(event->button() == Qt::MiddleButton) {
        middleButtonPressed = false;
        unsetCursor();
    }
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent* event){
    if(leftButtonPressed){
        setCursor(Qt::BlankCursor);
        QPoint mousePos = initialMousePos;
        QPoint delta = event->pos() - mousePos;

        float rotateSpeed = 0.3f;
        QQuaternion qx = QQuaternion::fromAxisAndAngle(1,0,0, delta.y() * rotateSpeed);
        QQuaternion qy = QQuaternion::fromAxisAndAngle(0,1,0, delta.x() * rotateSpeed);
        currentRotation = qy * qx * currentRotation;

        // возвращаем курсор в исходное положение
        QCursor::setPos(mapToGlobal(initialMousePos));
    }
    if(middleButtonPressed){
        QPoint delta = event->pos() - lastMousePos;
        float panSpeed = 0.01f;
        translation += QVector3D(delta.x()*panSpeed, -delta.y()*panSpeed, 0);
    }

    lastMousePos = event->pos();
    update();
}

void MyOpenGLWidget::wheelEvent(QWheelEvent* event){
    float zoomSpeed = 0.5f;
    zoom += event->angleDelta().y() * zoomSpeed / 120.0f;
    if(zoom>-1.0f) zoom=-1.0f;
    update();
}

void MyOpenGLWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MiddleButton) // колесо мыши
    {
        if(models.empty()) return;
        translation = -models[0]->computeModelCenter();
        zoom = -5.0f;
        update();
    }
}
Arrow MyOpenGLWidget::createArrow(float length, float radius, float coneLength, int segments, QVector3D color)
{
    Arrow arrow;
    arrow.color = color;
    arrow.rotation.setToIdentity();

    float cylinderLength = length - coneLength;
    float coneRadius = radius * 2.0f; // радиус основания конуса больше цилиндра

    // Цилиндр
    for (int i = 0; i < segments; ++i)
    {
        float theta1 = float(i) / segments * 2.0f * M_PI;
        float theta2 = float(i + 1) / segments * 2.0f * M_PI;

        QVector3D p0(0, radius * cos(theta1), radius * sin(theta1));
        QVector3D p1(cylinderLength, radius * cos(theta1), radius * sin(theta1));
        QVector3D p2(0, radius * cos(theta2), radius * sin(theta2));
        QVector3D p3(cylinderLength, radius * cos(theta2), radius * sin(theta2));

        QVector3D n0(0, cos(theta1), sin(theta1));
        QVector3D n1(0, cos(theta1), sin(theta1));
        QVector3D n2(0, cos(theta2), sin(theta2));
        QVector3D n3(0, cos(theta2), sin(theta2));

        arrow.vertices.push_back(p0);
        arrow.vertices.push_back(p1);
        arrow.vertices.push_back(p2);
        arrow.normals.push_back(n0);
        arrow.normals.push_back(n1);
        arrow.normals.push_back(n2);

        arrow.vertices.push_back(p1);
        arrow.vertices.push_back(p3);
        arrow.vertices.push_back(p2);
        arrow.normals.push_back(n1);
        arrow.normals.push_back(n3);
        arrow.normals.push_back(n2);
    }

    // Конус
    QVector3D tip(length, 0, 0); // вершина конуса

    for (int i = 0; i < segments; ++i)
    {
        float theta1 = float(i) / segments * 2.0f * M_PI;
        float theta2 = float(i + 1) / segments * 2.0f * M_PI;

        QVector3D base1(cylinderLength, coneRadius * cos(theta1), coneRadius * sin(theta1));
        QVector3D base2(cylinderLength, coneRadius * cos(theta2), coneRadius * sin(theta2));

        auto computeNormal = [&](const QVector3D& basePoint) -> QVector3D {
            QVector3D dir = basePoint - tip;        // от вершины к основанию
            dir.setX(0);
            QVector3D normal(coneLength, dir.y(), dir.z());
            return normal.normalized();
        };

        QVector3D n1 = computeNormal(base1);
        QVector3D n2 = computeNormal(tip);
        QVector3D n3 = computeNormal(base2);

        arrow.vertices.push_back(base1);
        arrow.vertices.push_back(tip);
        arrow.vertices.push_back(base2);
        arrow.normals.push_back(n1);
        arrow.normals.push_back(n2);
        arrow.normals.push_back(n3);
    }

    return arrow;
}
void MyOpenGLWidget::drawTriad()
{
    if (!shaderProgram) return;

    glEnable(GL_DEPTH_TEST);

    int size = 200;
    glViewport(width() - size + 120, 20, size, size);

    // Перспективная проекция
    QMatrix4x4 coordProj;
    coordProj.perspective(45.0f, 1.0f, 0.1f, 10.0f);

    // Камера мини-сцены
    QMatrix4x4 coordView;
    coordView.translate(0, 0, -3);

    // Модельная матрица триады
    QMatrix4x4 coordModel;
    coordModel.rotate(currentRotation);

    // Масштаб стрелок
    QMatrix4x4 scaleMatrix;
    scaleMatrix.scale(0.5f);


    std::vector<Arrow> arrows;


    Arrow xArrow = createArrow(2.0f, 0.1f, 0.7f, 100, QVector3D(1,0,0));
    xArrow.rotation.setToIdentity();
    arrows.push_back(xArrow);


    Arrow yArrow = createArrow(2.0f, 0.1f, 0.7f, 100, QVector3D(0,1,0));
    QMatrix4x4 rotY; rotY.rotate(90, 0, 0, 1);
    yArrow.rotation = rotY;
    arrows.push_back(yArrow);


    Arrow zArrow = createArrow(2.0f, 0.1f, 0.7f, 100, QVector3D(0,0,1));
    QMatrix4x4 rotZ; rotZ.rotate(-90, 0, 1, 0);
    zArrow.rotation = rotZ;
    arrows.push_back(zArrow);

    // Рисуем стрелки
    for(auto &arrow : arrows)
    {
        shaderProgram->bind();
        shaderProgram->setUniformValue("model", coordModel * arrow.rotation * scaleMatrix);
        shaderProgram->setUniformValue("view", coordView);
        shaderProgram->setUniformValue("projection", coordProj);
        shaderProgram->setUniformValue("lightPosKey", QVector3D(3.0f, 3.0f, 3.0f));
        shaderProgram->setUniformValue("lightPosFill", QVector3D(-3.0f, 2.0f, 1.0f));
        shaderProgram->setUniformValue("viewPos", QVector3D(0,0,3));
        shaderProgram->setUniformValue("objectColor", arrow.color);

        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        std::vector<float> data;
        for (size_t i = 0; i < arrow.vertices.size(); ++i) {
            data.push_back(arrow.vertices[i].x());
            data.push_back(arrow.vertices[i].y());
            data.push_back(arrow.vertices[i].z());
            data.push_back(arrow.normals[i].x());
            data.push_back(arrow.normals[i].y());
            data.push_back(arrow.normals[i].z());
        }

        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0); // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float))); // normal
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, arrow.vertices.size());

        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        shaderProgram->release();
    }

    // Восстанавливаем основной viewport
    glViewport(0, 0, width(), height());
}




