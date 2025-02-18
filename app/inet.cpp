#include "inet.h"

/**
 * @brief Cria um socket UDP.
 * 
 * Tenta criar um socket utilizando o domínio AF_INET (IPv4) e o tipo SOCK_DGRAM (UDP).
 * Se a criação falhar, lança uma exceção std::runtime_error.
 *
 * @return int Descritor do socket criado.
 */
int createSocket()
{
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    throw std::runtime_error("Failed to create socket");
  }
  return sockfd;
}

/**
 * @brief Converte uma string contendo um endereço IPv4 para uma estrutura sockaddr_in.
 * 
 * Preenche a estrutura sockaddr_in com os dados do endereço IPv4 fornecido.
 * Caso a conversão da string para o endereço binário falhe, lança uma exceção std::invalid_argument.
 *
 * @param ipAddress String contendo o endereço IPv4 (ex.: "192.168.0.1").
 * @return sockaddr_in Estrutura preenchida com o endereço IPv4.
 */
sockaddr_in stringToIPv4(const std::string &ipAddress)
{
  // Cria uma estrutura sockaddr_in para armazenar o endereço IPv4
  struct sockaddr_in ipv4Addr;

  // Zera a estrutura para garantir que não haja lixo de memória
  memset(&ipv4Addr, 0, sizeof(ipv4Addr));

  // Define a família de endereços para IPv4
  ipv4Addr.sin_family = AF_INET;

  // Converte a string para um endereço binário. Se falhar, lança uma exceção.
  if (inet_pton(AF_INET, ipAddress.c_str(), &(ipv4Addr.sin_addr)) != 1)
  {
    throw std::invalid_argument("Endereço IPv4 inválido: " + ipAddress);
  }

  return ipv4Addr;
}

/**
 * @brief Converte uma string representando uma porta para um número inteiro.
 * 
 * Tenta converter a string para inteiro usando stoi.
 * Verifica se o número está no intervalo válido (1 a 65535). Caso contrário, lança uma exceção.
 *
 * @param p String contendo o número da porta.
 * @return int Número da porta convertido.
 */
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
    // Caso a conversão falhe, lança uma exceção informando que a porta é inválida.
    throw std::invalid_argument("Porta inválida: " + p);
  }
}

/**
 * @brief Envia uma requisição de upload de arquivo para o servidor TFTP.
 * 
 * O método realiza as seguintes etapas:
 * - Verifica a versão do Google Protocol Buffers.
 * - Exibe no console que o upload está sendo iniciado.
 * - Cria e configura uma mensagem WRQ (Write Request) com o nome do arquivo e modo de transferência.
 * - Serializa a mensagem e envia ao servidor via sendto.
 * - Cria um callback de upload e adiciona-o a um poller para gerenciar o envio dos blocos.
 */
void tftpclient::upload()
{
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::cout << "Uploading file: " << this->filename << std::endl;

  // Cria e configura a mensagem de Write Request (WRQ)
  tftp2::Mensagem wrq;
  wrq.mutable_wrq();
  wrq.mutable_wrq()->set_fname(this->filename);
  wrq.mutable_wrq()->set_mode(tftp2::Mode::octet);

  std::cout << "Sending WRQ" << std::endl;

  // Serializa a mensagem WRQ para uma string
  std::string wrqString; 
  wrq.SerializeToString(&wrqString);

  // Envia a mensagem para o servidor
  sendto(this->sockfd, wrqString.data(), wrqString.size(), 0, (sockaddr *)&serverAddr, sizeof(serverAddr));

  // Cria um callback para gerenciar o upload
  uploadCallback cb(this->serverAddr, this->sockfd, this->filename, this->timeout);

  // Cria um poller para gerenciar os eventos de rede
  Poller poller;

  // Adiciona o callback ao poller
  poller.adiciona(&cb);

  // Inicia o loop do poller para processar os eventos
  poller.despache();
};

/**
 * @brief Envia uma requisição de download de arquivo para o servidor TFTP.
 * 
 * O método realiza as seguintes etapas:
 * - Verifica a versão do Google Protocol Buffers.
 * - Exibe no console que o download está sendo iniciado.
 * - Cria e configura uma mensagem RRQ (Read Request) com o nome do arquivo e modo de transferência.
 * - Serializa a mensagem e envia ao servidor via sendto.
 * - Cria um callback de download e adiciona-o a um poller para gerenciar a recepção dos blocos.
 */
void tftpclient::download()
{
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::cout << "Downloading file: " << this->filename << std::endl;

  // Cria e configura a mensagem de Read Request (RRQ)
  tftp2::Mensagem rrq;
  rrq.mutable_rrq(); 
  rrq.mutable_rrq()->set_fname(this->filename);
  rrq.mutable_rrq()->set_mode(tftp2::Mode::octet);

  std::cout << "Sending RRQ" << std::endl;

  // Serializa a mensagem RRQ para uma string
  std::string rrqString;
  rrq.SerializeToString(&rrqString);

  // Envia a mensagem para o servidor
  sendto(this->sockfd, rrqString.data(), rrqString.size(), 0, (sockaddr *)&serverAddr, sizeof(serverAddr));

  // Cria um callback para gerenciar o download
  downloadCallback cb(this->serverAddr, this->sockfd, this->filename, this->timeout);

  // Cria um poller para gerenciar os eventos de rede
  Poller poller;

  // Adiciona o callback ao poller
  poller.adiciona(&cb);

  // Inicia o loop do poller para processar os eventos
  poller.despache();
};

