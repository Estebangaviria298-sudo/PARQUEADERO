# Pruebas del parqueadero - ITM

## 1. Objetivo

Verificar el funcionamiento de la comunicación entre el cliente y el servidor del sistema de parqueadero.

## 2. Compilación

Se compilaron el cliente y el servidor en C++ utilizando g++ y la biblioteca Winsock.

## 3. Comunicación por sockets

Se ejecutaron el servidor y el cliente en ventanas separadas. Se comprobó que el servidor recibiera las placas enviadas por el cliente.

**Resultado:** la comunicación funcionó correctamente durante la prueba realizada.

## 4. Entrada de vehículos

Se comprobó que el servidor asignara una celda disponible al recibir una placa nueva.

## 5. Salida de vehículos

Se verificó que el servidor liberara la celda cuando recibía nuevamente una placa que se encontraba ocupando el parqueadero.

## 6. Capacidad

El sistema administra 40 celdas y muestra cuántas están ocupadas y cuántas están libres.

## 7. Pruebas pendientes

Quedan pendientes las pruebas del visualizador gráfico en Python y de la integración mediante SWIG.

## 8. Conclusión

La comunicación básica entre el cliente y el servidor funciona. El proyecto continuará con la organización del código y el desarrollo de los componentes restantes.