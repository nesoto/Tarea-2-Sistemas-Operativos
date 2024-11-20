# Tarea 2 Sistemas Operativos
## Integrantes
- [Nicolás Soto](https://github.com/nesoto)
- [Alejandro Neira](https://github.com/Aneira22)
- [Erick Saldívar](https://github.com/easaldivar)
## Parte 1: Creacion de un Monitor para manejo de colas
Para hacer uso del monitor se debe ejecutar el programa con los siguientes comandos:
```bash
$ ./monitor -p <numero_de_productores> -c <numero_de_consumidores> -s <tamaño_inicial_cola> -t <tiempo_de_espera_max>
```
### Ejemplo
```bash
$ ./monitor -p 10 -c 5 -s 50 -t 1
```
### Importante
El programa se desarrollo en un sistema WSL (Windows Subsystem for Linux) por lo que se recomienda ejecutarlo en un sistema similar para evitar problemas de compatibilidad.

## Parte 2: Creacion de simulador de memoria virtual
Para hacer uso del simulador de memoria virtual se debe ejecutar el programa con los siguientes comandos:
```bash
$ ./mvirtual -m <numero_de_marcos> -a <algoritmo> -f <archivo_de_referencias>
```
### Ejemplo
```bash
$ ./mvirtual -m 10 -a FIFO -f referencias.txt
```
### Importante
- El archivo de referencias debe tener formato "0 3 1 2" donde cada número representa una referencia a una página y llamarse referencias.txt
- Los algoritmos disponibles son:
    - FIFO
    - LRU
    - CLOCK
    - OPTIMO