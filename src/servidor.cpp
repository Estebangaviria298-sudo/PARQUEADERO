
#include <iostream>
#include <cstring>
#include <conio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "VehiculoPacket.h"
#include "Parqueadero.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

using namespace std;


bool recibirCompleto(
    SOCKET socketCliente,
    char* datos,
    int cantidad
) {
    int recibidos = 0;

    while (recibidos < cantidad) {
        int resultado = recv(
            socketCliente,
            datos + recibidos,
            cantidad - recibidos,
            0
        );

        if (resultado <= 0) {
            return false;
        }

        recibidos += resultado;
    }

    return true;
}


bool enviarCompleto(
    SOCKET socketCliente,
    const char* datos,
    int cantidad
) {
    int enviados = 0;

    while (enviados < cantidad) {
        int resultado = send(
            socketCliente,
            datos + enviados,
            cantidad - enviados,
            0
        );

        if (resultado <= 0) {
            return false;
        }

        enviados += resultado;
    }

    return true;
}


// Detectar si el usuario presiona Q.
bool quiereSalir() {
    if (_kbhit()) {
        int tecla = _getch();

        if (tecla == 'q' || tecla == 'Q') {
            return true;
        }
    }

    return false;
}


int main() {
    Parqueadero parqueadero;

    // Elegir si se recuperan los datos anteriores.
    int opcion;

    cout << "\n====================================\n";
    cout << "       PARQUEADERO ITM\n";
    cout << "====================================\n";
    cout << "1. Cargar informacion anterior\n";
    cout << "2. Empezar desde cero\n";
    cout << "Selecciona una opcion: ";

    if (!(cin >> opcion)) {
        cout << "Entrada invalida.\n";
        return 1;
    }

    while (opcion != 1 && opcion != 2) {
        cout << "Opcion invalida. Escribe 1 o 2: ";

        if (!(cin >> opcion)) {
            cout << "Entrada invalida.\n";
            return 1;
        }
    }

    if (opcion == 1) {
        if (parqueadero.cargarEstadoArchivo()) {
            cout << "\nInformacion anterior recuperada.\n";
        } else {
            cout << "\nNo se encontro informacion guardada.\n";
            cout << "Se iniciara un parqueadero vacio.\n";
        }
    } else {
        cout << "\nSe iniciara desde cero.\n";
    }

    parqueadero.guardarEstadoArchivo();
    parqueadero.mostrarEstado();

    // Iniciar Winsock.
    WSADATA datosWinsock;

    if (WSAStartup(MAKEWORD(2, 2), &datosWinsock) != 0) {
        cout << "Error al iniciar Winsock.\n";
        return 1;
    }

    SOCKET servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor == INVALID_SOCKET) {
        cout << "Error al crear el socket.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in direccionServidor = {};
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(PORT);
    direccionServidor.sin_addr.s_addr = INADDR_ANY;

    if (bind(
        servidor,
        (sockaddr*)&direccionServidor,
        sizeof(direccionServidor)
    ) == SOCKET_ERROR) {
        cout << "Error al asociar el puerto " << PORT << ".\n";
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    if (listen(servidor, 1) == SOCKET_ERROR) {
        cout << "Error al iniciar la escucha.\n";
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    cout << "\n====================================\n";
    cout << " SERVIDOR DEL PARQUEADERO ITM\n";
    cout << " Puerto: " << PORT << "\n";
    cout << " Presiona Q para salir.\n";
    cout << " Esperando conexion del cliente...\n";
    cout << "====================================\n";

    SOCKET cliente = INVALID_SOCKET;
    bool salir = false;

    // Esperar al cliente sin dejar de revisar la tecla Q.
    while (!salir) {
        if (quiereSalir()) {
            salir = true;
            break;
        }

        fd_set conjuntoLectura;
        FD_ZERO(&conjuntoLectura);
        FD_SET(servidor, &conjuntoLectura);

        timeval espera;
        espera.tv_sec = 0;
        espera.tv_usec = 100000;

        int resultado = select(
            0,
            &conjuntoLectura,
            NULL,
            NULL,
            &espera
        );

        if (resultado == SOCKET_ERROR) {
            cout << "Error al revisar conexiones.\n";
            break;
        }

        if (resultado == 0) {
            continue;
        }

        sockaddr_in direccionCliente = {};
        int tamanioDireccion = sizeof(direccionCliente);

        cliente = accept(
            servidor,
            (sockaddr*)&direccionCliente,
            &tamanioDireccion
        );

        if (cliente == INVALID_SOCKET) {
            cout << "Error al aceptar la conexion.\n";
            continue;
        }

        cout << "\nCliente conectado correctamente.\n";
        cout << "Presiona Q para detener el servidor.\n";

        // Recibir vehiculos y revisar la tecla Q.
        while (!salir) {
            if (quiereSalir()) {
                salir = true;
                break;
            }

            fd_set lecturaCliente;
            FD_ZERO(&lecturaCliente);
            FD_SET(cliente, &lecturaCliente);

            timeval esperaCliente;
            esperaCliente.tv_sec = 0;
            esperaCliente.tv_usec = 100000;

            int disponible = select(
                0,
                &lecturaCliente,
                NULL,
                NULL,
                &esperaCliente
            );

            if (disponible == SOCKET_ERROR) {
                cout << "Error en la conexion del cliente.\n";
                break;
            }

            if (disponible == 0) {
                continue;
            }

            VehiculoPacket solicitud = {};

            if (!recibirCompleto(
                cliente,
                (char*)&solicitud,
                sizeof(solicitud)
            )) {
                cout << "\nEl cliente se desconecto.\n";
                break;
            }

            solicitud.placa[6] = '\0';
            solicitud.hora[8] = '\0';

            VehiculoPacket respuesta = {};

            cout << "\nPlaca recibida: "
                 << solicitud.placa << "\n";

            cout << "Hora recibida: "
                 << solicitud.hora << "\n";

            parqueadero.procesarVehiculo(
                solicitud,
                respuesta
            );

            parqueadero.guardarEstadoArchivo();
            parqueadero.mostrarEstado();

            if (!enviarCompleto(
                cliente,
                (char*)&respuesta,
                sizeof(respuesta)
            )) {
                cout << "Error al responder al cliente.\n";
                break;
            }
        }

        if (cliente != INVALID_SOCKET) {
            closesocket(cliente);
            cliente = INVALID_SOCKET;
        }

        if (!salir) {
            cout << "\nEsperando otra conexion...\n";
        }
    }

    // Guardar el estado final y liberar los recursos.
    parqueadero.guardarEstadoArchivo();

    if (cliente != INVALID_SOCKET) {
        closesocket(cliente);
    }

    closesocket(servidor);
    WSACleanup();

    cout << "\nEstado del parqueadero guardado.\n";
    cout << "Servidor detenido correctamente.\n";

    return 0;
}
