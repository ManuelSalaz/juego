#ifndef ESCENARIO_H
#define ESCENARIO_H

#include <QGraphicsPixmapItem>
#include <QObject>

class escenario : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit escenario(int tipo = 0, QObject *parent = nullptr);

    void setType(int tipo);   // cambiar tipo de bloque

private:
    int m_tipo;
};

#endif // ESCENARIO_H
