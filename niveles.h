#ifndef NIVELES_H
#define NIVELES_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QVector>

#include "personaje.h"
#include "enemigos.h"

class niveles : public QGraphicsView
{
    Q_OBJECT

public:
    explicit niveles(int numNivel = 1, QWidget *parent = nullptr);

protected:
    // Para capturar el teclado en este nivel
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void actualizarEscena();   // si luego quieres mover enemigos, etc.

private:
    QGraphicsScene *scene;
    personaje *player;
    QTimer *timerUpdate;
    int nivelActual;
    QVector<enemigos*> centinelas;

    void configurarEscenaBase();
    void crearPlataformas();
    void generarCentinelas();
};

#endif // NIVELES_H
