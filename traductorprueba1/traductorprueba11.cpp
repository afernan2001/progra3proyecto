#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <windows.h> // Cabecera para PlaySound
#include <thread>
#include <chrono>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>

using namespace std;
using namespace CryptoPP;

// Función para cifrar una cadena usando SHA-256
string cifrarSHA256(const string& input) {
    string output;
    SHA256 hash;
    StringSource s(input, true,
        new HashFilter(hash,
            new HexEncoder(new StringSink(output)));
    return output;
}

// Estructura de Nodo para el Arbol Binario de Busqueda (BST)
struct Nodo {
    string palabra;
    unordered_map<string, string> traducciones;
    unordered_map<string, string> audios;
    Nodo* izq;
    Nodo* der;

    Nodo(const string& palabra, const unordered_map<string, string>& traducciones, const unordered_map<string, string>& audios)
        : palabra(palabra), traducciones(traducciones), audios(audios), izq(nullptr), der(nullptr) {}
};

// Clase del Arbol Binario de Busqueda
class ArbolBinarioBusqueda {
public:
    ArbolBinarioBusqueda() : raiz(nullptr) {}

    void insertar(const string& palabra, const unordered_map<string, string>& traducciones, const unordered_map<string, string>& audios) {
        raiz = insertarRecursivo(raiz, palabra, traducciones, audios);
    }

    string buscar(const string& palabra, const string& idioma, string& archivoAudio) {
        Nodo* resultado = buscarRecursivo(raiz, palabra);
        if (resultado!= nullptr) {
            archivoAudio = resultado->audios.at(idioma);
            return resultado->traducciones.at(idioma);
        }
        return "Palabra no encontrada";
    }

private:
    Nodo* raiz;

    Nodo* insertarRecursivo(Nodo* nodo, const string& palabra, const unordered_map<string, string>& traducciones, const unordered_map<string, string>& audios) {
        if (nodo == nullptr) {
            return new Nodo(palabra, traducciones, audios);
        }
        if (palabra < nodo->palabra) {
            nodo->izq = insertarRecursivo(nodo->izq, palabra, traducciones, audios);
        } else if (palabra > nodo->palabra) {
            nodo->der = insertarRecursivo(nodo->der, palabra, traducciones, audios);
        }
        return nodo;
    }

    Nodo* buscarRecursivo(Nodo* nodo, const string& palabra) {
        if (nodo == nullptr || nodo->palabra == palabra) {
            return nodo;
        }
        if (palabra < nodo->palabra) {
            return buscarRecursivo(nodo->izq, palabra);
        }
        return buscarRecursivo(nodo->der, palabra);
    }
};

// Funcion para leer el diccionario desde un archivo
void cargarDiccionario(const string& nombreArchivo, ArbolBinarioBusqueda& arbol) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error abriendo el archivo " << nombreArchivo << endl;
        return;
    }

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string palabraEsp, traduccionIng, audioIng, traduccionAle, audioAle, traduccionFra, audioFra, traduccionIta, audioIta;
        getline(ss, palabraEsp, '|');
        getline(ss, traduccionIng, '|');
        getline(ss, audioIng, '|');
        getline(ss, traduccionAle, '|');
        getline(ss, audioAle, '|');
        getline(ss, traduccionFra, '|');
        getline(ss, audioFra, '|');
        getline(ss, traduccionIta, '|');
        getline(ss, audioIta, '|');

        // Cifrar las palabras y los nombres de los archivos de audio
        traduccionIng = cifrarSHA256(traduccionIng);
        audioIng = cifrarSHA256(audioIng);
        traduccionAle = cifrarSHA256(traduccionAle);
        audioAle = cifrarSHA256(audioAle);
        traduccionFra = cifrarSHA256(traduccionFra);
        audioFra = cifrarSHA256(audioFra);
        traduccionIta = cifrarSHA256(traduccionIta);
        audioIta = cifrarSHA256(audioIta);

        unordered_map<string, string> traducciones = {
            {"ingles", traduccionIng},
            {"aleman", traduccionAle},
            {"frances", traduccionFra},
            {"italiano", traduccionIta}
        };
        unordered_map<string, string> audios = {
            {"ingles", audioIng},
            {"aleman", audioAle},
            {"frances", audioFra},
            {"italiano", audioIta}
        };
        arbol.insertar(palabraEsp, traducciones, audios);
    }
}

// Funcion para reproducir el audio
void reproducirAudio(const string& archivoAudio) {
    if (PlaySound(archivoAudio.c_str(), NULL, SND_FILENAME | SND_ASYNC)) {
        cout << "Reproduciendo " << archivoAudio << endl;
    } else {
        cerr << "Error al reproducir el audio: " << archivoAudio << endl;
    }
}

// Funcion para mostrar la interfaz de inicio de sesion
void mostrarInterfazInicioSesion() {
    cout << " ____________________________________________________________________________________________________________" << endl;
    cout << "|                                                                                                            |" << endl;
    cout << "|                                          Inicio de sesion                                                  |" << endl;
    cout << "||" << endl;
}

// Funcion para mostrar la interfaz de transicion
void mostrarInterfazTransicion() {
    cout << " ************************************************************************************************************" << endl;
    cout << "|                                            Traductor ABC                                                   | " << endl;
    cout << "||" << endl;
}

// Funcion para el inicio de sesion
bool iniciarSesion() {
    string usuario, contrasena;
    cout << "Usuario: ";
    cin >> usuario;
    cout << "Contrasena: ";
    cin >> contrasena;
    return (usuario == "Administrador" && contrasena == "admin");
}

int main() {
    mostrarInterfazInicioSesion();
    if (!iniciarSesion()) {
        cout << "Inicio de sesion fallido. Saliendo del programa." << endl;
        return 0;
    }
    
    mostrarInterfazTransicion();
    ArbolBinarioBusqueda arbol;
    cargarDiccionario("diccionario.txt", arbol);

    int opcion;
    string palabra, idioma, archivoAudio;

    while (true) {
        cout << "1. Ingles\n";
        cout << "2. Aleman\n";
        cout << "3. Frances\n";
        cout << "4. Italiano\n";
        cout << "5. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 5) {
            break;
        }

        cout << "Ingrese la palabra en espanol: ";
        cin >> palabra;

        switch (opcion) {
            case 1:
                idioma = "ingles";
                break;
            case 2:
                idioma = "aleman";
                break;
            case 3:
                idioma = "frances";
                break;
            case 4:
                idioma = "italiano";
                break;
            default:
                cout << "Opcion no valida" << endl;
                continue;
        }

        string traduccion = arbol.buscar(palabra, idioma, archivoAudio);
        if (traduccion!= "Palabra no encontrada") {
            cout << "La traduccion de '" << palabra << "' en " << idioma << " es: " << traduccion << endl;
            reproducirAudio(archivoAudio);
        } else {
            cout << traduccion << endl;
        }
    }

    return 0;
}

