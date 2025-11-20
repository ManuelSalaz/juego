#include "personaje.h"
#include <QTransform>
#include <QDebug>
#include <QBrush>
#include <QPen>
#include <QString>

personaje::personaje()
    : animacionActual(nullptr), estadoActual(EstadoAnimacion::Idle), framesDesdeSuelo(0), coyoteFrames(4) {
    spriteSheet.load(":/sprites/Viking-Sheet.png");

    hitbox = new QGraphicsRectItem(100, 130, 80, 50, this);
    hitbox->setBrush(QBrush(QColor(255, 0, 0, 100)));  // rojo semitransparente
    hitbox->setPen(Qt::NoPen);

    velocidad = 2.5f;    // 🔹 movimiento lateral
    gravedad = 0.6f;     // 🔹 fuerza de caída
    mirandoDerecha = true;

    int frameWidth  = 115;   // 1380 / 12
    int frameHeight = 84;    // 420 / 5

    // ---- Cargar animaciones ----
    animaciones[EstadoAnimacion::Idle] = extraerFrames(0 * frameHeight, frameWidth, frameHeight, 8);
    animaciones[EstadoAnimacion::Run] = extraerFrames(2 * frameHeight, frameWidth, frameHeight, 8);
    animaciones[EstadoAnimacion::Jump] = extraerFrames(6 * frameHeight, frameWidth, frameHeight, 3);
    animaciones[EstadoAnimacion::Slide] = extraerFrames(15 * frameHeight, frameWidth, frameHeight, 7);
    animaciones[EstadoAnimacion::Attack] = extraerFrames(8 * frameHeight, frameWidth, frameHeight, 4);


    frameActual = 0;
    cambiarAnimacion(EstadoAnimacion::Idle, true);

    setPixmap(animacionActual->at(frameActual)
                  .scaled(290, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setPos(300, 80);

    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &personaje::actualizarFrame);
    animTimer->start(70);
}

QVector<QPixmap> personaje::extraerFrames(int y, int frameWidth, int frameHeight, int numFrames) {
    QVector<QPixmap> frames;
    for (int i = 0; i < numFrames; ++i) {
        frames.append(spriteSheet.copy(i * frameWidth, y, frameWidth, frameHeight));
    }
    return frames;
}

void personaje::actualizarFrame() {

    if (!animacionActual || animacionActual->isEmpty()) return;

    frameActual = (frameActual + 1) % animacionActual->size();

    QPixmap frame = animacionActual->at(frameActual);

    if (!mirandoDerecha)
        frame = frame.transformed(QTransform().scale(-1, 1));

    frame = frame.scaled(290, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    setPixmap(frame);
}

void personaje::moverIzquierda() {
    velocidadX = -velocidad;   // ← establece movimiento continuo
    hitbox->setRect(100, 130, 80, 50);
    actualizarDireccion(false);
    if (!accionEspecialActiva)
        cambiarAnimacion(enSuelo ? EstadoAnimacion::Run : EstadoAnimacion::Jump, true);

}

void personaje::moverDerecha() {
    velocidadX = velocidad;    // ← establece movimiento continuo
    hitbox->setRect(100, 130, 80, 50);
    actualizarDireccion(true);
    if (!accionEspecialActiva)
        cambiarAnimacion(enSuelo ? EstadoAnimacion::Run : EstadoAnimacion::Jump, true);

}

void personaje::parar() {
    velocidadX = 0;            // ← detiene el movimiento lateral
    if (enSuelo && !accionEspecialActiva){
        cambiarAnimacion(EstadoAnimacion::Idle, true);
        hitbox->setRect(100, 130, 80, 50);
    }
}


void personaje::saltar() {
    if (framesDesdeSuelo > coyoteFrames)
        return;

    velocidadY = -13;
    enSuelo = false;
    framesDesdeSuelo = coyoteFrames + 1;
    cambiarAnimacion(EstadoAnimacion::Jump, true);
}

void personaje::deslizar() {
    if (!enSuelo || accionEspecialActiva) return;

    accionEspecialActiva = true;

    // Mantener misma base de hitbox (bottom)
    QRectF r = hitbox->rect();
    qreal bottom = r.top() + r.height();

    qreal nuevaAltura = 25;
    qreal nuevaY = bottom - nuevaAltura;

    hitbox->setRect(r.left(), nuevaY, r.width(), nuevaAltura);

    cambiarAnimacion(EstadoAnimacion::Slide, true);

    float velocidadOriginal = velocidadX;
    velocidadX = mirandoDerecha ? velocidad + 2 : -velocidad - 2;

    QTimer::singleShot(700, [this]() {
        // Restaurar hitbox completa
        hitbox->setRect(100, 130, 80, 50);

        velocidadX = 0;
        accionEspecialActiva = false;
        cambiarAnimacion(EstadoAnimacion::Idle);
    });
}

void personaje::atacar() {
    if (accionEspecialActiva) return;
    accionEspecialActiva = true;

    cambiarAnimacion(EstadoAnimacion::Attack, true);

    QTimer::singleShot(800, [this]() {
        accionEspecialActiva = false;
        cambiarAnimacion(EstadoAnimacion::Idle);
    });
}

QRectF personaje::posHitbox()
{
    return hitbox->sceneBoundingRect();
}



void personaje::actualizarFisica()
{
    // Movimiento horizontal
    setX(x() + velocidadX);

    // Aplicar gravedad
    if (!enSuelo) {
        velocidadY += gravedad;
        setY(y() + velocidadY);
    }

    bool tocandoSuelo = false;

    // --- Colisiones usando la HITBOX ---
    QList<QGraphicsItem*> colisiones = hitbox->collidingItems();
    for (QGraphicsItem *item : colisiones) {

        const QString tipoItem = item->data(0).toString();
        if (tipoItem == "plataforma" || tipoItem == "suelo") {

            qreal topPlataforma = item->sceneBoundingRect().top();
            qreal bottomHitbox = hitbox->sceneBoundingRect().bottom();

            // Solo si cae desde arriba
            if (velocidadY > 0 && bottomHitbox > topPlataforma) {

                const QRectF rectoHitbox = hitbox->rect();
                const qreal alturaHitbox = rectoHitbox.height();
                const qreal desplazamientoHitbox = rectoHitbox.top();
                const qreal nuevaY = topPlataforma - (desplazamientoHitbox + alturaHitbox);

                // Ajustar al personaje (NO a la hitbox)
                setY(nuevaY);
                velocidadY = 0;
                enSuelo = true;
                framesDesdeSuelo = 0;
                tocandoSuelo = true;

                // Animación correcta
                if (!accionEspecialActiva) {
                    cambiarAnimacion((velocidadX == 0) ? EstadoAnimacion::Idle : EstadoAnimacion::Run);
                }
                break;
            }
        }
    }

    // --- Si NO tocó piso ni plataforma ---
    if (!tocandoSuelo) {
        enSuelo = false;
        if (framesDesdeSuelo <= coyoteFrames)
            ++framesDesdeSuelo;
    }
}

void personaje::cambiarAnimacion(EstadoAnimacion estado, bool reiniciarFrame)
{
    const bool mismaAnimacion = (estadoActual == estado);
    if (mismaAnimacion && animacionActual && !reiniciarFrame)
        return;

    estadoActual = estado;
    animacionActual = &animaciones[estadoActual];
    if (animacionActual->isEmpty())
        return;

    if (reiniciarFrame || !mismaAnimacion || frameActual >= animacionActual->size())
        frameActual = 0;

    QPixmap frame = animacionActual->at(frameActual);
    if (!mirandoDerecha)
        frame = frame.transformed(QTransform().scale(-1, 1));
    frame = frame.scaled(290, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(frame);
}

void personaje::actualizarDireccion(bool aLaDerecha)
{
    if (mirandoDerecha == aLaDerecha)
        return;

    mirandoDerecha = aLaDerecha;

    if (!animacionActual || animacionActual->isEmpty())
        return;

    QPixmap frame = animacionActual->at(frameActual);

}





