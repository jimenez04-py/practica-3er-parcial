// Practica 5: Integracion de archivo + base de datos con manejo de errores
// Compilar: g++ ejercicio5_integracion.cpp -o ejercicio5 -lsqlite3
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sqlite3.h>
using namespace std;

class ErrorArchivo : public runtime_error {
public:
    ErrorArchivo(const string& ruta)
        : runtime_error("No se pudo abrir el archivo '" + ruta + "'. Revisa que exista.") {}
};

class ErrorBD : public runtime_error {
public:
    ErrorBD(const string& detalle)
        : runtime_error("Fallo la base de datos: " + detalle) {}
};

vector<string> leerLineas(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo) throw ErrorArchivo(ruta);
    vector<string> lineas;
    string l;
    while (getline(archivo, l)) {
        if (!l.empty()) lineas.push_back(l);
    }
    return lineas;
}

sqlite3* abrirBD(const string& ruta) {
    sqlite3* db = nullptr;
    int r = sqlite3_open_v2(ruta.c_str(), &db,
                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (r != SQLITE_OK) {
        string msg = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw ErrorBD(msg + " (ruta: " + ruta + ")");
    }
    return db;
}

void ejecutar(sqlite3* db, const string& sql) {
    char* error = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
        string msg = error;
        sqlite3_free(error);
        throw ErrorBD(msg);
    }
}

int main() {
    // Caso 1: archivo que no existe
    cout << "[1] Leyendo no_existe.txt ..." << endl;
    try {
        vector<string> datos = leerLineas("no_existe.txt");
        cout << "    Se leyeron " << datos.size() << " lineas" << endl;
    } catch (const ErrorArchivo& e) {
        cout << "    ERROR: " << e.what() << endl;
    }

    // Caso 2: base de datos en una carpeta que no existe
    cout << "\n[2] Conectando a /ruta/inexistente/inventario.db ..." << endl;
    try {
        sqlite3* db = abrirBD("/ruta/inexistente/inventario.db");
        sqlite3_close(db);
    } catch (const ErrorBD& e) {
        cout << "    ERROR: " << e.what() << endl;
    }

    // Caso 3: todo bien, el programa sigue su curso normal
    cout << "\n[3] Flujo normal: productos.txt -> inventario.db" << endl;
    {
        ofstream f("productos.txt");
        f << "Cuaderno\nLapiz\nCalculadora\n";
    }
    try {
        vector<string> productos = leerLineas("productos.txt");
        sqlite3* db = abrirBD("inventario.db");
        ejecutar(db, "DROP TABLE IF EXISTS productos;"
                     "CREATE TABLE productos (id INTEGER PRIMARY KEY, nombre TEXT);");
        sqlite3_stmt* insertar = nullptr;
        if (sqlite3_prepare_v2(db, "INSERT INTO productos (nombre) VALUES (?);",
                               -1, &insertar, nullptr) != SQLITE_OK)
            throw ErrorBD(sqlite3_errmsg(db));
        for (const string& p : productos) {
            sqlite3_bind_text(insertar, 1, p.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(insertar) != SQLITE_DONE)
                throw ErrorBD(sqlite3_errmsg(db));
            sqlite3_reset(insertar);
        }
        sqlite3_finalize(insertar);

        sqlite3_stmt* q = nullptr;
        sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM productos;", -1, &q, nullptr);
        if (sqlite3_step(q) == SQLITE_ROW)
            cout << "    Productos guardados en la base: " << sqlite3_column_int(q, 0) << endl;
        sqlite3_finalize(q);
        sqlite3_close(db);
    } catch (const exception& e) {
        cout << "    ERROR: " << e.what() << endl;
    }

    cout << "\nEl programa termino sin tronar." << endl;
    return 0;
}
