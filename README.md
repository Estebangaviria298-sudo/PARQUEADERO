# Sistema de Parqueadero ITM — C++ y Python

## 1. Descripción

Proyecto académico para administrar un parqueadero de 40 celdas mediante una arquitectura cliente-servidor. El cliente genera placas de vehículos y envía sus datos al servidor utilizando sockets TCP en C++. El sistema administra las entradas y salidas, conserva el estado del parqueadero y dispone de un visualizador gráfico desarrollado en Python.

## 2. Objetivos

- Generar placas de vehículos automáticamente.
- Enviar y recibir información mediante sockets TCP.
- Registrar la placa, la hora y la celda asignada.
- Registrar la salida de un vehículo cuando se recibe nuevamente su placa.
- Administrar 40 celdas de parqueadero.
- Guardar y recuperar el estado del parqueadero.
- Visualizar gráficamente las celdas libres y ocupadas.
- Utilizar SWIG para crear una interfaz entre C++ y Python.

## 3. Tecnologías

- C++ y g++.
- Winsock para la comunicación TCP en Windows.
- Python y Tkinter para la interfaz gráfica.
- SWIG para la generación del enlace entre C++ y Python.
- CSV para la persistencia del estado.
- Git y GitHub para el control de versiones.

## 4. Estructura del proyecto

```text
PARQUEADERO/
├── src/
│   ├── cliente.cpp
│   ├── servidor.cpp
│   ├── Parqueadero.h
│   ├── Parqueadero.cpp
│   └── VehiculoPacket.h
├── python/
│   ├── visualizador.py
│   └── parqueadero.i
├── docs/
│   ├── pruebas.md
│   └── imagenes/
├── README.md
└── estado_parqueadero.csv
```

Los archivos auxiliares generados por SWIG y la librería compilada se encuentran en `python/`. Algunos son archivos generados automáticamente y no necesitan incluirse en el repositorio.

## 5. Funcionamiento general

1. El cliente genera placas y prepara los datos del vehículo.
2. El cliente envía una estructura al servidor mediante un socket TCP.
3. El servidor procesa la solicitud usando la clase `Parqueadero`.
4. El sistema asigna una celda libre o libera la celda asociada a una placa que ya estaba registrada, de acuerdo con la lógica implementada.
5. El servidor guarda el estado en `estado_parqueadero.csv`.
6. El visualizador Python consulta el archivo y representa las 40 celdas.
7. El servidor permite recuperar el estado anterior o iniciar desde cero.

## 6. Requisitos

- Windows.
- MSYS2 UCRT64 con el compilador g++ y Winsock.
- Python con Tkinter.
- SWIG y la configuración necesaria para compilar la extensión de Python.

## 7. Compilación y ejecución

Ejecuta los comandos desde la carpeta raíz del proyecto.

### Compilar el servidor

```bash
g++ src/servidor.cpp src/Parqueadero.cpp -o servidor.exe -lws2_32
```

### Compilar el cliente

```bash
g++ src/cliente.cpp -o cliente.exe -lws2_32
```

### Iniciar el servidor

```bash
./servidor.exe
```

Selecciona si deseas recuperar el estado anterior o empezar desde cero. Para detener el servidor, utiliza la tecla `Q`, según la implementación actual.

### Iniciar el visualizador

En otra terminal:

```bash
python python/visualizador.py
```

### Iniciar el cliente

En otra terminal:

```bash
./cliente.exe
```

El cliente debe tener configurada la dirección IPv4 actual del computador donde se ejecuta el servidor. Ambos computadores deben tener conectividad de red y utilizar el puerto configurado en el programa.

## 8. Persistencia de datos

El archivo `estado_parqueadero.csv` conserva información del parqueadero para permitir recuperar el estado en una ejecución posterior. El archivo se genera durante el funcionamiento del programa y no es necesario versionar los datos de ejecución en GitHub.

## 9. Integración C++ y Python

El archivo `python/parqueadero.i` define la interfaz de SWIG. La extensión dinámica permite acceder desde Python a funciones de la clase `Parqueadero`.

La compilación e importación de la extensión se probaron por separado. Está pendiente verificar si la interfaz SWIG participa directamente en el flujo de datos del servidor en ejecución.

## 10. Pruebas y limitaciones

La comunicación básica cliente-servidor, la administración del parqueadero, la recuperación del estado, el visualizador y la importación de la extensión SWIG se han trabajado por separado. La comunicación entre los dos computadores del ITM y la integración completa de SWIG deben validarse en el entorno final.

Consulta el documento `docs/pruebas.md` para ver el registro de pruebas.

## 11. Control de versiones

El proyecto utiliza Git y GitHub para registrar avances, organizar ramas y mantener el historial de cambios.

## 12. Autoría

Proyecto académico desarrollado para el ITM como parte de la formación del SENA.