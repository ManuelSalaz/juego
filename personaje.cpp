#include "personaje.h"
#include <QTransform>
#include <QDebug>
#include <QBrush>
#include <QPen>

personaje::personaje() {
    spriteSheet.load(":/sprites/Viking-Sheet.png");

    hitbox = new QGraphicsRectItem(100, 240, 80, 50, this);
    hitbox->setBrush(QBrush(QColor(255, 0, 0, 100)));  // rojo semitransparente
    hitbox->setPen(Qt::NoPen);

    velocidad = 2.5f;    // 🔹 movimiento lateral
    gravedad = 0.6f;     // 🔹 fuerza de caída
    mirandoDerecha = true;

    int frameWidth  = 115;   // 1380 / 12
    int frameHeight = 84;    // 420 / 5

    // ---- Cargar animaciones ----
    framesIdle = extraerFrames(0 * frameHeight, frameWidth, frameHeight, 8);
    framesRun  = extraerFrames(2 * frameHeight, frameWidth, frameHeight, 8);
    framesJump = extraerFrames(6 * frameHeight, frameWidth, frameHeight, 3);
    framesSlide = extraerFrames(15 * frameHeight, frameWidth, frameHeight, 7);
    framesAttack = extraerFrames(8 * frameHeight, frameWidth, frameHeight, 4);


    animacionActual = &framesIdle;
    frameActual = 0;

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
    static int contador = 0;
    contador++;

    int intervalo = (animacionActual == &framesIdle) ? 4 : 2;
    if (contador % intervalo != 0)
        return;

    frameActual = (frameActual + 1) % animacionActual->size();
    QPixmap frame = animacionActual->at(frameActual);

    if (!mirandoDerecha)
        frame = frame.transformed(QTransform().scale(-1, 1));

    setPixmap(frame.scaled(290, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    actualizarFisica();

}

void personaje::moverIzquierda() {
    velocidadX = -velocidad;   // ← establece movimiento continuo
    animacionActual = enSuelo ? &framesRun : &framesJump;
    mirandoDerecha = false;
}

void personaje::moverDerecha() {
    velocidadX = velocidad;    // ← establece movimiento continuo
    animacionActual = enSuelo ? &framesRun : &framesJump;
    mirandoDerecha = true;
}

void personaje::parar() {
    velocidadX = 0;            // ← detiene el movimiento lateral
    if (enSuelo)
        animacionActual = &framesIdle;
}


void personaje::saltar() {
    if (!enSuelo) return;
    velocidadY = -13;
    enSuelo = false;
    animacionActual = &framesJump;
    frameActual = 0;

    // 🔹 Impulso horizontal automático
    if (mirandoDerecha)
        velocidadX = velocidad * 1.6f;  // más impulso hacia delante
    else
        velocidadX = -velocidad * 1.6f;
}

void personaje::deslizar() {
    if (!enSuelo || accionEspecialActiva) return;

    accionEspecialActiva = true;

    animacionActual = &framesSlide;
    frameActual = 0;

    float velocidadOriginal = velocidadX;
    if (mirandoDerecha)
        velocidadX = velocidad + 2;   // Desliza rápido a la derecha
    else
        velocidadX = -velocidad - 2;  // Desliza rápido a la izquierda

    QTimer::singleShot(700, [this, velocidadOriginal]() {
        velocidadX = 0;
        accionEspecialActiva = false;
        animacionActual = &framesIdle;
    });
}

void personaje::atacar() {
    if (accionEspecialActiva) return;
    accionEspecialActiva = true;

    animacionActual = &framesAttack;
    frameActual = 0;

    QTimer::singleShot(800, [this]() {
        accionEspecialActiva = false;
        animacionActual = &framesIdle;
    });
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

        if (item->data(0).toString() == "plataforma" || item->data(0).toString() == "suelo") {

            qreal topPlataforma = item->sceneBoundingRect().top();
            qreal bottomHitbox = hitbox->sceneBoundingRect().bottom();

            // Solo si cae desde arriba
            if (velocidadY > 0 && bottomHitbox >= topPlataforma - 5) {

                // Reposicionar al personaje encima de la plataforma
                // Información de la hitbox respecto al sprite
                const QRectF rectoHitbox = hitbox->rect();
                const qreal alturaHitbox = rectoHitbox.height();
                const qreal desplazamientoHitbox = rectoHitbox.top();

                // Mover al personaje para que la parte inferior de la hitbox
                // quede alineada con la plataforma en coordenadas de escena
                const qreal nuevaY = topPlataforma - (desplazamientoHitbox + alturaHitbox);

                // Ajustar al personaje (NO a la hitbox)
                setY(nuevaY);
                velocidadY = 0;
                enSuelo = true;
                tocandoSuelo = true;

                // Animación correcta
                if (!accionEspecialActiva) {
                    animacionActual = (velocidadX == 0) ? &framesIdle : &framesRun;
                }
                break;
            }
        }
    }

    // --- Si NO tocó piso ni plataforma ---
    if (!tocandoSuelo) {
        enSuelo = false;
    }
}





