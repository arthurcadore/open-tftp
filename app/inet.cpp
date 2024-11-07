#include "inet.h"

/*
  Converte uma string contendo um endereço IPv4 para uma estrutura sockaddr_in
  Parâmetros:
    - ipAddress: string contendo o endereço IPv4
  Retorno:
    - estrutura sockaddr_in preenchida com o endereço IPv4
  Exceção:
    - std::invalid_argument caso o endereço IPv4 seja inválido
*/
std::sockaddr_in stringToIPv4(const std::string& ipAddress) {

    // Cria uma estrutura sockaddr_in para armazenar o endereço IPv4
    struct sockaddr_in ipv4Addr;

    // Zera a estrutura sockaddr_in
    memset(&ipv4Addr, 0, sizeof(ipv4Addr));

    // Define a família do endereço para IPv4
    ipv4Addr.sin_family = AF_INET;

    // Converte a string para endereço binário. Se falhar, lança uma exceção.
    if (inet_pton(AF_INET, ipAddress.c_str(), &(ipv4Addr.sin_addr)) != 1) {
        throw std::invalid_argument("Endereço IPv4 inválido: " + ipAddress);
    }

    return ipv4Addr;
}

std::int stringToPort(const std::string& p) {
  std::int port;

  try {
    port = stoi(p);
    if (port < 1 || port > 65535) {
        throw std::invalid_argument("Porta inválida, utilize um número entre 1 e 65535");
    }
    return port;

  } catch (invalid_argument e) {
        throw std::invalid_argument("Porta inválida: " + port);
  }
}