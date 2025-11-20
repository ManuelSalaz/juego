#include "niveles.h"
#include <QDebug>
#include <QList>
#include <QVariant>
#include <vector>
#include <utility>
#include <QMessageBox>

namespace {

struct PlataformaInfo {
    int x;
    int y;
    int ancho;
    int alto;
    QColor color;
};

QGraphicsRectItem* crearPlataforma(QGraphicsScene *scene, int x, int y, int ancho, int alto,
                                   const QColor &color = QColor("#654321"))
{
    QGraphicsRectItem *plataforma = new QGraphicsRectItem(x, y, ancho, alto);
    plataforma->setBrush(color);
    plataforma->setPen(Qt::NoPen);
    plataforma->setZValue(1);
    plataforma->setData(0, QVariant(QStringLiteral("plataforma")));
    scene->addItem(plataforma);
    return plataforma;
}

}

niveles::niveles(int numNivel, QWidget *parent)
    : QGraphicsView(parent),
    scene(new QGraphicsScene(this)),
    player(nullptr),
    timerUpdate(new QTimer(this)),
    nivelActual(numNivel)
{
    // -- Visor de vidas ---
    textoVidas = new QLabel(this);
    textoVidas->setText("Vidas: 3");
    textoVidas->setStyleSheet("color:white; font-size: 20px;");
    textoVidas->move(10, 10); // esquina superior
    textoVidas->raise();

    // -- Monedas
    textoMonedas = new QLabel(this);
    textoMonedas->setText("Monedas: 0");
    textoMonedas->setStyleSheet("color: yellow; font-size: 20px;");
    textoMonedas->move(10, 40); // debajo del contador de vidas
    textoMonedas->raise();

    //-- Escena Base ----
    configurarEscenaBase();



    // --- Crear personaje ---
    player = new personaje();
    scene->addItem(player);
    player->setZValue(2);

    if (nivelActual == 3) {
        player->setPos(scene->sceneRect().width() / 2, 500);
    } else {
        player->setPos(100, 550);
    }

    crearPlataformas();

    generarCentinelas();

    // --- Cámara ---
    centerOn(player);
    setAlignment(Qt::AlignCenter); // evita recortes de borde

    // --- Timer general del nivel ---
    connect(timerUpdate, &QTimer::timeout, this, &niveles::actualizarEscena);
    timerUpdate->start(16); // ~60 FPS
}

