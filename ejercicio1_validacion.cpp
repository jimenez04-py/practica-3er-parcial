// Practica 1: Validacion con excepciones (edad y correo)
#include <iostream>
#include <string>
#include <stdexcept>
#include <regex>
using namespace std;

// Excepcion propia para cuando el correo no tiene el formato correcto
class CorreoInvalido : public runtime_error {
public:
    CorreoInvalido(const string& correo)
        : runtime_error("El correo '" + correo + "' no tiene un formato valido") {}
};

int leerEdad(const string& texto) {
    size_t pos = 0;
    int edad;
    try {
        edad = stoi(texto, &pos);
    } catch (const invalid_argument&) {
        throw invalid_argument("La edad debe ser un numero");
    } catch (const out_of_range&) {
        throw out_of_range("Ese numero es demasiado grande");
    }
    // stoi acepta cosas como "12abc", asi que reviso que se haya leido todo
    if (pos != texto.size())
        throw invalid_argument("La edad solo debe llevar digitos");
    if (edad < 0 || edad > 120)
        throw out_of_range("La edad debe estar entre 0 y 120");
    return edad;
}

void validarCorreo(const string& correo) {
    const regex formato("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!regex_match(correo, formato))
        throw CorreoInvalido(correo);
}

int main() {
    int edad = 0;
    string correo;
    string entrada;

    // Pido la edad hasta que sea valida
    while (true) {
        cout << "Edad: ";
        getline(cin, entrada);
        try {
            edad = leerEdad(entrada);
            break;
        } catch (const invalid_argument& e) {
            cout << "  Error (formato): " << e.what() << endl;
        } catch (const out_of_range& e) {
            cout << "  Error (rango): " << e.what() << endl;
        }
    }

    // Lo mismo con el correo
    while (true) {
        cout << "Correo: ";
        getline(cin, correo);
        try {
            validarCorreo(correo);
            break;
        } catch (const CorreoInvalido& e) {
            cout << "  Error: " << e.what() << endl;
        }
    }

    cout << "\nDatos guardados -> edad: " << edad << ", correo: " << correo << endl;
    return 0;
}
