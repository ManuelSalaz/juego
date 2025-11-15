#include "enemigos.h"

#include <QBrush>
#include <QColor>
#include <QLineF>
#include <QPen>
#include <QPixmap>
#include <QtGlobal>

enemigos::enemigos(QObject *parent)
    : QObject(parent), velX(0.0f), velY(0.0f), radioVision(160.0), objetivoEnVision(false), areaVision(nullptr) {
    // Sprite provisional
    setPixmap(QPixmap(":/sprites/enemigo.png").scaled(40, 40));
    setData(0, "enemigo_centinela");

    // Área de visión visible (centinela)
    areaVision = new QGraphicsEllipseItem(-radioVision, -radioVision, radioVision * 2, radioVision * 2, this);
    areaVision->setBrush(QColor(255, 215, 0, 25));
    areaVision->setPen(QPen(QColor(255, 215, 0, 160), 2, Qt::DashLine));
    areaVision->setZValue(-1);  // que no tape al sprite
}

void enemigos::mover() {
    // Movimiento lineal simple (por ahora los centinelas se mantienen quietos)
    if (!qFuzzyIsNull(velX) || !qFuzzyIsNull(velY))
        setPos(x() + velX, y() + velY);
}

void enemigos::actualizarVision(const QPointF &objetivo) {
    if (!areaVision)
        return;

    QLineF distancia(mapToScene(0, 0), objetivo);
    bool detectado = distancia.length() <= radioVision;

    if (detectado != objetivoEnVision) {
        objetivoEnVision = detectado;
        QColor relleno = detectado ? QColor(255, 69, 0, 80) : QColor(255, 215, 0, 25);
        QColor borde = detectado ? QColor(220, 20, 60, 200) : QColor(255, 215, 0, 160);
        areaVision->setBrush(relleno);
        areaVision->setPen(QPen(borde, 2, Qt::DashLine));
    }
}

bool enemigos::jugadorDetectado() const {
    return objetivoEnVision;
}

qreal enemigos::rangoVision() const {
    return radioVision;
}
