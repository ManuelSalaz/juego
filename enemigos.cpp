#include "enemigos.h"

enemigos::enemigos(QObject *parent)
    : QObject(parent) {
    // Sprite provisional
    setPixmap(QPixmap(":/sprites/enemigo.png").scaled(40, 40));

    velX = -2.0f;   // se mueve hacia la izquierda
    velY = 0.0f;
}

void enemigos::mover() {
    // Movimiento lineal simple
    setPos(x() + velX, y() + velY);
}
