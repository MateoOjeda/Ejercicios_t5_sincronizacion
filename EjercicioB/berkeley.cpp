
#include "red.hpp"
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso:\n";
        cout << "Servidor: berkeley.exe servidor\n";
        cout << "Cliente: berkeley.exe cliente <desfase_ms>\n";
        return 1;
    }

    string modo = argv[1];
    iniciarRed();

    try {
        if (modo == "servidor") {
            SOCKET servidor = crearServidor(6000);

            vector<SOCKET> clientes;
            vector<long long> horas;

            long long horaCoordinador = horaMs();

            cout << "Coordinador Berkeley iniciado\n";
            cout << "Esperando 3 clientes...\n";

            for (int i = 0; i < 3; i++) {
                SOCKET cliente = accept(
                    servidor, nullptr, nullptr
                );

                if (cliente == INVALID_SOCKET)
                    throw runtime_error("Error en accept");

                long long hora = stoll(recibir(cliente));

                clientes.push_back(cliente);
                horas.push_back(hora);

                cout << "Cliente " << i + 1
                     << " | Hora recibida: "
                     << hora << "\n";
            }

            long long suma = horaCoordinador;

            for (long long hora : horas)
                suma += hora;

            long long promedio = suma / 4;

            cout << "\nPromedio calculado: "
                 << promedio << "\n";

            cout << "Correccion coordinador: "
                 << promedio - horaCoordinador
                 << " ms\n";

            for (int i = 0; i < 3; i++) {
                long long ajuste = promedio - horas[i];

                enviar(clientes[i], to_string(ajuste));

                cout << "Ajuste cliente " << i + 1
                     << ": " << ajuste << " ms\n";

                closesocket(clientes[i]);
            }

            closesocket(servidor);
        }
        else if (modo == "cliente" && argc == 3) {
            long long desfase = stoll(argv[2]);

            SOCKET servidor = conectar("127.0.0.1", 6000);

            long long relojLocal = horaMs() + desfase;

            cout << "Desfase inicial: "
                 << desfase << " ms\n";

            enviar(servidor, to_string(relojLocal));

            long long ajuste = stoll(recibir(servidor));

            desfase += ajuste;

            cout << "Ajuste recibido: "
                 << ajuste << " ms\n";

            cout << "Desfase actualizado: "
                 << desfase << " ms\n";

            closesocket(servidor);
        }
        else {
            throw runtime_error("Argumentos invalidos");
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        cerrarRed();
        return 1;
    }

    cerrarRed();
    return 0;
}
