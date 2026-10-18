
#include <iostream>
#include <cstring>
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


int main() {
    // Crear el administrador del parqueadero.
    Parqueadero parqueadero;

    // Elegir qué hacer con la información guardada.
    int opcion;

    cout << "\n====================================\n";
    cout << "       PARQUEADERO ITM\n";
    cout << "====================================\n";
    cout << "1. Cargar informacion anterior\n";
    cout << "2. Empezar desde cero\n";
    cout << "Selecciona una opcion: ";

    if (!(cin >> opcion)) {
        cout << "Entrada invalida. Cierra y vuelve a iniciar.\n";
        return 1;
    }

    while (opcion != 1 && opcion != 2) {
        cout << "Opcion invalida. Escribe 1 o 2: ";

        if (!(cin >> opcion)) {
            cout << "Entrada invalida. Cierra y vuelve a iniciar.\n";
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

    // Guardar el estado inicial elegido y mostrar las 40 celdas.
    parqueadero.guardarEstadoArchivo();
    parqueadero.mostrarEstado();

    // Inicializar la comunicacion de Windows.
    WSADATA datosWinsock;

    if (WSAStartup(MAKEWORD(2, 2), &datosWinsock) != 0) {
        cout << "Error al iniciar Winsock.\n";
        return 1;
    }

    SOCKET servidor = socket(AF_INET, SOCK_STREAM, 0);

    if (servidor == INVALID_SOCKET) {
        cout << "Error al crear el socket del servidor.\n";
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
        cout << "Comprueba que no haya otro servidor ejecutandose.\n";
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    if (listen(servidor, 1) == SOCKET_ERROR) {
        cout << "Error al poner el servidor en escucha.\n";
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    cout << "\n====================================\n";
    cout << " SERVIDOR DEL PARQUEADERO ITM\n";
    cout << " Puerto: " << PORT << "\n";
    cout << " Esperando conexion del cliente...\n";
    cout << "====================================\n";

    sockaddr_in direccionCliente = {};
    int tamanioDireccion = sizeof(direccionCliente);

    SOCKET cliente = accept(
        servidor,
        (sockaddr*)&direccionCliente,
        &tamanioDireccion
    );

    if (cliente == INVALID_SOCKET) {
        cout << "Error al aceptar la conexion.\n";
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    cout << "Cliente conectado correctamente.\n";

    // Recibir y procesar los vehiculos.
    while (true) {
        VehiculoPacket solicitud = {};

        if (!recibirCompleto(
            cliente,
            (char*)&solicitud,
            sizeof(solicitud)
        )) {
            cout << "\nEl cliente se desconecto.\n";
            break;
        }

        // Garantizar el final de las cadenas.
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

        // Guardar y mostrar el estado actualizado.
        parqueadero.guardarEstadoArchivo();
        parqueadero.mostrarEstado();

        if (!enviarCompleto(
            cliente,
            (char*)&respuesta,
            sizeof(respuesta)
        )) {
            cout << "Error al enviar la respuesta al cliente.\n";
            break;
        }
    }

    closesocket(cliente);
    closesocket(servidor);
    WSACleanup();

    cout << "Servidor detenido correctamente.\n";

    return 0;
}
