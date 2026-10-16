
import tkinter as tk
import csv
from pathlib import Path


TOTAL_CELDAS = 40

ARCHIVO_ESTADO = (
    Path(__file__).resolve().parent.parent
    / "estado_parqueadero.csv"
)


class VisualizadorParqueadero:
    def __init__(self, ventana):
        self.ventana = ventana
        self.ventana.title("Sistema de Parqueadero ITM")
        self.ventana.geometry("950x700")
        self.ventana.minsize(600, 400)
        self.ventana.configure(bg="#f0f2f5")

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
        if not ARCHIVO_ESTADO.exists():
            self.estado.config(
                text="Esperando datos: inicia el servidor y el cliente."
            )
            self.ventana.after(1000, self.actualizar_estado)
            return

        try:
            with open(
                ARCHIVO_ESTADO,
                "r",
                newline="",
                encoding="utf-8"
            ) as archivo:
                lector = csv.DictReader(archivo)
                datos = list(lector)

            ocupadas = 0

            for dato in datos:
                numero = int(dato["celda"])
                esta_ocupada = dato["ocupada"] == "1"
                placa = dato["placa"]
                hora = dato["hora"]

                if numero < 1 or numero > TOTAL_CELDAS:
                    continue

                celda = self.celdas[numero - 1]

                if esta_ocupada:
                    ocupadas += 1
                    texto = f"CELDA {numero}\n{placa}\n{hora}"
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
                text="Estado actualizado desde el servidor."
            )

        except (OSError, csv.Error, KeyError, ValueError):
            self.estado.config(
                text="Leyendo estado del parqueadero..."
            )

        self.ventana.after(1000, self.actualizar_estado)


if __name__ == "__main__":
    ventana = tk.Tk()
    aplicacion = VisualizadorParqueadero(ventana)
    ventana.mainloop()
