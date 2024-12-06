cmake_minimum_required(VERSION 3.13)
project(tftpclient.out)

set(CMAKE_CXX_STANDARD 14)

# Define o diretório de saída para o binário
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

# Localiza a biblioteca do Protocol Buffers
find_package(Protobuf REQUIRED)

add_executable(tftpclient.out 
               app/main.cpp 
               app/main.h 
               app/messaging.h 
               app/messaging.cpp 
               app/io.h 
               app/io.cpp
               app/inet.h
               app/inet.cpp
               libs/callback.cpp
               libs/callback.h
               libs/poller.cpp
               libs/poller.h
               proto/msg.protoBuffer.pb.cc
               proto/msg.protoBuffer.pb.h) 

# Vincula a biblioteca do Protocol Buffers ao executável
target_link_libraries(tftpclient.out ${Protobuf_LIBRARIES})

add_custom_target(run
    COMMAND tftpclient.out
    DEPENDS tftpclient.out
    
  # Define o diretório de trabalho para ./out
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/out
)