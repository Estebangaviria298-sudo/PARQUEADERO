#ifndef VEHICULO_PACKET_H
#define VEHICULO_PACKET_H

struct VehiculoPacket {
    char placa[7];
    char hora[9];
    int celda;
    int esSalida;
    int aceptado;
};

#endif