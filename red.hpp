
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>

using namespace std;

void iniciarRed() {
    WSADATA datos;
    if (WSAStartup(MAKEWORD(2, 2), &datos) != 0)
        throw runtime_error("Error al iniciar Winsock");
}

void cerrarRed() {
    WSACleanup();
}

long long horaMs() {
    using namespace chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

SOCKET crearServidor(int puerto) {
    SOCKET servidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (servidor == INVALID_SOCKET)
        throw runtime_error("Error al crear socket");

    sockaddr_in direccion{};
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = INADDR_ANY;

    int opcion = 1;
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR,
               (const char*)&opcion, sizeof(opcion));

    if (bind(servidor, (sockaddr*)&direccion,
             sizeof(direccion)) == SOCKET_ERROR)
        throw runtime_error("Error en bind");

    if (listen(servidor, 10) == SOCKET_ERROR)
        throw runtime_error("Error en listen");

    return servidor;
}

SOCKET conectar(string host, int puerto) {
    SOCKET cliente = INVALID_SOCKET;

    for (int intento = 0; intento < 30; intento++) {
        cliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        sockaddr_in direccion{};
        direccion.sin_family = AF_INET;
        direccion.sin_port = htons(puerto);

        if (inet_pton(AF_INET, host.c_str(),
                      &direccion.sin_addr) != 1) {
            closesocket(cliente);
            throw runtime_error("IP invalida");
        }

        if (connect(cliente, (sockaddr*)&direccion,
                    sizeof(direccion)) == 0)
            return cliente;

        closesocket(cliente);
        Sleep(500);
    }

    throw runtime_error("No se pudo conectar al servidor");
}

void enviar(SOCKET socketCliente, string mensaje) {
    mensaje += "\n";
    size_t enviados = 0;

    while (enviados < mensaje.size()) {
        int n = send(socketCliente,
                     mensaje.data() + enviados,
                     (int)(mensaje.size() - enviados), 0);

        if (n <= 0)
            throw runtime_error("Error al enviar");

        enviados += n;
    }
}

string recibir(SOCKET socketCliente) {
    string mensaje;
    char caracter;

    while (true) {
        int n = recv(socketCliente, &caracter, 1, 0);

        if (n <= 0)
            throw runtime_error("Conexion cerrada");

        if (caracter == '\n')
            break;

        mensaje += caracter;
    }

    return mensaje;
}
