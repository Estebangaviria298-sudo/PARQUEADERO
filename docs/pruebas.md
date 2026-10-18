# Plan y registro de pruebas — Parqueadero ITM

## 1. Objetivo

Registrar las pruebas realizadas y pendientes del sistema de parqueadero desarrollado en C++ y Python.

## 2. Compilación

**Prueba:** compilar el cliente y el servidor con g++ y Winsock.

**Resultado:** realizada durante el desarrollo.

## 3. Comunicación por sockets

**Prueba:** ejecutar el cliente y el servidor en procesos separados y verificar el envío y la recepción de las placas.

**Resultado:** la comunicación básica funcionó en las pruebas realizadas.

**Pendiente:** validar la conexión entre dos computadores diferentes conectados por Ethernet en el ITM.

## 4. Entrada y salida de vehículos

**Prueba:** enviar una placa nueva y comprobar que se asigne una celda libre. Enviar nuevamente una placa activa y comprobar que se libere su celda.

**Resultado:** lógica implementada y probada durante el desarrollo.

## 5. Capacidad del parqueadero

**Prueba:** comprobar que el sistema administre 40 celdas y muestre las cantidades ocupadas y libres.

**Resultado:** implementado.

## 6. Persistencia del estado

**Prueba:** guardar el estado en `estado_parqueadero.csv`, cerrar el servidor y recuperar la información en una nueva ejecución.

**Resultado:** recuperación del estado probada durante el desarrollo.

## 7. Visualizador Python

**Prueba:** ejecutar `python/visualizador.py`, comprobar la visualización de las 40 celdas y verificar que se actualicen según el archivo CSV.

**Resultado:** visualizador implementado y probado con el archivo de estado.

**Pendiente:** confirmar su funcionamiento durante la comunicación real entre los dos computadores del ITM.

## 8. Integración con SWIG

**Prueba:** generar la interfaz SWIG, compilar la extensión dinámica e importarla desde Python.

**Resultado:** la extensión se compiló y su importación se probó por separado.

**Pendiente:** comprobar la integración directa de SWIG con el estado que administra el servidor en ejecución.

## 9. Cierre del servidor

**Prueba:** presionar `Q` y comprobar que el servidor finalice y guarde el estado.

**Resultado:** funcionalidad implementada y probada durante el desarrollo.

## 10. Pruebas de red en el ITM

- [ ] Consultar la IPv4 actual del computador servidor.
- [ ] Configurar esa dirección en el cliente.
- [ ] Comprobar la conectividad entre los computadores.
- [ ] Verificar que el puerto configurado esté permitido por el firewall.
- [ ] Ejecutar cliente, servidor y visualizador.
- [ ] Confirmar la actualización de las celdas al recibir vehículos.
- [ ] Comprobar la persistencia del estado después del cierre.

## 11. Conclusión

El sistema cuenta con los componentes principales implementados. Las pruebas locales realizadas permiten avanzar hacia la validación final. La conexión entre los computadores del ITM y la integración directa de SWIG con el estado real del servidor permanecen como verificaciones pendientes.