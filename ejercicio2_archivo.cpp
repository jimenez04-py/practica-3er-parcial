// Practica 2: Archivo de texto (guardar y leer una lista de tareas)
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

const string ARCHIVO = "tareas.txt";

void guardarTareas() {
    vector<string> tareas;
    string linea;

    cout << "Escribe tus tareas (minimo 5). Linea vacia para terminar." << endl;
    while (true) {
        cout << tareas.size() + 1 << "> ";
        if (!getline(cin, linea)) break;
        if (linea.empty()) {
            if (tareas.size() >= 5) break;
            cout << "  Faltan tareas, necesito al menos 5." << endl;
            continue;
        }
        tareas.push_back(linea);
    }

    ofstream salida(ARCHIVO);
    for (const string& t : tareas) {
        salida << t << "\n";
    }
    cout << "Se guardaron " << tareas.size() << " tareas en " << ARCHIVO << endl;
}

void mostrarTareas() {
    ifstream entrada(ARCHIVO);
    if (!entrada) {
        cout << "Todavia no hay tareas guardadas." << endl;
        return;
    }
    string linea;
    int n = 1;
    cout << "--- Tareas pendientes ---" << endl;
    while (getline(entrada, linea)) {
        cout << n++ << ". " << linea << endl;
    }
}

int main() {
    int opcion = 0;
    do {
        cout << "\n1) Guardar tareas\n2) Mostrar tareas\n3) Salir\nOpcion: ";
        string texto;
        getline(cin, texto);
        opcion = texto.empty() ? 0 : texto[0] - '0';

        if (opcion == 1) guardarTareas();
        else if (opcion == 2) mostrarTareas();
        else if (opcion != 3) cout << "Opcion no valida." << endl;
    } while (opcion != 3);
    return 0;
}
