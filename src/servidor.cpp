
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "VehiculoPacket.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define TOTAL_CELDAS 40

struct Celda {
    bool ocupada;
    char placa[7];
    char hora[9];
};

Celda parqueadero[TOTAL_CELDAS];

bool recibirCompleto(SOCKET sock, char* datos, int cantidad) {
    int recibidos = 0;

    while (recibidos < cantidad) {
        int resultado = recv(sock, datos + recibidos,
                             cantidad - recibidos, 0);
        if (resultado <= 0) return false;
        recibidos += resultado;
    }

    return true;
}

bool enviarCompleto(SOCKET sock, char* datos, int cantidad) {
    int enviados = 0;

    while (enviados < cantidad) {
        int resultado = send(sock, datos + enviados,
                             cantidad - enviados, 0);
        if (resultado <= 0) return false;
        enviados += resultado;
    }

    return true;
}

int buscarPlaca(const char placa[]) {
    for (int i = 0; i < TOTAL_CELDAS; i++) {
        if (parqueadero[i].ocupada &&
            strcmp(parqueadero[i].placa, placa) == 0) {
            return i;
        }
    }

    return -1;
}

int buscarCeldaLibre() {
    for (int i = 0; i < TOTAL_CELDAS; i++) {
        if (!parqueadero[i].ocupada) return i;
    }

    return -1;
}

void mostrarEstado() {
    int ocupadas = 0;

    std::cout << "\n===== ESTADO DEL PARQUEADERO =====\n";

    for (int i = 0; i < TOTAL_CELDAS; i++) {
        std::cout << "Celda " << i + 1 << ": ";

        if (parqueadero[i].ocupada) {
            std::cout << "OCUPADA - "
                      << parqueadero[i].placa
                      << " - " << parqueadero[i].hora;
            ocupadas++;
        } else {
            std::cout << "LIBRE";
        }

        std::cout << "\n";
    }

    std::cout << "Ocupadas: " << ocupadas << "\n";
    std::cout << "Libres: " << TOTAL_CELDAS - ocupadas << "\n";
    std::cout << "==================================\n";
}

int main() {
    WSADATA wsaData;

    for (int i = 0; i < TOTAL_CELDAS; i++) {
        parqueadero[i].ocupada = false;
        parqueadero[i].placa[0] = '\0';
        parqueadero[i].hora[0] = '\0';
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Error al iniciar Winsock.\n";
        return 1;
    }

    SOCKET servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor == INVALID_SOCKET) {
        std::cout << "Error al crear el socket.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in direccion = {};
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(PORT);

    if (bind(servidor, reinterpret_cast<sockaddr*>(&direccion),
             sizeof(direccion)) == SOCKET_ERROR) {
        std::cout << "Error al asignar el puerto 8080.\n";
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    if (listen(servidor, 3) == SOCKET_ERROR) {
        std::cout << "Error al escuchar conexiones.\n";
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    std::cout << "SERVIDOR DEL PARQUEADERO\n";
    std::cout << "Puerto: " << PORT << "\n";
    std::cout << "Celdas: " << TOTAL_CELDAS << "\n";
    std::cout << "Esperando conexion del cliente...\n";

    SOCKET cliente = accept(servidor, NULL, NULL);

    if (cliente == INVALID_SOCKET) {
        std::cout << "Error al aceptar al cliente.\n";
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    std::cout << "Cliente conectado correctamente.\n";

    while (true) {
        VehiculoPacket paquete = {};

        if (!recibirCompleto(cliente,
                reinterpret_cast<char*>(&paquete),
                sizeof(paquete))) {
            std::cout << "Cliente desconectado.\n";
            break;
        }

        paquete.placa[6] = '\0';
        paquete.hora[8] = '\0';

        VehiculoPacket respuesta = paquete;
        respuesta.aceptado = 0;

        std::cout << "\nPlaca recibida: " << paquete.placa << "\n";
        std::cout << "Hora recibida: " << paquete.hora << "\n";

        int posicion = buscarPlaca(paquete.placa);

        if (posicion != -1) {
            respuesta.esSalida = 1;
            respuesta.celda = posicion + 1;
            respuesta.aceptado = 1;

            parqueadero[posicion].ocupada = false;
            parqueadero[posicion].placa[0] = '\0';
            parqueadero[posicion].hora[0] = '\0';

            std::cout << "Salida: celda "
                      << posicion + 1 << " liberada.\n";
        } else if (paquete.esSalida == 1) {
            respuesta.aceptado = 0;
            std::cout << "Salida rechazada: placa desconocida.\n";
        } else {
            int celda = buscarCeldaLibre();

            if (celda == -1) {
                respuesta.aceptado = 0;
                std::cout << "Parqueadero lleno.\n";
            } else {
                parqueadero[celda].ocupada = true;
                strcpy(parqueadero[celda].placa, paquete.placa);
                strcpy(parqueadero[celda].hora, paquete.hora);

                respuesta.celda = celda + 1;
                respuesta.esSalida = 0;
                respuesta.aceptado = 1;

                std::cout << "Entrada: asignada celda "
                          << celda + 1 << ".\n";
            }
        }

        if (!enviarCompleto(cliente,
                reinterpret_cast<char*>(&respuesta),
                sizeof(respuesta))) {
            std::cout << "Error enviando respuesta.\n";
            break;
        }

        mostrarEstado();
    }

    closesocket(cliente);
    closesocket(servidor);
    WSACleanup();
    return 0;
}
