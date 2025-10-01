#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <vector>
#include "model.h"

struct Mesh {
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;      // новый
    int indexCount = 0;  // теперь будем считать количество индексов
};

struct Arrow {
    std::vector<QVector3D> vertices;  // вершины
    std::vector<QVector3D> normals;   // нормали
    QVector3D color;                  // цвет стрелки
    QMatrix4x4 rotation;              // ориентация стрелки
};


class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit MyOpenGLWidget(QWidget *parent = nullptr);
    ~MyOpenGLWidget();
    void openModel(std::string path);

    // Добавление модели на сцену
    void addModel(Model* model);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLShaderProgram *shaderProgram = nullptr;
    std::vector<Mesh> meshes;       // все меши на сцене
    std::vector<Model*> models;     // указатели на модели

    void setupShaders();
    Mesh createMeshFromModel(const Model& model); // функция создания Mesh из Model

private:
    // Arcball и панорамирование
    QQuaternion currentRotation;
    QVector3D translation = QVector3D(0,0,0);
    float zoom = -5.0f;

    QPoint lastMousePos;
    QPoint initialMousePos;
    bool leftButtonPressed = false;
    bool middleButtonPressed = false;

    QVector3D mapToSphere(const QPointF& pt);

    void drawTriad();
    Arrow createArrow(float length, float radius, float coneLength, int segments, QVector3D color);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;


};

#endif // MYOPENGLWIDGET_H
