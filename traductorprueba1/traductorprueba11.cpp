#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <windows.h> // Cabecera para PlaySound
#include <thread>
#include <chrono>
#include <functional> // Para std::hash
#include <conio.h> // Para _getch()

// Estructura de Nodo para el Arbol Binario de Busqueda (BST)
struct Nodo {
    size_t palabra_hash; // Almacenamos el hash de la palabra en lugar de la palabra misma
    std::unordered_map<std::string, std::string> traducciones;
    std::unordered_map<std::string, std::string> audios;
    Nodo* izq;
    Nodo* der;

    Nodo(size_t palabra_hash, const std::unordered_map<std::string, std::string>& traducciones, const std::unordered_map<std::string, std::string>& audios)
        : palabra_hash(palabra_hash), traducciones(traducciones), audios(audios), izq(nullptr), der(nullptr) {}
};

// Clase del Arbol Binario de Busqueda
class ArbolBinarioBusqueda {
public:
    ArbolBinarioBusqueda() : raiz(nullptr) {}

    void insertar(const std::string& palabra, const std::unordered_map<std::string, std::string>& traducciones, const std::unordered_map<std::string, std::string>& audios) {
        size_t palabra_hash = std::hash<std::string>()(palabra); // Calcular el hash de la palabra
        raiz = insertarRecursivo(raiz, palabra_hash, traducciones, audios);
    }

    std::string buscar(const std::string& palabra, const std::string& idioma, std::string& archivoAudio) {
        size_t palabra_hash = std::hash<std::string>()(palabra); // Calcular el hash de la palabra
        Nodo* resultado = buscarRecursivo(raiz, palabra_hash);
        if (resultado != nullptr) {
            archivoAudio = resultado->audios.at(idioma);
            return resultado->traducciones.at(idioma);
        }
        return "Palabra no encontrada";
    }

private:
    Nodo* raiz;

    Nodo* insertarRecursivo(Nodo* nodo, size_t palabra_hash, const std::unordered_map<std::string, std::string>& traducciones, const std::unordered_map<std::string, std::string>& audios) {
        if (nodo == nullptr) {
            return new Nodo(palabra_hash, traducciones, audios);
        }
        if (palabra_hash < nodo->palabra_hash) {
            nodo->izq = insertarRecursivo(nodo->izq, palabra_hash, traducciones, audios);
        } else if (palabra_hash > nodo->palabra_hash) {
            nodo->der = insertarRecursivo(nodo->der, palabra_hash, traducciones, audios);
        }
        return nodo;
    }

    Nodo* buscarRecursivo(Nodo* nodo, size_t palabra_hash) {
        if (nodo == nullptr || nodo->palabra_hash == palabra_hash) {
            return nodo;
        }
        if (palabra_hash < nodo->palabra_hash) {
            return buscarRecursivo(nodo->izq, palabra_hash);
        }
        return buscarRecursivo(nodo->der, palabra_hash);
    }
};

// Función para encriptar usando el cifrado César
std::string encriptar(const std::string& texto, int desplazamiento) {
    std::string resultado;
    for (char c : texto) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base + desplazamiento) % 26 + base;
        }
        resultado += c;
    }
    return resultado;
}

// Función para desencriptar usando el cifrado César
std::string desencriptar(const std::string& texto, int desplazamiento) {
    return encriptar(texto, 26 - desplazamiento); // Desencriptar es encriptar con el desplazamiento inverso
}

// Funcion para leer el diccionario desde un archivo
void cargarDiccionario(const std::string& nombreArchivo, ArbolBinarioBusqueda& arbol) {
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "Error abriendo el archivo " << nombreArchivo << std::endl;
        return;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string palabraEsp, traduccionIng, audioIng, traduccionAle, audioAle, traduccionFra, audioFra, traduccionIta, audioIta;
        std::getline(ss, palabraEsp, '|');
        std::getline(ss, traduccionIng, '|');
        std::getline(ss, audioIng, '|');
        std::getline(ss, traduccionAle, '|');
        std::getline(ss, audioAle, '|');
        std::getline(ss, traduccionFra, '|');
        std::getline(ss, audioFra, '|');
        std::getline(ss, traduccionIta, '|');
        std::getline(ss, audioIta, '|');
        std::unordered_map<std::string, std::string> traducciones = {
            {"ingles", traduccionIng},
            {"aleman", traduccionAle},
            {"frances", traduccionFra},
            {"italiano", traduccionIta}
        };
        std::unordered_map<std::string, std::string> audios = {
            {"ingles", audioIng},
            {"aleman", audioAle},
            {"frances", audioFra},
            {"italiano", audioIta}
        };
        arbol.insertar(palabraEsp, traducciones, audios);
    }
}



