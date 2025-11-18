#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMap>

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
    enum class EstadoAnimacion { Idle, Run, Jump, Slide, Attack };

    QPixmap spriteSheet;
    QMap<EstadoAnimacion, QVector<QPixmap>> animaciones;
    QVector<QPixmap> *animacionActual;
    EstadoAnimacion estadoActual;
    float velocidadY = 0;     // velocidad vertical
    float gravedad = 0.8;     // gravedad constante
    bool enSuelo = true;      // para evitar doble salto
    float velocidadX = 0;  // velocidad horizontal
    int frameActual;
    bool mirandoDerecha;
    bool accionEspecialActiva=false;
    float velocidad;
    int framesDesdeSuelo;
    int coyoteFrames;
    QGraphicsRectItem *hitbox;
    QTimer *animTimer;

    QVector<QPixmap> extraerFrames(int y, int frameWidth, int frameHeight, int numFrames);
    void cambiarAnimacion(EstadoAnimacion estado, bool reiniciarFrame = false);
    void actualizarDireccion(bool aLaDerecha);

};

#endif // PERSONAJE_H
