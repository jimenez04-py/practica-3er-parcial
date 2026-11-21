// Practica 4: Uso de una libreria externa (zlib) para comprimir texto
// Instalar la libreria (Ubuntu): sudo apt install zlib1g-dev
// En Mac ya viene incluida con las herramientas de desarrollo.
// Compilar: g++ ejercicio4_libreria.cpp -o ejercicio4 -lz
#include <iostream>
#include <string>
#include <stdexcept>
#include <zlib.h>
using namespace std;

string comprimir(const string& texto) {
    uLongf tam = compressBound(texto.size());
    string salida(tam, '\0');
    int r = compress(reinterpret_cast<Bytef*>(&salida[0]), &tam,
                     reinterpret_cast<const Bytef*>(texto.data()), texto.size());
    if (r != Z_OK) throw runtime_error("zlib no pudo comprimir el texto");
    salida.resize(tam);
    return salida;
}

string descomprimir(const string& datos, size_t tamOriginal) {
    uLongf tam = tamOriginal;
    string salida(tam, '\0');
    int r = uncompress(reinterpret_cast<Bytef*>(&salida[0]), &tam,
                       reinterpret_cast<const Bytef*>(datos.data()), datos.size());
    if (r != Z_OK) throw runtime_error("zlib no pudo descomprimir los datos");
    salida.resize(tam);
    return salida;
}

int main() {
    string texto;
    for (int i = 0; i < 40; i++) {
        texto += "El manejo de excepciones hace que el programa no truene. ";
    }

    try {
        string comprimido = comprimir(texto);
        string recuperado = descomprimir(comprimido, texto.size());

        double ahorro = 100.0 * (1.0 - (double)comprimido.size() / texto.size());
        cout << "Tamano original:   " << texto.size() << " bytes" << endl;
        cout << "Tamano comprimido: " << comprimido.size() << " bytes" << endl;
        cout << "Ahorro: " << ahorro << "%" << endl;
        cout << "Version de zlib: " << zlibVersion() << endl;
        cout << "El texto recuperado es igual al original? "
             << (recuperado == texto ? "si" : "no") << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