/**
 * @brief Envia uma requisição para renomear (mover) um arquivo no servidor TFTP.
 * 
 * O método realiza as seguintes etapas:
 * - Verifica a versão do Google Protocol Buffers.
 * - Exibe no console a operação de renomeação.
 * - Cria e configura uma mensagem MOVE com o nome original e o novo nome.
 * - Serializa a mensagem e envia ao servidor via sendto.
 * - Cria um callback de move e adiciona-o a um poller para gerenciar a operação.
 *
 * @param newname Novo nome para o arquivo.
 */
void tftpclient::move(const std::string& newname) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Renomeando arquivo: " << this->filename << " -> " << newname << std::endl;

    // Cria e configura a mensagem de MOVE
    tftp2::Mensagem moveMsg;
    auto* move = moveMsg.mutable_move();
    move->set_nome_orig(filename);
    move->set_nome_novo(newname);

    // Serializa a mensagem MOVE para uma string
    std::string moveString;
    moveMsg.SerializeToString(&moveString);

    // Envia a mensagem para o servidor
    sendto(this->sockfd, moveString.data(), moveString.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Cria um callback para gerenciar a operação de MOVE
    moveCallback cb(this->serverAddr, this->sockfd, filename, newname, this->timeout);

    // Cria um poller para gerenciar os eventos de rede
    Poller poller;

    // Adiciona o callback ao poller
    poller.adiciona(&cb);

    // Inicia o loop do poller para processar os eventos
    poller.despache();
}

/**
 * @brief Envia uma requisição para remover um arquivo do servidor TFTP.
 * 
 * O método realiza as seguintes etapas:
 * - Verifica a versão do Google Protocol Buffers.
 * - Exibe no console a operação de remoção.
 * - Cria e configura uma mensagem MOVE (utilizada para remoção) com o nome do arquivo.
 * - Serializa a mensagem e envia ao servidor via sendto.
 * - Cria um callback de remoção e adiciona-o a um poller para gerenciar a operação.
 */
void tftpclient::remove() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Removendo arquivo: " << this->filename << std::endl;

    // Cria e configura a mensagem MOVE para remoção
    tftp2::Mensagem moveMsg;
    auto* move = moveMsg.mutable_move();
    move->set_nome_orig(filename);

    // Serializa a mensagem MOVE para uma string
    std::string moveString;
    moveMsg.SerializeToString(&moveString);

    // Envia a mensagem para o servidor
    sendto(this->sockfd, moveString.data(), moveString.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Cria um callback para gerenciar a remoção
    removeCallback cb(this->serverAddr, this->sockfd, filename, this->timeout);

    // Cria um poller para gerenciar os eventos de rede
    Poller poller;

    // Adiciona o callback ao poller
    poller.adiciona(&cb);

    // Inicia o loop do poller para processar os eventos
    poller.despache();
}

/**
 * @brief Envia uma requisição para criar um diretório no servidor TFTP.
 * 
 * O método realiza as seguintes etapas:
 * - Verifica a versão do Google Protocol Buffers.
 * - Exibe no console a operação de criação de diretório.
 * - Cria e configura uma mensagem MKDIR com o caminho do diretório.
 * - Serializa a mensagem e envia ao servidor via sendto.
 * - Cria um callback de MKDIR e adiciona-o a um poller para gerenciar a operação.
 */
void tftpclient::mkdir(){
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Criando diretório: " << this->filename << std::endl;

    // Cria e configura a mensagem MKDIR
    tftp2::Mensagem mkdirMsg;
    auto* mkdir = mkdirMsg.mutable_mkdir();
    mkdir->set_path(filename);

    // Serializa a mensagem MKDIR para uma string
    std::string mkdirString;
    mkdirMsg.SerializeToString(&mkdirString);

    // Envia a mensagem para o servidor
    sendto(this->sockfd, mkdirString.data(), mkdirString.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Cria um callback para gerenciar a operação de MKDIR
    mkdirCallback cb(this->serverAddr, this->sockfd, filename, this->timeout);

    // Cria um poller para gerenciar os eventos de rede
    Poller poller;

    // Adiciona o callback ao poller
    poller.adiciona(&cb);

    // Inicia o loop do poller para processar os eventos
    poller.despache();
}

/**
 * @brief Envia uma requisição para listar o conteúdo de um diretório no servidor TFTP.
 * 
 * O método realiza as seguintes etapas:
 * - Verifica a versão do Google Protocol Buffers.
 * - Exibe no console a operação de listagem do diretório.
 * - Cria e configura uma mensagem LIST com o caminho do diretório.
 * - Serializa a mensagem e envia ao servidor via sendto.
 * - Cria um callback de listagem e adiciona-o a um poller para gerenciar a operação.
 */
void tftpclient::list(){
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Listando diretório: " << this->filename << std::endl;

    // Cria e configura a mensagem LIST
    tftp2::Mensagem listMsg;
    auto* list = listMsg.mutable_list();
    list->set_path(filename);

    // Serializa a mensagem LIST para uma string
    std::string listString;
    listMsg.SerializeToString(&listString);

    // Envia a mensagem para o servidor
    sendto(this->sockfd, listString.data(), listString.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Cria um callback para gerenciar a listagem
    listCallback cb(this->serverAddr, this->sockfd, filename, this->timeout);

    // Cria um poller para gerenciar os eventos de rede
    Poller poller;

    // Adiciona o callback ao poller
    poller.adiciona(&cb);

    // Inicia o loop do poller para processar os eventos
    poller.despache();
}

/**
 * @brief Retorna o endereço IP em formato string a partir de uma estrutura sockaddr_in.
 * 
 * Utiliza inet_ntop para converter o endereço binário para uma string no formato IPv4.
 *
 * @param socket Estrutura sockaddr_in contendo o endereço.
 * @return std::string Endereço IP no formato string.
 */
std::string getIP(sockaddr_in socket) {
  char ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(socket.sin_addr), ip, INET_ADDRSTRLEN);
  return std::string(ip);
}
