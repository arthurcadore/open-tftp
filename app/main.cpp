#include "main.h"

/*
  Cria um socket UDP e retorna o descritor deste socket
*/
int createSocket() {
    // Tentar criar o socket UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd < 0) {
        throw std::runtime_error("Erro ao criar o socket UDP");
    }

    cout << "Socket criado com sucesso!" << endl;
    
    return sockfd; // Retorna o descritor de socket se for bem-sucedido
}


/*
    Recebe uma mensagem com cabeçalho e retorna apenas a mensagem de erro em string
*/
string errorCheck(char* msg) {
    // remove os primeiros caracteres da mensagem que são o cabeçalho deixando apenas a mensagem
    string errMsg = msg + 4;

    // remove o delimitador 0x00 ao final da mensagem
    errMsg.pop_back();

    return errMsg;
}


/*
  Converte uma string contendo um endereço IPv4 para uma estrutura sockaddr_in
  Parâmetros:
    - ipAddress: string contendo o endereço IPv4
  Retorno:
    - estrutura sockaddr_in preenchida com o endereço IPv4
  Exceção:
    - std::invalid_argument caso o endereço IPv4 seja inválido
*/
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

/*
  Verifica se a mensagem recebida é válida, comparando o opcode e o número do bloco
  Parâmetros:
    - buffer: buffer contendo a mensagem recebida
    - opcode: opcode esperado
    - blockNumber: número do bloco esperado
  Retorno:
    - true se a mensagem é válida, false caso contrário
*/
bool checkData(char* buffer, int opcode, int blockNumber) {

  blockNumber = blockNumber + 1;

  cout << "Opcode Esperado: " << "0" << opcode << endl;
  cout << "Opcode Recebido: " << (int) buffer[0] << (int) buffer[1] << endl;

    if (buffer[0] != 0 || buffer[1] != opcode) {

        // coleta o código de erro e a mensagem de erro
        string error = errorCheck(buffer);
        throw std::runtime_error("Opcode inválido, Erro: " + error);
        return false;
    }

  cout << "Block Number Esperado: " << blockNumber << endl;
  cout << "Block Number Recebido: " << (int) buffer[2] << (int) buffer[3] << endl;

    if (buffer[2] != (blockNumber >> 8) || buffer[3] != (blockNumber & 0xFF)) {
        throw std::runtime_error("Erro: número de bloco inválido");
        return false;
    }
    return true;
}

/*
  Verifica se a mensagem recebida de ACK é válida, comparando o opcode e o número do bloco
  Parâmetros:
    - buffer: buffer contendo a mensagem recebida
    - opcode: opcode esperado
    - blockNumber: número do bloco esperado
  Retorno:
    - true se a mensagem é válida, false caso contrário
*/
bool checkACK(char* buffer, int opcode, int blockNumber){

  cout << "Opcode Esperado: " << "0" << opcode << endl;
  cout << "Opcode Recebido: " << (int) buffer[0] << (int) buffer[1] << endl;

    if (buffer[0] != 0 || buffer[1] != opcode) {
        // coleta o código de erro e a mensagem de erro
        string error = errorCheck(buffer);
        throw std::runtime_error("Opcode inválido, Erro: " + error);
        return false;
    }

  cout << "Block Number Esperado: " << blockNumber << endl;
  cout << "Block Number Recebido: " << (int) buffer[2] << (int) buffer[3] << endl;

    if (buffer[2] != (blockNumber >> 8) || buffer[3] != (blockNumber & 0xFF)) {
        throw std::runtime_error("Erro: número de bloco inválido");
        return false;
    }
    return true;
}


/*
  Converte uma estrutura sockaddr_in para uma string contendo um endereço IPv4
  Parâmetros:
    - ipv4Addr: estrutura sockaddr_in contendo o endereço IPv4
*/
void download(sockaddr_in ip, int porta, string arquivo) {

        // Criar o socket
        int sockfd = createSocket();
        ip.sin_port = htons(porta);

        string msg = requestMessage(RRQ, arquivo, "octet"); 

        // Enviar a mensagem para o servidor
        ssize_t sentBytes = sendto(sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr*)&ip, sizeof(ip));

        if (sentBytes < 0) {
            throw std::runtime_error("Erro ao enviar a mensagem");
        } 

        // Estado do download
        bool state = true; 
        int blockNumber = 0;

        // Enviar a mensagem de ack para o server
        string ack = ackMessage(blockNumber);
        sentBytes = sendto(sockfd, ack.c_str(), ack.size(), 0, (struct sockaddr*)&ip, sizeof(ip));
        
        do {
          // Aguarda a resposta do servidor de um bloco de dados com 512 bytes e imprime no terminal
          char buffer[516];
          socklen_t len = sizeof(ip);
          ssize_t recvBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&ip, &len);

          cout << "Recebendo " << recvBytes << " bytes" << endl;

          if (recvBytes < 0) {
              throw std::runtime_error("Erro ao receber a mensagem");
          }

          if(checkData(buffer, DATA, blockNumber)){
            
            // 2 bytes opcode + 2 bytes block number 
            // em seguida escreve em um arquivo, removendo os 4 primeiros bytes
            writefile(buffer + 4, recvBytes - 4, arquivo);

            // Verifica se o bloco de dados recebido é menor que 516 bytes
            // 512 bytes de dados + 2 bytes de opcode + 2 bytes de block number
            if (recvBytes < 516) {
                state = false;
            }

            // Incrementa o número do bloco
            blockNumber++;

            // Enviar a mensagem de ack para o server
            string ack = ackMessage(blockNumber);
            sentBytes = sendto(sockfd, ack.c_str(), ack.size(), 0, (struct sockaddr*)&ip, sizeof(ip));

          }

        } while (state);


        // Fechar o socket
        close(sockfd);
}

