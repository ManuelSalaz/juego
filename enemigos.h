#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPointF>

class enemigos : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit enemigos(QObject *parent = nullptr);

    // Movimiento muy simple por ahora
    void mover();

    // Centinela: actualiza el estado de detección según la posición del objetivo
    void actualizarVision(const QPointF &objetivo);
    bool jugadorDetectado() const;
    qreal rangoVision() const;

private:
    float velX;
    float velY;
    qreal radioVision;
    bool objetivoEnVision;
    QGraphicsEllipseItem *areaVision;
};

#endif // ENEMIGOS_H
