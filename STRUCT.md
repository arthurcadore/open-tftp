
## Operational Codes: 

 opcode  operation
   1     Read request (RRQ)
   2     Write request (WRQ)
   3     Data (DATA)
   4     Acknowledgment (ACK)
   5     Error (ERROR)

## RRQ / WRQ: 

 2 bytes     string    1 byte     string   1 byte
 ------------------------------------------------
| Opcode |  Filename  |   0  |   "octet"   |   0  |
 ------------------------------------------------

- Opcode: 1 ou 2 
- String1: Nome do arquivo
- Delimitador: 0x00 (1 byte)
- String2: octet
- Delimitador: 0x00 (1 byte)

## DATA: 

  2 bytes     2 bytes      512 bytes
 ----------------------------------
| Opcode |   Block #  |   Data     |
 ----------------------------------

- Opcode: 3 
- Block: número do bloco (2 bytes)
- Data: 512 Bytes

# ACK: 

  2 bytes     2 bytes
  ---------------------
 | Opcode |   Block #  |
  ---------------------

- Opcode: 4
- Block: número do bloco (2 bytes)

## ERROR: 

  2 bytes     2 bytes      string    1 byte
  -----------------------------------------
 | Opcode |  ErrorCode |   ErrMsg   |   0  |
  -----------------------------------------

- Opcode: 5
- ErrorCode: 0 á 7 (abaixo)
- ErrMsg: String Identificando o erro
- Delimitador: 0x00 (1 byte)

### Error Codes: 

   0   Not defined, see error message (if any).
   1   File not found.
   2   Access violation.
   3   Disk full or allocation exceeded.
   4   Illegal TFTP operation.
   5   Unknown transfer ID.
   6   File already exists.
   7   No such user.
