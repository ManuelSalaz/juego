#include "niveles.h"
#include <QDebug>
#include <vector>

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
    setFocusPolicy(Qt::StrongFocus);
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, true);

    // --- Fondo ---
    QPixmap bg(":/backgrounds/background.jpg");
    const int backgroundTiles = 2;
    const int levelWidth = bg.width() * backgroundTiles;
    scene->setSceneRect(0, 0, levelWidth, 1080);

    for (int i = 0; i < backgroundTiles; ++i) {
        QGraphicsPixmapItem *background = new QGraphicsPixmapItem(bg);
        background->setPos(i * bg.width(), 0);
        background->setZValue(0);
        scene->addItem(background);
    }


    // --- Crear personaje ---
    player = new personaje();
    scene->addItem(player);
    player->setZValue(2);
    player->setPos(300, 200);

    //-- Crear Plataformas --
    struct PlataformaInfo {
        int x;
        int y;
        int ancho;
        int alto;
        QColor color;
    };

    const std::vector<PlataformaInfo> plataformas = {
        {420, 720, 220, 20, QColor("#6F4E37")},
        {700, 660, 180, 20, QColor("#855E42")},
        {950, 700, 160, 20, QColor("#6F4E37")},
        {1160, 660, 200, 20, QColor("#855E42")},
        {1400, 700, 180, 20, QColor("#6F4E37")},
        {1600, 650, 160, 20, QColor("#855E42")},
        {1800, 690, 150, 20, QColor("#6F4E37")},
        {1980, 650, 180, 20, QColor("#855E42")},
        {2200, 700, 200, 20, QColor("#6F4E37")},
        {2420, 660, 220, 20, QColor("#855E42")},
        {2660, 710, 160, 20, QColor("#6F4E37")},
        {2880, 670, 190, 20, QColor("#855E42")},
        {3100, 720, 210, 20, QColor("#6F4E37")},
        {3340, 660, 180, 20, QColor("#855E42")},
        {3560, 700, 220, 20, QColor("#6F4E37")}
    };

    for (const auto &plataforma : plataformas) {
        crearPlataforma(scene, plataforma.x, plataforma.y, plataforma.ancho, plataforma.alto, plataforma.color);
    }

    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, 780, levelWidth, 40);
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
