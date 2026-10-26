
#include "red.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso:\n";
        cout << "Servidor: cristian.exe servidor\n";
        cout << "Cliente: cristian.exe cliente <desfase_ms>\n";
        return 1;
    }

    string modo = argv[1];
    iniciarRed();

    try {
        if (modo == "servidor") {
            SOCKET servidor = crearServidor(7000);

            cout << "Servidor de tiempo iniciado\n";
            cout << "Esperando cliente...\n";

            SOCKET cliente = accept(
                servidor, nullptr, nullptr
            );

            if (cliente == INVALID_SOCKET)
                throw runtime_error("Error en accept");

            string solicitud = recibir(cliente);

            if (solicitud == "HORA") {
                long long hora = horaMs();

                enviar(cliente, to_string(hora));

                cout << "Hora enviada: "
                     << hora << " ms\n";
            }

            closesocket(cliente);
            closesocket(servidor);
        }
        else if (modo == "cliente" && argc == 3) {
            long long desfase = stoll(argv[2]);

            SOCKET servidor = conectar("127.0.0.1", 7000);

            long long inicio = horaMs();

            enviar(servidor, "HORA");

            long long horaServidor = stoll(
                recibir(servidor)
            );

            long long fin = horaMs();

            long long rtt = fin - inicio;

            long long horaEstimada =
                horaServidor + rtt / 2;

            long long relojCliente =
                fin + desfase;

            long long ajuste =
                horaEstimada - relojCliente;

            cout << "RTT: " << rtt << " ms\n";
            cout << "Hora servidor: "
                 << horaServidor << "\n";

            cout << "Hora estimada: "
                 << horaEstimada << "\n";

            cout << "Reloj del cliente: "
                 << relojCliente << "\n";

            cout << "Ajuste necesario: "
                 << ajuste << " ms\n";

            desfase += ajuste;

            cout << "Desfase final estimado: "
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