void niveles::actualizarEscena()
{
    if (!player) return;

    player->actualizarFisica();

    // --- Cámara sigue al jugador con un pequeño offset vertical ---
    QPointF centro = player->pos();
    centerOn(centro.x(), centro.y() - 100);

    if (centro.x() < 0) {
        player->setPos(0, 600);
    }

    if (centro.x() > 3700) {
        player->setPos(3700, 600);
    }

    // ===========================
    //  PROCESAR ENEMIGOS
    // ===========================
    for (enemigos *centinela : std::as_const(centinelas)) {
        if (!centinela) continue;

        if (nivelActual == 3) {

            QRectF hitboxJugador = player->posHitbox();
            QRectF enemigoRect   = centinela->sceneBoundingRect();

            // Revisión de colisión ANTES de mover
            if (hitboxJugador.intersects(enemigoRect)) {

                scene->removeItem(centinela);
                centinelas.removeOne(centinela);
                delete centinela;

                player->perderVida();
                textoVidas->setText(QString("Vidas: %1").arg(player->vidas));

                if (player->vidas <= 0)
                    emit gameOver("muerte");

                // detener el procesamiento de TODOS los enemigos
                return;
            }
        }

        if (nivelActual == 1) {
            centinela->actualizarVision(player->posHitbox());

            if (centinela->jugadorDetectado()) {
                player->setPos(0, 600);
                player->perderVida();
                textoVidas->setText(QString("Vidas: %1").arg(player->vidas));

                if (player->vidas <= 0) {
                    emit gameOver("muerte");
                    return;
                }
            }
        }
        centinela->mover();
    }

    if (player->atacando) {
        QRectF golpe = player->hitboxAtaque->sceneBoundingRect();

        for (int i = centinelas.size() - 1; i >= 0; i--) {
            enemigos *e = centinelas[i];
            if (!e) continue;

            if (golpe.intersects(e->sceneBoundingRect())) {
                scene->removeItem(e);
                delete e;
                centinelas.removeAt(i);
            }
        }
    }


    for (int i = monedasEscena.size() - 1; i >= 0; i--) {
        QGraphicsPixmapItem *m = monedasEscena[i];

        if (player->posHitbox().intersects(m->sceneBoundingRect())) {

            scene->removeItem(m);
            delete m;
            monedasEscena.removeAt(i);

            monedas++;
            textoMonedas->setText(QString("Monedas: %1").arg(monedas));

            if (monedas >= 3) {
                QMessageBox::information(this, "¡Nivel completado!", "Has recolectado todas las monedas.");
                emit gameOver("ganar");
                return;
            }
        }
    }
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

void niveles::configurarEscenaBase()
{
    setScene(scene);
    setFixedSize(800, 600);
    setFocusPolicy(Qt::StrongFocus);
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPixmap bg;


    if(nivelActual ==1){
        bg.load(":/backgrounds/background.jpg");
        QList<QPointF> posicionesMonedas = {
            {500, 550},
            {1000, 600},
            {1600, 600},
            {2500, 600}
        };

        for (const QPointF &p : posicionesMonedas) {
            QGraphicsPixmapItem *m = new QGraphicsPixmapItem(
                QPixmap(":/sprites/tesoro.png").scaled(90, 90)
                );
            m->setPos(p);
            m->setData(0, "moneda");
            m->setZValue(3);
            scene->addItem(m);
            monedasEscena.append(m);
        }
    }

    else if (nivelActual == 2) {
        bg.load(":/backgrounds/background_nivel3.jpg");  // pon el nombre que tengas en tu .qrc
    }
    else if (nivelActual == 3) {
        scene->setSceneRect(0, 0, 1600, 800);  // tamaño pequeño tipo arena
        QPixmap bg(":/backgrounds/background3.jpg");
        QGraphicsPixmapItem *background = new QGraphicsPixmapItem(bg);
        background->setPos(0, 0);
        background->setZValue(0);
        scene->addItem(background);

        resetTransform();
        scale(0.9, 0.9);
        return;
    }
    else {
        bg.load(":/backgrounds/background.jpg");         // el que ya usas en nivel 1
    }

    const int backgroundTiles = 2;
    const int levelWidth = bg.width() * backgroundTiles;
    scene->setSceneRect(0, 0, levelWidth, 1080);

    for (int i = 0; i < backgroundTiles; ++i) {
        QGraphicsPixmapItem *background = new QGraphicsPixmapItem(bg);
        background->setPos(i * bg.width(), 0);
        background->setZValue(0);
        scene->addItem(background);
    }

    resetTransform();  // limpia transformaciones anteriores
    scale(0.8, 0.8);
}

void niveles::crearPlataformas()
{

    if (nivelActual == 3) {
        // --- SOLO SUELO, SIN PLATAFORMAS ---
        const int levelWidth = static_cast<int>(scene->sceneRect().width());

        QGraphicsRectItem *suelo = new QGraphicsRectItem(0, 780, levelWidth, 40);
        suelo->setBrush(QColor("#5B3A29"));   // marrón
        suelo->setPen(Qt::NoPen);
        suelo->setZValue(1);
        suelo->setData(0, QVariant(QStringLiteral("suelo")));
        scene->addItem(suelo);

        const std::vector<PlataformaInfo> plataformas = {
            {420, 620, 220, 20, QColor("#6F4E37")},
            {700, 500, 180, 20, QColor("#855E42")},
            {950, 700, 160, 20, QColor("#6F4E37")}
        };

        for (const auto &plataforma : plataformas) {
            crearPlataforma(scene, plataforma.x, plataforma.y, plataforma.ancho, plataforma.alto, plataforma.color);
        }

        return; // importante: NO crear las plataformas del nivel 1 aquí
    }

    else if(nivelActual == 1){

    const std::vector<PlataformaInfo> plataformas = {
        {420, 620, 220, 20, QColor("#6F4E37")},
        {700, 500, 180, 20, QColor("#855E42")},
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

    const int levelWidth = static_cast<int>(scene->sceneRect().width());
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, 780, levelWidth, 40);
    suelo->setBrush(QColor("#5B3A29"));   // marrón
    suelo->setPen(Qt::NoPen);
    suelo->setZValue(1);
    suelo->setData(0, QVariant(QStringLiteral("suelo")));
    scene->addItem(suelo);
    }
}

void niveles::generarCentinelas()
{
    if (nivelActual == 3) {

        // Timer para oleadas automáticas
        QTimer *timerOleadas = new QTimer(this);

        connect(timerOleadas, &QTimer::timeout, this, [this]() {

            // Crear 2 enemigos a izquierda y derecha
            for (int i = 0; i < 2; i++) {

                // ENEMIGO IZQUIERDA
                enemigos *eL = new enemigos(this);
                eL->setPos(-100, 500);
                eL->setZValue(2);
                eL->habilitarCampo(player);   // <<--- ACTIVAMOS CAMPO
                centinelas.append(eL);
                scene->addItem(eL);

                // ENEMIGO DERECHA
                enemigos *eR = new enemigos(this);
                eR->setPos(scene->sceneRect().width() + 100, 500);
                eR->setZValue(2);
                eR->habilitarCampo(player);   // <<--- ACTIVAMOS CAMPO
                centinelas.append(eR);
                scene->addItem(eR);
            }
        });

        timerOleadas->start(4000); // cada 4 segundos
        return;
    }

    else if (nivelActual ==1){
    const QList<QPointF> posiciones = {
        {520, 650},
        {1220, 620},
        {2100, 640},
        {3000, 650}
    };

    for (const QPointF &pos : posiciones) {
        enemigos *centinela = new enemigos(this);
        centinela->setZValue(2);
        centinela->setPos(pos);
        centinela->configurarPatrulla(pos.x() - 80, pos.x() + 80, 1.2);
        centinelas.append(centinela);
        scene->addItem(centinela);
    }
    }
}
