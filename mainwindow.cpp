#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "registro.h"

#include <QMessageBox>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnRegistrar_clicked() {
    QString nombre = ui->txtNombre->text();
    QString usuario = ui->txtUsuario->text();

    if (registro.guardarDatos(nombre, usuario)) {
        QMessageBox::information(this, "Registro", "Datos guardados correctamente.");
    } else {
        QMessageBox::warning(this, "Error", "No se pudo guardar el registro.");
    }
}


void MainWindow::on_btnNivel1_clicked()
{
    game = new niveles(1, this);
    game->show();
}


void MainWindow::on_btnNivel2_clicked() {
    QMessageBox::information(this, "Nivel 2", "Iniciando Nivel 2...");
    // game = new niveles(2, this);
    // game->show();
}

void MainWindow::on_btnNivel3_clicked() {
    QMessageBox::information(this, "Nivel 3", "Iniciando Nivel 3...");
    // game = new niveles(3, this);
    // game->show();
}
