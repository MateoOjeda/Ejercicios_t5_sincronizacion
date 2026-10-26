
#include "red.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Uso: lamport.exe <numero de proceso 1-4>\n";
        return 1;
    }

    int proceso = stoi(argv[1]);

    if (proceso < 1 || proceso > 4) {
        cout << "Proceso invalido\n";
        return 1;
    }

    iniciarRed();

    try {
        int reloj = 0;
        int puerto = 5000 + proceso;

        if (proceso > 1) {
            SOCKET servidor = crearServidor(puerto);

            cout << "P" << proceso
                 << " esperando mensaje en puerto "
                 << puerto << "...\n";

            SOCKET anterior = accept(
                servidor, nullptr, nullptr
            );

            if (anterior == INVALID_SOCKET)
                throw runtime_error("Error en accept");

            int recibido = stoi(recibir(anterior));

            reloj = max(reloj, recibido) + 1;

            cout << "Mensaje recibido: " << recibido << "\n";
            cout << "Reloj actualizado: " << reloj << "\n";

            closesocket(anterior);
            closesocket(servidor);
        }

        reloj++;

        cout << "\nEjecutando P" << proceso
             << " | Reloj logico: " << reloj << "\n";

        Sleep(1000);

        if (proceso < 4) {
            reloj++;

            SOCKET siguiente = conectar(
                "127.0.0.1", 5000 + proceso + 1
            );

            enviar(siguiente, to_string(reloj));

            cout << "Mensaje enviado a P"
                 << proceso + 1
                 << " | Marca: " << reloj << "\n";

            closesocket(siguiente);
        }

        cout << "P" << proceso << " finalizado.\n";

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        cerrarRed();
        return 1;
    }

    cerrarRed();
    return 0;
}
