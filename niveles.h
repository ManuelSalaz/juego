#ifndef NIVELES_H
#define NIVELES_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>

#include "personaje.h"

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
    bool suelo;
};

#endif // NIVELES_H
