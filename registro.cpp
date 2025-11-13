#include "registro.h"
#include <QFile>
#include <QTextStream>

Registro::Registro() {
    fileName = "registro_jugadores.txt"; // Archivo donde se guardan los datos
}

bool Registro::guardarDatos(const QString &nombre, const QString &usuario) {
    QFile file(fileName);

    // Abrimos el archivo en modo Append (añadir al final)
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << nombre << "," << usuario << "\n";
        file.close();
        return true;
    }

    return false; // Si no se pudo abrir el archivo
}
