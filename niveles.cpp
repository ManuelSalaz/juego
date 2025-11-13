#include "niveles.h"
#include <QDebug>

QGraphicsRectItem* crearPlataforma(QGraphicsScene *scene, int x, int y, int ancho, int alto, QColor color = QColor("#654321"))
{
    QGraphicsRectItem *plataforma = new QGraphicsRectItem(x, y, ancho, alto);
    plataforma->setBrush(color);
    plataforma->setPen(Qt::NoPen);
    plataforma->setZValue(1);
    plataforma->setData(0, "plataforma");
    scene->addItem(plataforma);
    return plataforma;
}

niveles::niveles(int numNivel, QWidget *parent)
    : QGraphicsView(parent),
    scene(new QGraphicsScene(this)),
    player(nullptr),
    timerUpdate(new QTimer(this)),
    nivelActual(numNivel)
{
    // --- Configuración base ---
    setScene(scene);
    setFixedSize(800, 600);
    scene->setSceneRect(0, 0, 1920, 1080);   // Nivel ancho de 2000 px
    setFocusPolicy(Qt::StrongFocus);
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, true);

    // --- Fondo ---
    QPixmap bg(":/backgrounds/background.jpg");
    QGraphicsPixmapItem *background = new QGraphicsPixmapItem(bg);
    background->setZValue(0);
    scene->addItem(background);


    // --- Crear personaje ---
    player = new personaje();
    scene->addItem(player);
    player->setZValue(2);
    player->setPos(300, 200);

    //-- Crear Plataformas --
    crearPlataforma(scene, 600, 860, 200, 20);
    crearPlataforma(scene, 900, 500, 250, 20);
    crearPlataforma(scene, 1400, 700, 180, 20);
    crearPlataforma(scene, 600, 400, 150, 20, QColor("#855E42"));

    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, 780, 1920, 40);
    suelo->setBrush(QColor("#5B3A29"));   // marrón
    suelo->setPen(Qt::NoPen);
    suelo->setZValue(1);
    suelo->setData(0, "suelo");
    scene->addItem(suelo);

    // --- Cámara ---
    centerOn(player);
    setAlignment(Qt::AlignCenter); // evita recortes de borde

    // --- Timer general del nivel ---
    connect(timerUpdate, &QTimer::timeout, this, &niveles::actualizarEscena);
    timerUpdate->start(16); // ~60 FPS
    resetTransform();  // limpia transformaciones anteriores
    scale(0.8, 0.8);
}

void niveles::actualizarEscena()
{
    if (!player) return;

    player->actualizarFisica();

    // --- Cámara sigue al jugador con un pequeño offset vertical ---
    QPointF centro = player->pos();
    centerOn(centro.x(), centro.y() - 100);  // mueve el foco 100px hacia abajo

}

void niveles::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        player->moverIzquierda();
        break;
    case Qt::Key_D:
        player->moverDerecha();
        break;
    case Qt::Key_Space:
        player->saltar();
        break;
    case Qt::Key_C:
        player->deslizar();
        break;
    case Qt::Key_J:
        player->atacar();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void niveles::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A || event->key() == Qt::Key_D) {
        player->parar();
    } else {
        QGraphicsView::keyReleaseEvent(event);
    }
}
