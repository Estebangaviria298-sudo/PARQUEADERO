
#include <iostream>
#include <cstring>
#include "Parqueadero.h"

Parqueadero::Parqueadero() {
    for (int i = 0; i < TOTAL_CELDAS; i++) {
        celdas[i].ocupada = false;
        celdas[i].placa[0] = '\0';
        celdas[i].hora[0] = '\0';
    }
}

int Parqueadero::buscarPlaca(const char placa[]) const {
    for (int i = 0; i < TOTAL_CELDAS; i++) {
        if (celdas[i].ocupada &&
            strcmp(celdas[i].placa, placa) == 0) {
            return i;
        }
    }
    return -1;
}

int Parqueadero::buscarCeldaLibre() const {
    for (int i = 0; i < TOTAL_CELDAS; i++) {
        if (!celdas[i].ocupada) {
            return i;
        }
    }
    return -1;
}

void Parqueadero::procesarVehiculo(
    const VehiculoPacket& solicitud,
    VehiculoPacket& respuesta
) {
    respuesta = solicitud;
    respuesta.aceptado = 0;

    int posicion = buscarPlaca(solicitud.placa);

    if (posicion != -1) {
        respuesta.esSalida = 1;
        respuesta.celda = posicion + 1;
        respuesta.aceptado = 1;

        celdas[posicion].ocupada = false;
        celdas[posicion].placa[0] = '\0';
        celdas[posicion].hora[0] = '\0';

        std::cout << "Salida: celda "
                  << posicion + 1 << " liberada.\n";
    }
    else if (solicitud.esSalida == 1) {
        respuesta.aceptado = 0;
        std::cout << "Salida rechazada: placa desconocida.\n";
    }
    else {
        int celda = buscarCeldaLibre();

        if (celda == -1) {
            respuesta.aceptado = 0;
            std::cout << "Parqueadero lleno.\n";
        }
        else {
            celdas[celda].ocupada = true;

            strcpy(celdas[celda].placa, solicitud.placa);
            strcpy(celdas[celda].hora, solicitud.hora);

            respuesta.celda = celda + 1;
            respuesta.esSalida = 0;
            respuesta.aceptado = 1;

            std::cout << "Entrada: asignada celda "
                      << celda + 1 << ".\n";
        }
    }
}

void Parqueadero::mostrarEstado() const {
    int ocupadas = 0;

    std::cout << "\n===== ESTADO DEL PARQUEADERO =====\n";

    for (int i = 0; i < TOTAL_CELDAS; i++) {
        std::cout << "Celda " << i + 1 << ": ";

        if (celdas[i].ocupada) {
            std::cout << "OCUPADA - "
                      << celdas[i].placa
                      << " - " << celdas[i].hora;
            ocupadas++;
        }
        else {
            std::cout << "LIBRE";
        }

        std::cout << "\n";
    }

    std::cout << "Ocupadas: " << ocupadas << "\n";
    std::cout << "Libres: "
              << TOTAL_CELDAS - ocupadas << "\n";
    std::cout << "==================================\n";
}
