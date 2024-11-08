#include "inet.h"

sockaddr_in stringToIPv4(const std::string& ipAddress) {

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

int stringToPort(const std::string& p) {
  int port;

  try {
    port = stoi(p);
    if (port < 1 || port > 65535) {
        throw std::invalid_argument("Porta inválida, utilize um número entre 1 e 65535");
    }
    return port;

  } catch (std::invalid_argument e) {
        throw std::invalid_argument("Porta inválida: " + port);
  }
}

void tftpclient::download(const std::string& filename){

    std::cout << "Verificando acesso ao arquivo" << std::endl;
    if (!fileCheck(filename)) {
        return;
    }
    std::cout << "Servidor: " << ip << " na porta " << port << std::endl;

    std::string buffer = readBlock(filename, 0, 512);

    //printf("Conteúdo do arquivo: %s\n", buffer.c_str());

    dataMessage data(OpcodeDM::DATA, 1, buffer);
    std::vector<uint8_t> serializedData = data.serialize();

    // imprimir o tamanho do vetor
    std::cout << "Tamanho do vetor: " << serializedData.size() << std::endl;
}

void tftpclient::upload(const std::string& filename){

    std::cout << "Verificando acesso ao arquivo" << std::endl;
    if (!fileCheck(filename)) {
        return;
    }

    std::cout << "Servidor: " << ip << " na porta " << port << std::endl;

}