// Funcion para guardar el diccionario en un archivo encriptado
void guardarDiccionarioEncriptado(const std::string& nombreArchivo, ArbolBinarioBusqueda& arbol, int desplazamiento) {
    std::ifstream archivoEntrada(nombreArchivo);
    if (!archivoEntrada.is_open()) {
        std::cerr << "Error abriendo el archivo " << nombreArchivo << std::endl;
        return;
    }

    std::ofstream archivoSalida("diccionario_encriptado.txt");
    std::string linea;
    while (std::getline(archivoEntrada, linea)) {
        std::stringstream ss(linea);
        std::string palabraEsp, traduccionIng, audioIng, traduccionAle, audioAle, traduccionFra, audioFra, traduccionIta, audioIta;
        std::getline(ss, palabraEsp, '|');
        std::getline(ss, traduccionIng, '|');
        std::getline(ss, audioIng, '|');
        std::getline(ss, traduccionAle, '|');
        std::getline(ss, audioAle, '|');
        std::getline(ss, traduccionFra, '|');
        std::getline(ss, audioFra, '|');
        std::getline(ss, traduccionIta, '|');
        std::getline(ss, audioIta, '|');
        archivoSalida << palabraEsp << '|'
                      << encriptar(traduccionIng, desplazamiento) << '|' << audioIng << '|'
                      << encriptar(traduccionAle, desplazamiento) << '|' << audioAle << '|'
                      << encriptar(traduccionFra, desplazamiento) << '|' << audioFra << '|'
                      << encriptar(traduccionIta, desplazamiento) << '|' << audioIta << '\n';
    }
}

// Funcion para guardar el diccionario desencriptado en un archivo
void guardarDiccionarioDesencriptado(const std::string& nombreArchivo, int desplazamiento) {
    std::ifstream archivoEntrada(nombreArchivo);
    if (!archivoEntrada.is_open()) {
        std::cerr << "Error abriendo el archivo " << nombreArchivo << std::endl;
        return;
    }

    std::ofstream archivoSalida("diccionario_desencriptado.txt");
    std::string linea;
    while (std::getline(archivoEntrada, linea)) {
        std::stringstream ss(linea);
        std::string palabraEsp, traduccionIng, audioIng, traduccionAle, audioAle, traduccionFra, audioFra, traduccionIta, audioIta;
        std::getline(ss, palabraEsp, '|');
        std::getline(ss, traduccionIng, '|');
        std::getline(ss, audioIng, '|');
        std::getline(ss, traduccionAle, '|');
        std::getline(ss, audioAle, '|');
        std::getline(ss, traduccionFra, '|');
        std::getline(ss, audioFra, '|');
        std::getline(ss, traduccionIta, '|');
        std::getline(ss, audioIta, '|');
        archivoSalida << palabraEsp << '|'
                      << desencriptar(traduccionIng, desplazamiento) << '|' << audioIng << '|'
                      << desencriptar(traduccionAle, desplazamiento) << '|' << audioAle << '|'
                      << desencriptar(traduccionFra, desplazamiento) << '|' << audioFra << '|'
                      << desencriptar(traduccionIta, desplazamiento) << '|' << audioIta << '\n';
    }
}

// Funcion para reproducir el audio
void reproducirAudio(const std::string& archivoAudio) {
    if (PlaySound(archivoAudio.c_str(), NULL, SND_FILENAME | SND_ASYNC)) {
        std::cout << "Reproduciendo " << archivoAudio << std::endl;
    } else {
        std::cerr << "Error al reproducir el audio: " << archivoAudio << std::endl;
    }
}

