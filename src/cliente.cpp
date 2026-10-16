
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <conio.h>
#include "VehiculoPacket.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define TOTAL_CELDAS 40

// Para probar en casa, usando el mismo computador.
// Cuando seas cliente en el ITM, cambia esta IP por la IPv4
// del computador que tenga ejecutandose el servidor.
const char IP_SERVIDOR[] = "192.168.1.6";

struct VehiculoActivo {
    char placa[7];
    int celda;
};

VehiculoActivo activos[TOTAL_CELDAS];
int cantidadActivos = 0;

// Genera una placa aleatoria de tres letras y tres numeros.
void generarPlaca(char placa[]) {
    placa[0] = 'A' + rand() % 26;
    placa[1] = 'A' + rand() % 26;
    placa[2] = 'A' + rand() % 26;

    placa[3] = '0' + rand() % 10;
    placa[4] = '0' + rand() % 10;
    placa[5] = '0' + rand() % 10;
    placa[6] = '\0';
}

// Evita crear una placa que ya este dentro del parqueadero.
void generarPlacaNueva(char placa[]) {
    bool repetida;

    do {
        generarPlaca(placa);
        repetida = false;

        for (int i = 0; i < cantidadActivos; i++) {
            if (strcmp(activos[i].placa, placa) == 0) {
                repetida = true;
                break;
            }
        }
    } while (repetida);
}

void obtenerHora(char hora[]) {
    time_t ahora = time(NULL);
    tm tiempo;
    localtime_s(&tiempo, &ahora);

    hora[0] = '0' + tiempo.tm_hour / 10;
    hora[1] = '0' + tiempo.tm_hour % 10;
    hora[2] = ':';
    hora[3] = '0' + tiempo.tm_min / 10;
    hora[4] = '0' + tiempo.tm_min % 10;
    hora[5] = ':';
    hora[6] = '0' + tiempo.tm_sec / 10;
    hora[7] = '0' + tiempo.tm_sec % 10;
    hora[8] = '\0';
}

bool enviarCompleto(SOCKET sock, char* datos, int cantidad) {
    int enviados = 0;

    while (enviados < cantidad) {
        int resultado = send(
            sock, datos + enviados, cantidad - enviados, 0
        );

        if (resultado <= 0) return false;
        enviados += resultado;
    }

    return true;
}

bool recibirCompleto(SOCKET sock, char* datos, int cantidad) {
    int recibidos = 0;

    while (recibidos < cantidad) {
        int resultado = recv(
            sock, datos + recibidos, cantidad - recibidos, 0
        );

        if (resultado <= 0) return false;
        recibidos += resultado;
    }

    return true;
}

int buscarActivo(const char placa[]) {
    for (int i = 0; i < cantidadActivos; i++) {
        if (strcmp(activos[i].placa, placa) == 0) {
            return i;
        }
    }

    return -1;
}

void agregarActivo(const char placa[], int celda) {
    if (cantidadActivos >= TOTAL_CELDAS) return;

    strcpy(activos[cantidadActivos].placa, placa);
    activos[cantidadActivos].celda = celda;
    cantidadActivos++;
}

void quitarActivo(const char placa[]) {
    int posicion = buscarActivo(placa);

    if (posicion == -1) return;

    for (int i = posicion; i < cantidadActivos - 1; i++) {
        activos[i] = activos[i + 1];
    }

    cantidadActivos--;
}

// Espera entre 2 y 5 segundos y permite pulsar Q para salir.
bool esperarSiguienteEnvio(int segundos) {
    std::cout << "Pulsa Q para detener el cliente.\n";

    for (int i = 0; i < segundos * 10; i++) {
        if (_kbhit()) {
            char tecla = _getch();

            if (tecla == 'q' || tecla == 'Q') {
                return false;
            }
        }

        Sleep(100);
    }

    return true;
}

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Error al iniciar Winsock.\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == INVALID_SOCKET) {
        std::cout << "Error al crear el socket.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in servidor = {};
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PORT);

    if (inet_pton(AF_INET, IP_SERVIDOR,
                  &servidor.sin_addr) != 1) {
        std::cout << "La IP del servidor no es valida.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Conectando con " << IP_SERVIDOR
              << " en el puerto " << PORT << "...\n";

    if (connect(sock, reinterpret_cast<sockaddr*>(&servidor),
                sizeof(servidor)) == SOCKET_ERROR) {
        std::cout << "No se pudo conectar al servidor.\n";
        std::cout << "Comprueba que este ejecutandose y revisa la IP.\n";

        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "\nCLIENTE conectado correctamente.\n";
    std::cout << "Placas aleatorias y envios cada 2 a 5 segundos.\n\n";

    int envios = 0;
    bool continuar = true;

    while (continuar) {
        VehiculoPacket paquete = {};

        // Cada cinco envios intenta retirar un vehiculo activo.
        bool salida = (
            envios > 0 &&
            (envios + 1) % 5 == 0 &&
            cantidadActivos > 0
        );

        if (salida) {
            int indice = rand() % cantidadActivos;

            strcpy(paquete.placa, activos[indice].placa);
            paquete.esSalida = 1;
            paquete.celda = activos[indice].celda;
        } else {
            generarPlacaNueva(paquete.placa);
            paquete.esSalida = 0;
            paquete.celda = 0;
        }

        obtenerHora(paquete.hora);

        if (!enviarCompleto(
                sock,
                reinterpret_cast<char*>(&paquete),
                sizeof(paquete))) {
            std::cout << "Conexion cerrada al enviar.\n";
            break;
        }

        VehiculoPacket respuesta = {};

        if (!recibirCompleto(
                sock,
                reinterpret_cast<char*>(&respuesta),
                sizeof(respuesta))) {
            std::cout << "El servidor cerro la conexion.\n";
            break;
        }

        respuesta.placa[6] = '\0';

        std::cout << "\n------------------------------\n";
        std::cout << "Placa: " << paquete.placa << "\n";
        std::cout << "Hora: " << paquete.hora << "\n";

        if (respuesta.aceptado == 1) {
            if (respuesta.esSalida == 1) {
                quitarActivo(respuesta.placa);

                std::cout << "SALIDA: celda "
                          << respuesta.celda << " liberada.\n";
            } else {
                agregarActivo(respuesta.placa, respuesta.celda);

                std::cout << "ENTRADA: celda "
                          << respuesta.celda << " asignada.\n";
            }
        } else {
            std::cout << "Operacion rechazada por el servidor.\n";
        }

        std::cout << "Vehiculos activos: "
                  << cantidadActivos << "\n";

        envios++;

        int segundos = 2 + rand() % 4;

        if (!esperarSiguienteEnvio(segundos)) {
            continuar = false;
        }
    }

    std::cout << "\nCerrando conexion con el servidor...\n";

    shutdown(sock, SD_BOTH);
    closesocket(sock);
    WSACleanup();

    std::cout << "Cliente detenido correctamente.\n";
    return 0;
}
