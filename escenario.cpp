#include "escenario.h"
#include <QPixmap>

escenario::escenario(int tipo, QObject *parent)
    : QObject(parent), m_tipo(tipo)
{
    setType(tipo);
}

void escenario::setType(int tipo)
{
    m_tipo = tipo;

    // Por ahora solo ponemos 2 ejemplos de sprite.
    // Ajusta las rutas a como tengas tu .qrc o comenta estas líneas
    // si aún no tienes imágenes.

    switch (m_tipo) {
    case 0: // suelo
        setPixmap(QPixmap(":/sprites/suelo.png").scaled(32, 32));
        break;
    case 1: // pared / roca
        setPixmap(QPixmap(":/sprites/roca.png").scaled(32, 32));
        break;
    default:
        // bloque genérico
        setPixmap(QPixmap(":/sprites/bloque.png").scaled(32, 32));
        break;
    }
}
