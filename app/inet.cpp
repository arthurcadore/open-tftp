#include "inet.h"

// Function to create a socket
int createSocket()
{
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    throw std::runtime_error("Failed to create socket");
  }
  return sockfd;
}

sockaddr_in stringToIPv4(const std::string &ipAddress)
{

  // Cria uma estrutura sockaddr_in para armazenar o endereço IPv4
  struct sockaddr_in ipv4Addr;

  // Zera a estrutura sockaddr_in
  memset(&ipv4Addr, 0, sizeof(ipv4Addr));

  // Define a família do endereço para IPv4
  ipv4Addr.sin_family = AF_INET;

  // Converte a string para endereço binário. Se falhar, lança uma exceção.
  if (inet_pton(AF_INET, ipAddress.c_str(), &(ipv4Addr.sin_addr)) != 1)
  {
    throw std::invalid_argument("Endereço IPv4 inválido: " + ipAddress);
  }

  return ipv4Addr;
}

int stringToPort(const std::string &p)
{
  int port;

  try
  {
    port = stoi(p);
    if (port < 1 || port > 65535)
    {
      throw std::invalid_argument("Porta inválida, utilize um número entre 1 e 65535");
    }
    return port;
  }
  catch (std::invalid_argument e)
  {
    throw std::invalid_argument("Porta inválida: " + port);
  }
}

void tftpclient::upload()
{

  std::cout << "Uploading file: " << this->filename << std::endl;

  // cria a mensagem de WRQ e envia para o servidor
  requestMessage msg(OpcodeRM::WRITE, this->filename);

  std::cout << "Sending WRQ" << std::endl;

  // envia a mensagem para o servidor
  sendto(this->sockfd, msg.serialize().data(), msg.serialize().size(), 0, (sockaddr *)&serverAddr, sizeof(serverAddr));

  // cria um callback para o upload
  uploadCallback cb(this->serverAddr, this->sockfd, this->filename, this->timeout);

  // cria um poller
  Poller poller;

  // adiciona o callback ao poller
  poller.adiciona(&cb);

  // despacha o poller
  poller.despache();
};

void tftpclient::download()
{

  std::cout << "Downloading file: " << this->filename << std::endl;

  // cria a mensagem de WRQ e envia para o servidor
  requestMessage msg(OpcodeRM::READ, this->filename);

  // envia a mensagem para o servidor
  sendto(this->sockfd, msg.serialize().data(), msg.serialize().size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

  // cria um callback para o download
  downloadCallback cb(this->serverAddr, this->sockfd, this->filename, this->timeout);

  // cria um poller
  Poller poller;

  // adiciona o callback ao poller
  poller.adiciona(&cb);

  // despacha o poller
  poller.despache();
};

/*
  Função para receber um socket e retornar uma string com o endereço IP de origem

  Parâmetros:
    - socket: descritor do socket
*/

std::string getIP(sockaddr_in socket) {
  char ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(socket.sin_addr), ip, INET_ADDRSTRLEN);
  return std::string(ip);
}