// Funcion para mostrar la interfaz de inicio de sesion
void mostrarInterfazInicioSesion() {
    std::cout << " ____________________________________________________________________________________________________________" << std::endl;
    std::cout << "|                                                                                                            |" << std::endl;
    std::cout << "|                                          Inicio de sesion                                                  |" << std::endl;
    std::cout << "|____________________________________________________________________________________________________________|" << std::endl;
}

// Funcion para mostrar la interfaz de transicion
void mostrarInterfazTransicion() {
    std::cout << "|************************************************************************************************************|" << std::endl;
    std::cout << "|                                            Traductor ABC                                                   |" << std::endl;
    std::cout << "|____________________________________________________________________________________________________________|" << std::endl;
}

// Función para obtener la contraseña de forma segura sin mostrarla en pantalla
std::string getpass() {
    const char BACKSPACE = 8;
    const char RETURN = 13;

    std::string password;
    unsigned char ch = 0;

    while ((ch = _getch()) != RETURN) {
        if (ch == BACKSPACE) {
            if (password.length() != 0) {
                std::cout << "\b \b";
                password.resize(password.length() - 1);
            }
        } else {
            std::cout << '*';
            password += ch;
        }
    }
    std::cout << std::endl;
    return password;
}

// Funcion para el inicio de sesion
bool iniciarSesion() {
    std::string usuario, contrasena;
    std::cout << "Usuario: ";
    std::cin >> usuario;
    std::cout << "Contrasena: ";
    contrasena = getpass();
    return (usuario == "Admin" && contrasena == "admin");
}

int main() {
    mostrarInterfazInicioSesion();
    while (true) {
        if (!iniciarSesion()) {
            std::cout << "Inicio de sesion fallido. Usuario o contrasena incorrectos. Intente nuevamente." << std::endl;
            continue;
        } else {
            std::cout << "Inicio de sesion exitoso." << std::endl;
            break;
        }
    }

    mostrarInterfazTransicion();
    ArbolBinarioBusqueda arbol;
    cargarDiccionario("diccionario.txt", arbol);

    int opcion;
    std::string palabra, idioma, archivoAudio;

    while (true) {
        std::cout << "Menu principal:\n";
        std::cout << "1. Traductor\n";
        std::cout << "2. Encriptar diccionario\n";
        std::cout << "3. Desencriptar diccionario\n";
        std::cout << "4. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        if (opcion == 4) {
            break;
        }

        if (opcion == 1) {
            std::cout << "1. Ingles\n";
            std::cout << "2. Aleman\n";
            std::cout << "3. Frances\n";
            std::cout << "4. Italiano\n";
            std::cout << "Seleccione una opcion: ";
            std::cin >> opcion;

            if (opcion == 5) {
                break;
            }

            std::cout << "Ingrese la palabra en espanol: ";
            std::cin >> palabra;

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
                    std::cout << "Opcion no valida" << std::endl;
                    continue;
            }

            std::string traduccion = arbol.buscar(palabra, idioma, archivoAudio);
            if (traduccion != "Palabra no encontrada") {
                std::cout << "La traduccion de '" << palabra << "' en " << idioma << " es: " << traduccion << std::endl;
                reproducirAudio(archivoAudio);
            } else {
                std::cout << traduccion << std::endl;
            }
        } else if (opcion == 2) {
            int desplazamiento;
            std::cout << "Ingrese el desplazamiento para la encriptacion: ";
            std::cin >> desplazamiento;
            guardarDiccionarioEncriptado("diccionario.txt", arbol, desplazamiento);
            std::cout << "Diccionario encriptado guardado en 'diccionario_encriptado.txt'" << std::endl;
        } else if (opcion == 3) {
            int desplazamiento;
            std::cout << "Ingrese el desplazamiento para la desencriptacion: ";
            std::cin >> desplazamiento;
            guardarDiccionarioDesencriptado("diccionario_encriptado.txt", desplazamiento);
            std::cout << "Diccionario desencriptado guardado en 'diccionario_desencriptado.txt'" << std::endl;
        } else {
            std::cout << "Opcion no valida" << std::endl;
        }
    }

    return 0;
}

