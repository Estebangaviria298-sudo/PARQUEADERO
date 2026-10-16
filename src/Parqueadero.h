
#ifndef PARQUEADERO_H
#define PARQUEADERO_H

#include "VehiculoPacket.h"

#define TOTAL_CELDAS 40

struct Celda {
    bool ocupada;
    char placa[7];
    char hora[9];
};

class Parqueadero {
private:
    Celda celdas[TOTAL_CELDAS];

    int buscarPlaca(const char placa[]) const;
    int buscarCeldaLibre() const;

public:
    Parqueadero();

    void procesarVehiculo(
        const VehiculoPacket& solicitud,
        VehiculoPacket& respuesta
    );

    void mostrarEstado() const;

    // Funciones para consultar el parqueadero desde Python.
    int contarOcupadas() const;
    bool estaOcupada(int numeroCelda) const;
    const char* obtenerPlaca(int numeroCelda) const;
    const char* obtenerHora(int numeroCelda) const;
};

#endif
