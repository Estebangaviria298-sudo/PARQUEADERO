
import tkinter as tk
import sys
from pathlib import Path


TOTAL_CELDAS = 40

# Permite importar la librería SWIG ubicada en esta misma carpeta.
CARPETA_PYTHON = Path(__file__).resolve().parent
sys.path.insert(0, str(CARPETA_PYTHON))

try:
    import parqueadero
    ERROR_SWIG = None
except ImportError as error:
    parqueadero = None
    ERROR_SWIG = str(error)


class VisualizadorParqueadero:
    def __init__(self, ventana):
        self.ventana = ventana
        self.ventana.title("Sistema de Parqueadero ITM")
        self.ventana.geometry("950x700")
        self.ventana.minsize(600, 400)
        self.ventana.configure(bg="#f0f2f5")

        # Objeto C++ expuesto mediante la librería SWIG.
        self.parqueadero = None

        if parqueadero is not None:
            self.parqueadero = parqueadero.Parqueadero()

        contenedor = tk.Frame(ventana, bg="#f0f2f5")
        contenedor.pack(fill="both", expand=True)

        lienzo = tk.Canvas(
            contenedor,
            bg="#f0f2f5",
            highlightthickness=0
        )

        barra = tk.Scrollbar(
            contenedor,
            orient="vertical",
            command=lienzo.yview
        )

        lienzo.configure(yscrollcommand=barra.set)

        barra.pack(side="right", fill="y")
        lienzo.pack(side="left", fill="both", expand=True)

        contenido = tk.Frame(lienzo, bg="#f0f2f5")

        ventana_contenido = lienzo.create_window(
            (0, 0),
            window=contenido,
            anchor="nw"
        )

        contenido.bind(
            "<Configure>",
            lambda evento: lienzo.configure(
                scrollregion=lienzo.bbox("all")
            )
        )

        lienzo.bind(
            "<Configure>",
            lambda evento: lienzo.itemconfigure(
                ventana_contenido,
                width=evento.width
            )
        )

        def rueda_mouse(evento):
            lienzo.yview_scroll(
                int(-1 * (evento.delta / 120)),
                "units"
            )

        lienzo.bind_all("<MouseWheel>", rueda_mouse)

        titulo = tk.Label(
            contenido,
            text="PARQUEADERO ITM",
            font=("Arial", 22, "bold"),
            bg="#f0f2f5",
            fg="#1f2937"
        )
        titulo.pack(pady=15)

        self.resumen = tk.Label(
            contenido,
            text="Ocupadas: 0     Libres: 40",
            font=("Arial", 14, "bold"),
            bg="#f0f2f5"
        )
        self.resumen.pack(pady=5)

        tk.Label(
            contenido,
            text="VERDE: LIBRE       ROJO: OCUPADA",
            font=("Arial", 11, "bold"),
            bg="#f0f2f5"
        ).pack(pady=10)

        self.marco_celdas = tk.Frame(
            contenido,
            bg="#f0f2f5"
        )
        self.marco_celdas.pack(padx=10, pady=10)

        self.celdas = []

        for numero in range(1, TOTAL_CELDAS + 1):
            fila = (numero - 1) // 5
            columna = (numero - 1) % 5

            celda = tk.Label(
                self.marco_celdas,
                text=f"CELDA {numero}\nLIBRE",
                width=16,
                height=4,
                bg="#b7efc5",
                fg="#14532d",
                font=("Arial", 10, "bold"),
                relief="raised",
                borderwidth=2
            )

            celda.grid(
                row=fila,
                column=columna,
                padx=5,
                pady=5
            )

            self.celdas.append(celda)

        self.estado = tk.Label(
            contenido,
            text="Esperando datos del servidor...",
            font=("Arial", 10),
            bg="#f0f2f5",
            fg="#374151"
        )
        self.estado.pack(pady=15)

        self.actualizar_estado()

    def actualizar_estado(self):
        # Comprobar si la librería SWIG se pudo importar.
        if self.parqueadero is None:
            self.estado.config(
                text="Error al cargar la librería SWIG."
            )
            self.ventana.after(1000, self.actualizar_estado)
            return

        try:
            # Cargar en el objeto C++ el estado guardado por el servidor.
            archivo_cargado = (
                self.parqueadero.cargarEstadoArchivo()
            )

            if not archivo_cargado:
                self.estado.config(
                    text=(
                        "Esperando datos: inicia el servidor "
                        "para crear el archivo de estado."
                    )
                )
                self.ventana.after(1000, self.actualizar_estado)
                return

            ocupadas = 0

            # Consultar cada una de las 40 celdas a través de SWIG.
            for numero in range(1, TOTAL_CELDAS + 1):
                esta_ocupada = bool(
                    self.parqueadero.estaOcupada(numero)
                )

                celda = self.celdas[numero - 1]

                if esta_ocupada:
                    ocupadas += 1

                    placa = self.parqueadero.obtenerPlaca(numero)
                    hora = self.parqueadero.obtenerHora(numero)

                    texto = (
                        f"CELDA {numero}\n"
                        f"{placa}\n"
                        f"{hora}"
                    )

                    celda.config(
                        text=texto,
                        bg="#fca5a5",
                        fg="#7f1d1d"
                    )
                else:
                    celda.config(
                        text=f"CELDA {numero}\nLIBRE",
                        bg="#b7efc5",
                        fg="#14532d"
                    )

            libres = TOTAL_CELDAS - ocupadas

            self.resumen.config(
                text=f"Ocupadas: {ocupadas}     Libres: {libres}"
            )

            self.estado.config(
                text="Estado actualizado mediante C++ y SWIG."
            )

        except (OSError, RuntimeError, ValueError) as error:
            self.estado.config(
                text=f"Error al consultar el parqueadero: {error}"
            )

        self.ventana.after(1000, self.actualizar_estado)


if __name__ == "__main__":
    ventana = tk.Tk()
    aplicacion = VisualizadorParqueadero(ventana)
    ventana.mainloop()
