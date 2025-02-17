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

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::cout << "Uploading file: " << this->filename << std::endl;

  // cria a mensagem de WRQ e envia para o servidor
  tftp2::Mensagem wrq;

  wrq.mutable_wrq();
  wrq.mutable_wrq()->set_fname(this->filename);
  wrq.mutable_wrq()->set_mode(tftp2::Mode::octet);

  std::cout << "Sending WRQ" << std::endl;

  // cria uma mensagem de WRQ
  std::string wrqString; 
  wrq.SerializeToString(&wrqString);

  // envia a mensagem para o servidor
  sendto(this->sockfd, wrqString.data(), wrqString.size(), 0, (sockaddr *)&serverAddr, sizeof(serverAddr));

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
  
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::cout << "Downloading file: " << this->filename << std::endl;

  // cria a mensagem de WRQ e envia para o servidor
  tftp2::Mensagem rrq;

  rrq.mutable_rrq(); 
  rrq.mutable_rrq()->set_fname(this->filename);
  rrq.mutable_rrq()->set_mode(tftp2::Mode::octet);

  std::cout << "Sending RRQ" << std::endl;

  // cria uma mensagem de RRQ
  std::string rrqString;
  rrq.SerializeToString(&rrqString);

  // envia a mensagem para o servidor
  sendto(this->sockfd, rrqString.data(), rrqString.size(), 0, (sockaddr *)&serverAddr, sizeof(serverAddr));

  // cria um callback para o download
  downloadCallback cb(this->serverAddr, this->sockfd, this->filename, this->timeout);

  // cria um poller
  Poller poller;

  // adiciona o callback ao poller
  poller.adiciona(&cb);

  // despacha o poller
  poller.despache();
};

void tftpclient::move(const std::string& newname) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Renomeando arquivo: " << this->filename << " -> " << newname << std::endl;

    // Cria a mensagem de MOVE e envia para o servidor
    tftp2::Mensagem moveMsg;

    // Configura a mensagem MOVE
    auto* move = moveMsg.mutable_move();
    move->set_nome_orig(filename);
    move->set_nome_novo(newname);

    // Serializa a mensagem MOVE
    std::string moveString;
    moveMsg.SerializeToString(&moveString);

    // Envia a mensagem para o servidor
    sendto(this->sockfd, moveString.data(), moveString.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Cria um callback para o comando MOVE
    moveCallback cb(this->serverAddr, this->sockfd, filename, newname, this->timeout);

    // Cria um poller
    Poller poller;

    // Adiciona o callback ao poller
    poller.adiciona(&cb);

    // Despacha o poller
    poller.despache();
}

void tftpclient::mkdir(){
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Criando diretório: " << this->filename << std::endl;

    // Cria a mensagem de MKDIR e envia para o servidor
    tftp2::Mensagem mkdirMsg;

    // Configura a mensagem MKDIR
    auto* mkdir = mkdirMsg.mutable_mkdir();
    mkdir->set_path(filename);

    // Serializa a mensagem MKDIR
    std::string mkdirString;
    mkdirMsg.SerializeToString(&mkdirString);

    // Envia a mensagem para o servidor
    sendto(this->sockfd, mkdirString.data(), mkdirString.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Cria um callback para o comando MKDIR
    mkdirCallback cb(this->serverAddr, this->sockfd, filename, this->timeout);

    // Cria um poller
    Poller poller;

    // Adiciona o callback ao poller
    poller.adiciona(&cb);

    // Despacha o poller
    poller.despache();
}




// void tftpclient::remove(){
//     GOOGLE_PROTOBUF_VERIFY_VERSION;

//     std::cout << "Removendo arquivo: " << this->filename << std::endl;

//     // Cria a mensagem de REMOVE e envia para o servidor
//     tftp2::Mensagem removeMsg;

//     // Configura a mensagem REMOVE
//     auto* remove = removeMsg.mutable
//     remove->set_nome(filename);

//     // Serializa a mensagem REMOVE
//     std::string removeString;
//     removeMsg.SerializeToString(&removeString);

//     // Envia a mensagem para o servidor
//     sendto(this->sockfd, removeString.data(), removeString.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

//     // Cria um callback para o comando REMOVE
//     removeCallback cb(this->serverAddr, this->sockfd, filename, this->timeout);

//     // Cria um poller
//     Poller poller;

//     // Adiciona o callback ao poller
//     poller.adiciona(&cb);

//     // Despacha o poller
//     poller.despache();
// }

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


void tftpclient::list(){

}

void tftpclient::remove(){
  
}