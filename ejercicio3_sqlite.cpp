// Practica 3: Conexion a base de datos (SQLite) - insercion y consulta
// Compilar: g++ ejercicio3_sqlite.cpp -o ejercicio3 -lsqlite3
#include <iostream>
#include <string>
#include <sqlite3.h>
using namespace std;

int main() {
    sqlite3* db = nullptr;

    if (sqlite3_open("tareas.db", &db) != SQLITE_OK) {
        cerr << "No se pudo abrir la base: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    cout << "Conectado a tareas.db" << endl;

    // Empiezo con la tabla limpia para que el programa se pueda correr varias veces
    const char* crear =
        "DROP TABLE IF EXISTS tareas;"
        "CREATE TABLE tareas ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  titulo TEXT NOT NULL,"
        "  hecha INTEGER DEFAULT 0);";
    char* error = nullptr;
    if (sqlite3_exec(db, crear, nullptr, nullptr, &error) != SQLITE_OK) {
        cerr << "Error al crear la tabla: " << error << endl;
        sqlite3_free(error);
        sqlite3_close(db);
        return 1;
    }

    // Inserciones con sentencia preparada
    const string titulos[] = {"Estudiar excepciones en C++",
                              "Entregar practica de SQLite",
                              "Repasar sobrecarga de operadores"};
    sqlite3_stmt* insertar = nullptr;
    sqlite3_prepare_v2(db, "INSERT INTO tareas (titulo) VALUES (?);", -1, &insertar, nullptr);
    for (const string& t : titulos) {
        sqlite3_bind_text(insertar, 1, t.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(insertar) != SQLITE_DONE)
            cerr << "No se pudo insertar: " << t << endl;
        sqlite3_reset(insertar);
    }
    sqlite3_finalize(insertar);
    cout << "Se insertaron 3 tareas." << endl;

    // Consulta
    sqlite3_stmt* consulta = nullptr;
    sqlite3_prepare_v2(db, "SELECT id, titulo, hecha FROM tareas;", -1, &consulta, nullptr);
    cout << "\nID | Tarea                              | Hecha" << endl;
    cout << "---+------------------------------------+------" << endl;
    while (sqlite3_step(consulta) == SQLITE_ROW) {
        int id = sqlite3_column_int(consulta, 0);
        string titulo = reinterpret_cast<const char*>(sqlite3_column_text(consulta, 1));
        int hecha = sqlite3_column_int(consulta, 2);
        cout << id << "  | " << titulo << string(35 - titulo.size(), ' ')
             << "| " << (hecha ? "si" : "no") << endl;
    }
    sqlite3_finalize(consulta);

    sqlite3_close(db);
    return 0;
}
