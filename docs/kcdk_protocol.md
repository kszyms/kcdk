# KCDK protocol
A small document about protocol used for communication between gateway and miner.  

## Cycle
```
Slave                          Host
  |                             |
  |     ------[ IDN ]----->     |
  |                             |
  |     <-----[ ACK ]------     |
  |                             |
  |  ┌----------->-----------┐  |
  |  |                       |  |
  |  |  ------[ JRQ ]----->  |  |
  |  |                       |  |
  |  |  <-----[ ACK ]------  |  |
  |  |                       |  |
  |  |  <-----[ JOB ]------  |  |
  |  ∧                       ∨  |
  |  |  ------[ ACK ]----->  |  |
  |  |                       |  |
  |  |  -----[ JRES ]----->  |  |
  |  |                       |  |
  |  |  <-----[ ACK ]------  |  |
  |  |                       |  |
  |  └-----------<-----------┘  |
  |                             |
```

## Miner commands
### Identify
|     field     |                         value                         |    parse from    |
|---------------|-------------------------------------------------------|------------------|
|     sync      |                         `Kk`                          |     char[2]      |
|    header     |                        `0x49`                         |     uint8_t      |
|    source     |                       miner id                        |    uint8_t[8]    |
|   checksum    |         [crc-8 checksum of packet](#checksum)         |     uint8_t      |

### Job request
|     field     |                         value                         |    parse from    |
|---------------|-------------------------------------------------------|------------------|
|     sync      |                         `Kk`                          |     char[2]      |
|    header     |                        `0x52`                         |     uint8_t      |
|    source     |                       miner id                        |    uint8_t[8]    |
|   checksum    |         [crc-8 checksum of packet](#checksum)         |     uint8_t      |

### Job result
|     field     |                         value                         |    parse from    |
|---------------|-------------------------------------------------------|------------------|
|     sync      |                         `Kk`                          |     char[2]      |
|    header     |                        `0x56`                         |     uint8_t      |
|    source     |                       miner id                        |    uint8_t[8]    |
|   checksum    |         [crc-8 checksum of packet](#checksum)         |     uint8_t      |
|     nonce     |                      found nonce                      |  uint32_t_(le)   |

### Acknowledgment
|     field     |                         value                         |    parse from    |
|---------------|-------------------------------------------------------|------------------|
|     sync      |                         `Kk`                          |     char[2]      |
|    header     |                        `0x41`                         |     uint8_t      |
|    source     |                       miner id                        |    uint8_t[8]    |
|   checksum    |         [crc-8 checksum of packet](#checksum)         |     uint8_t      |

## Gateway commands
### Job
|     field     |                         value                         |     parse to     |
|---------------|-------------------------------------------------------|------------------|
|     sync      |                         `kK`                          |     char[2]      |
|    header     |                        `0x4A`                         |     uint8_t      |
|  destination  |                       miner id                        |    uint8_t[8]    |
|   checksum    |         [crc-8 checksum of packet](#checksum)         |     uint8_t      |
|   old hash    |         old hash already parsed as input data         |   uint8_t[40]    |
| expected hash |                     expected hash                     | uint32_t[5]_(le) |
|  difficulty   |               `(max_nonce - 1) / 1000`                |  uint32_t_(le)   |

### Acknowledgment
|     field     |                         value                         |     parse to     |
|---------------|-------------------------------------------------------|------------------|
|     sync      |                         `kK`                          |     char[2]      |
|    header     |                        `0x41`                         |     uint8_t      |
|  destination  |                       miner id                        |    uint8_t[8]    |
|   checksum    |         [crc-8 checksum of packet](#checksum)         |     uint8_t      |

## Checksum
KCDK uses packet checksum to detect and reject corrupted/collided packets.  
Utilized checksum method is CRC-8/DVB-S2.  
Checksum is calculated from whole packet, that is - from sync bytes, till the last byte of the packet.  
When calculating checksum, the checksum field should be set to `0x69`.  
