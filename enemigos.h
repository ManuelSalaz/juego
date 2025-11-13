#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include <QGraphicsPixmapItem>
#include <QObject>

class enemigos : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit enemigos(QObject *parent = nullptr);

    // Movimiento muy simple por ahora
    void mover();

private:
    float velX;
    float velY;
};

#endif // ENEMIGOS_H
