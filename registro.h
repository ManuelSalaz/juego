#ifndef REGISTRO_H
#define REGISTRO_H

#include <QString>

class Registro {
public:
    Registro();
    bool guardarDatos(const QString &nombre, const QString &usuario);

private:
    QString fileName;
};

#endif // REGISTRO_H
