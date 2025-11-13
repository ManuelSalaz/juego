#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QVector>

class personaje : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    personaje();
    void moverIzquierda();
    void moverDerecha();
    void parar();
    void saltar();

    void actualizarFisica();
    void deslizar();
    void atacar();
private slots:
    void actualizarFrame();

private:
    QPixmap spriteSheet;
    QVector<QPixmap> framesIdle;
    QVector<QPixmap> framesRun;
    QVector<QPixmap> framesSlide;
    QVector<QPixmap> *animacionActual;
    QVector<QPixmap> framesJump;
    QVector<QPixmap> framesAttack;
    float velocidadY = 0;     // velocidad vertical
    float gravedad = 0.8;     // gravedad constante
    bool enSuelo = true;      // para evitar doble salto
    float velocidadX = 0;  // velocidad horizontal
    int frameActual;
    bool mirandoDerecha;
    bool saltando = false;
    bool deslizando = false;
    bool atacando=false;
    bool accionEspecialActiva=false;
    float velocidad;
    QGraphicsRectItem *hitbox;
    QTimer *animTimer;

    QVector<QPixmap> extraerFrames(int y, int frameWidth, int frameHeight, int numFrames);

};

#endif // PERSONAJE_H
