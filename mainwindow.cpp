#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "model.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("3D Viewer");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bu_openFile_clicked()
{
    // диалог выбора файла
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open 3D Model"),
        "",
        tr("3D Models (*.obj *.fbx *.dae *.3ds *.gltf *.glb)")
    );
    if(fileName.isEmpty()) return;
    ui->openGLWidget->openModel(fileName.toStdString());
}