/*
  Envia um arquivo para um servidor FTP
  Parâmetros:
    - ip: estrutura sockaddr_in contendo o endereço do servidor FTP
    - porta: porta do servidor FTP
    - arquivo: nome do arquivo a ser enviado
*/
void upload(sockaddr_in ip, int porta, string arquivo) {

        // Criar o socket
        int sockfd = createSocket();
        ip.sin_port = htons(porta);

        string msg = requestMessage(WRQ, arquivo, "octet"); 

        // Enviar a mensagem para o servidor
        ssize_t sentBytes = sendto(sockfd, msg.c_str(), msg.size(), 0, 
                                   (struct sockaddr*)&ip, sizeof(ip));

        if (sentBytes < 0) {
            throw std::runtime_error("Erro ao enviar a mensagem");
        } 

        bool state = true; // Estado do download
        int blockNumber = 0;

        do {  
          // aguarda um ACK do servidor para o bloco de dados enviado. 
          char buffer[516];
          socklen_t len = sizeof(ip);
          ssize_t recvBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&ip, &len);

          cout << "Recebendo " << recvBytes << " bytes" << endl;

          if (recvBytes < 0) {
              throw std::runtime_error("Erro ao receber a mensagem");
          }

          // verifica se o ACK recebido é válido
          if(checkACK(buffer, ACK, blockNumber)){
            
            // lê 512 bytes do arquivo e armazena no buffer
            readfile(arquivo, buffer, 512, blockNumber * 512);

            // Enviar a mensagem para o servidor
            string data = datablock(blockNumber, buffer);

            sentBytes = sendto(sockfd, data.c_str(), data.size(), 0, (struct sockaddr*)&ip, sizeof(ip));

            if (sentBytes < 0) {
                throw std::runtime_error("Erro ao enviar a mensagem");
            }    

            // incrementa o número do bloco

            blockNumber++;
          }
        } while (state);

        // Fechar o socket
        close(sockfd);
}

/*
  Função principal
  Parâmetros:
    - argc: número de argumentos na linha de comando
    - argv: array de argumentos da linha de comando
*/
int main(int argc, char* argv[]) {

  // verifica se há 3 argumentos na linha de comando (nome do programa, IP e porta)
  if (argc != 3) {
    cout << "Uso correto: " << argv[0] << " <IP> <Porta>" << endl;
    return 1;
  }

  // tenta converter o IP string para um IPv4, caso não consiga exibe o erro "Endereço IPv4 inválido"
  struct sockaddr_in ipv4Addr;
  try {
    ipv4Addr = stringToIPv4(argv[1]);
  } catch (invalid_argument e) {
    cout << "Endereço IPv4 inválido" << endl;
    return 1;
  }

  // tenta converter a porta para número usando stoi, e exibe o erro "Porta inválida" caso não seja possível
  // Também verifica se a porta é um número entre 1 e 65535.
  int port;
  try {
    port = stoi(argv[2]);
    if (port < 1 || port > 65535) {
      cout << "Porta inválida, utilize um número entre 1 e 65535" << endl;
      return 1;
    }
  } catch (invalid_argument e) {
    cout << "Argumento de porta inválido, utilize um número" << endl;
    return 1;
  }

  /*
    Laço do prompt de comando
  */
  while (true) {
    cout << "ftp > ";
    string comando;
    getline(cin, comando);

    // separa a string comando em duas partes: comando e argumento
    string argumento;
    size_t pos = comando.find(" ");
    if (pos != string::npos) {
      argumento = comando.substr(pos + 1);
      comando = comando.substr(0, pos);
    }
    
    // ignora comandos vazios
    if (comando.empty()) {
      continue;
    }

    // verifica se o comando é "exit" e sai do laço
    if (comando == "exit") {
      break;

    // verifica se o comando é "get" e chama a função download
    } else if (comando == "get") {

      // ignora argumentos vazios
      if (argumento.empty()) {
        cout << "Argumento inválido" << endl;
        continue;
      }

      cout << "Fazendo download do arquivo..." << endl;
      download(ipv4Addr, port, argumento);

    // verifica se o comando é "put" e chama a função upload
    } else if (comando == "put") {

      // ignora argumentos vazios
      if (argumento.empty()) {
        cout << "Argumento inválido" << endl;
        continue;
      }

      cout << "Fazendo upload do arquivo..." << endl;
      upload(ipv4Addr, port, argumento);

    // comando inválido
    } else {
      cout << "Comando inválido" << endl;
    }
  }
}