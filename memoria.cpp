#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <climits>
#include <cstring>

using namespace std;

// Leer referencias de archivo
vector<int> leerReferencias(const string& archivo) {
    vector<int> referencias;
    ifstream file(archivo);
    if (!file) {
        cerr << "Error al abrir el archivo: " << archivo << endl;
        exit(EXIT_FAILURE);
    }

    int referencia;
    while (file >> referencia) {
        referencias.push_back(referencia);
    }
    return referencias;
}

// Algoritmo FIFO usando unordered_map
int reemplazoFIFO(const vector<int>& referencias, int marcos) {
    unordered_map<int, list<int>> tabla;  // Tabla hash con chaining
    list<int> memoria;
    int fallosPagina = 0;

    for (int pagina : referencias) {
        if (tabla.find(pagina) == tabla.end()) {
            fallosPagina++;
            if (memoria.size() >= marcos) {
                int paginaARemover = memoria.front();
                memoria.pop_front();
                tabla.erase(paginaARemover);  // Eliminar la página
            }
            memoria.push_back(pagina);
            tabla[pagina].push_back(pagina);  // Insertar en la tabla hash
        }
    }

    return fallosPagina;
}

// Algoritmo LRU usando unordered_map
int reemplazoLRU(const vector<int>& referencias, int marcos) {
    unordered_map<int, list<int>> tabla;
    unordered_map<int, int> ultimaUso;  // Guardar el último uso de cada página
    int fallosPagina = 0;
    vector<int> memoria;

    for (size_t i = 0; i < referencias.size(); ++i) {
        int pagina = referencias[i];
        if (tabla.find(pagina) == tabla.end()) {
            fallosPagina++;
            if (memoria.size() >= marcos) {
                int lru = INT_MAX, paginaARemover = -1;
                for (int p : memoria) {
                    if (ultimaUso[p] < lru) {
                        lru = ultimaUso[p];
                        paginaARemover = p;
                    }
                }
                memoria.erase(remove(memoria.begin(), memoria.end(), paginaARemover), memoria.end());
                tabla.erase(paginaARemover);  // Eliminar la página
            }
            memoria.push_back(pagina);
            tabla[pagina].push_back(pagina);  // Insertar en la tabla hash
        }
        ultimaUso[pagina] = i;
    }

    return fallosPagina;
}

// Algoritmo Clock usando unordered_map
int reemplazoClock(const vector<int>& referencias, int marcos) {
    unordered_map<int, list<int>> tabla;
    vector<pair<int, bool>> memoria(marcos, {-1, false});
    int puntero = 0, fallosPagina = 0;

    for (int pagina : referencias) {
        bool encontrada = false;
        for (auto& m : memoria) {
            if (m.first == pagina) {
                m.second = true;  // Actualizar bit de uso
                encontrada = true;
                break;
            }
        }

        if (!encontrada) {
            fallosPagina++;
            while (true) {
                if (!memoria[puntero].second) {
                    if (memoria[puntero].first != -1) {
                        tabla.erase(memoria[puntero].first);  // Eliminar la página
                    }
                    memoria[puntero] = {pagina, true};
                    tabla[pagina].push_back(pagina);  // Insertar en la tabla hash
                    puntero = (puntero + 1) % marcos;
                    break;
                } else {
                    memoria[puntero].second = false;  // Resetear bit de uso
                    puntero = (puntero + 1) % marcos;
                }
            }
        }
    }

    return fallosPagina;
}

// Algoritmo Óptimo usando unordered_map
int reemplazoOptimo(const vector<int>& referencias, int marcos) {
    unordered_map<int, list<int>> tabla;
    vector<int> memoria;
    int fallosPagina = 0;

    for (size_t i = 0; i < referencias.size(); ++i) {
        int pagina = referencias[i];
        if (tabla.find(pagina) == tabla.end()) {
            fallosPagina++;
            if (memoria.size() >= marcos) {
                int maxDistancia = -1, paginaARemover = -1;
                for (int p : memoria) {
                    int distancia = INT_MAX;
                    for (size_t k = i + 1; k < referencias.size(); ++k) {
                        if (referencias[k] == p) {
                            distancia = k;
                            break;
                        }
                    }
                    if (distancia > maxDistancia) {
                        maxDistancia = distancia;
                        paginaARemover = p;
                    }
                }
                memoria.erase(remove(memoria.begin(), memoria.end(), paginaARemover), memoria.end());
                tabla.erase(paginaARemover);  // Eliminar la página
            }
            memoria.push_back(pagina);
            tabla[pagina].push_back(pagina);  // Insertar en la tabla hash
        }
    }

    return fallosPagina;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Uso: " << argv[0] << " -m <marcos> -a <algoritmo> -f <archivo>" << endl;
        return EXIT_FAILURE;
    }

    int marcos = 0;
    string algoritmo, archivoReferencias;

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-m") == 0) marcos = stoi(argv[i + 1]);
        else if (strcmp(argv[i], "-a") == 0) algoritmo = argv[i + 1];
        else if (strcmp(argv[i], "-f") == 0) archivoReferencias = argv[i + 1];
    }

    vector<int> referencias = leerReferencias(archivoReferencias);
    int fallosPagina = 0;

    if (algoritmo == "FIFO") fallosPagina = reemplazoFIFO(referencias, marcos);
    else if (algoritmo == "LRU") fallosPagina = reemplazoLRU(referencias, marcos);
    else if (algoritmo == "CLOCK") fallosPagina = reemplazoClock(referencias, marcos);
    else if (algoritmo == "OPTIMO") fallosPagina = reemplazoOptimo(referencias, marcos);
    else {
        cerr << "Algoritmo no reconocido: " << algoritmo << endl;
        return EXIT_FAILURE;
    }

    cout << "Número de fallos de página: " << fallosPagina << endl;
    return 0;
}
