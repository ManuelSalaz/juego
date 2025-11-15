#include "enemigos.h"

#include <QBrush>
#include <QColor>
#include <QLineF>
#include <QPen>
#include <QPainter>
#include <QPixmap>
#include <QRadialGradient>
#include <QtGlobal>

enemigos::enemigos(QObject *parent)
    : QObject(parent), velX(0.0f), velY(0.0f), radioVision(160.0), objetivoEnVision(false), areaVision(nullptr) {
    setData(0, "enemigo_centinela");

    spriteReposo = crearSpriteCentinela(QColor(255, 180, 0, 245), QColor(120, 60, 0, 255), QColor(35, 40, 45, 230),
                                        QColor(120, 220, 255, 220));
    spriteAlerta = crearSpriteCentinela(QColor(255, 100, 60, 250), QColor(120, 30, 0, 255), QColor(120, 10, 10, 230),
                                       QColor(255, 240, 180, 230));
    aplicarSprite(spriteReposo);

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

    if (detectado == objetivoEnVision)
        return;

    objetivoEnVision = detectado;

    const QColor relleno = detectado ? QColor(255, 69, 0, 80) : QColor(255, 215, 0, 25);
    const QColor borde = detectado ? QColor(220, 20, 60, 200) : QColor(255, 215, 0, 160);
    areaVision->setBrush(relleno);
    areaVision->setPen(QPen(borde, 2, Qt::DashLine));

    aplicarSprite(objetivoEnVision ? spriteAlerta : spriteReposo);
}

bool enemigos::jugadorDetectado() const {
    return objetivoEnVision;
}

qreal enemigos::rangoVision() const {
    return radioVision;
}

QPixmap enemigos::crearSpriteCentinela(const QColor &colorBase, const QColor &colorBorde, const QColor &colorVisor,
                                       const QColor &colorOjos) const {
    constexpr int lado = 54;
    QPixmap sprite(lado, lado);
    sprite.fill(Qt::transparent);

    QPainter painter(&sprite);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF cuerpoRect(5, 5, lado - 10, lado - 10);

    QRadialGradient gradCuerpo(cuerpoRect.center(), cuerpoRect.width() / 2.0);
    gradCuerpo.setColorAt(0.0, colorBase.lighter(140));
    gradCuerpo.setColorAt(0.65, colorBase);
    gradCuerpo.setColorAt(1.0, colorBorde);

    painter.setBrush(gradCuerpo);
    painter.setPen(QPen(colorBorde, 3));
    painter.drawEllipse(cuerpoRect);

    // Visor frontal
    QRectF visorRect(lado * 0.2, lado * 0.42, lado * 0.6, lado * 0.22);
    painter.setBrush(colorVisor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(visorRect, lado * 0.12, lado * 0.12);

    // Ojos luminosos
    painter.setBrush(colorOjos);
    const qreal ojoRadio = lado * 0.07;
    painter.drawEllipse(QPointF(lado * 0.36, lado * 0.53), ojoRadio, ojoRadio);
    painter.drawEllipse(QPointF(lado * 0.64, lado * 0.53), ojoRadio, ojoRadio);

    // Base flotante
    QRadialGradient gradBase(QPointF(lado / 2.0, lado * 0.82), lado * 0.28);
    gradBase.setColorAt(0.0, QColor(0, 0, 0, 140));
    gradBase.setColorAt(1.0, QColor(0, 0, 0, 0));

    painter.setBrush(gradBase);
    painter.drawEllipse(QRectF(lado * 0.2, lado * 0.72, lado * 0.6, lado * 0.18));

    return sprite;
}

void enemigos::aplicarSprite(const QPixmap &sprite) {
    setPixmap(sprite);
    setOffset(-sprite.width() / 2.0, -sprite.height() / 2.0);
}
