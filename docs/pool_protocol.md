# Pool protocol
A small document to better and easier understand the protocol mining pools use (it does NOT cover all commands supported by pools).  
This documentation aims to be up to date with:  
- `c5ef03dbf9fd4eebf9888ce9a1fff36ee264d118` commit on the [master branch of duino-coin repo](https://github.com/duino-coin/duino-coin/tree/master/)  
- `a4ef71e1bafe08ccf796a45a1ce8cd6bd382f05a` commit on the [useful-tools branch of duino-coin repo](https://github.com/duino-coin/duino-coin/tree/useful-tools/)  
- `74add920d01b3e28e8d526c8474c2f5b490a38e5` commit on the [master branch of duco-webservices repo](https://github.com/duino-coin/duco-webservices/tree/master/)  
- `709a83ff3e48afcde817ddffa1ab324a77daeb99` commit on the [main branch of duino-coin-pools repo](https://github.com/duino-coin/duino-coin-pools/tree/main/)

Consistency with further releases is not promised.  

## Basic info
Pools support both raw TCP connection as well as websocket to talk with miners.  
Connecting more than 50 times in timespan shorter than 30 seconds might get you IP banned.  
Commands used to communicate with pools follow the pattern of values separated by commas terminated by newline.  
Regardless of the value's data type, every field is passed as text.   
The protocol allows only one miner per connection, for `x` mining threads on one device - `x` connections to pool are required.  
To acquire pool connection info, it's possible to use `getPool` REST API query (https://server.duinocoin.com/getPool)  

## Cycle
```
Miner                          Pool
  |                             |
  |     ----* connect *--->     |
  |                             |
  |     <---[ version ]----     |
  |                             |
  |  ┌----------->-----------┐  |
  |  |                       |  |
  |  |  --[ job request ]->  |  |
  |  |                       |  |
  |  |  <-----[ job ]------  |  |
  |  ∧                       ∨  |
  |  |  ----[ result ]---->  |  |
  |  |                       |  |
  |  |  <---[ feedback ]---  |  |
  |  |                       |  |
  |  └-----------<-----------┘  |
  |                             |
```

## Miner commands
### Job request (DUCO-S1)
|     field     |                         value                         |  parse from   |   optional?   |
|---------------|-------------------------------------------------------|---------------|---------------|
|     header    |                         `JOB`                         |    char *     |      no       |
|    username   |                       username                        |    char *     |      no       |
|   difficulty  |   name from [pool difficulties](#pool-difficulties)   |    char *     |      yes      |
|   mining key  |                      mining key                       |    char *     |      yes      |

### Result
|     field     |                         value                         |  parse from   |   optional?   |
|---------------|-------------------------------------------------------|---------------|---------------|
|     nonce     |                      found nonce                      |     uint      |      no       |
|    hashrate   |                       hashrate                        |     uint      |      yes      |
| software name |                name of mining software                |    char *     |      yes      |
|    rig name   |                  name of mining rig                   |    char *     |      yes      |
|  identifier   |                    unique miner ID                    |    char *     |      no       |

## Pool commands
### Version
|     field     |                         value                         |   parse to    |   optional?   |
|---------------|-------------------------------------------------------|---------------|---------------|
|    version    |                 pool software version                 |    char *     |      no       |

### Job (DUCO-S1)
|     field     |                         value                         |   parse to    |   optional?   |
|---------------|-------------------------------------------------------|---------------|---------------|
|   last hash   |                sha1 hash of last block                |  uint32_t[5]  |      no       |
|  target hash  |               sha1 hash of target block               |  uint32_t[5]  |      no       |
|  difficulty   |               `(max_nonce - 1) / 1000`                |     uint      |      no       |

### Feedback
|     field     |                         value                         |   parse to    |   optional?   |
|---------------|-------------------------------------------------------|---------------|---------------|
|    header     |               `GOOD` / `BLOCK` / `BAD`                |    char *     |      no       |
|    reason     |                      explanation                      |    char *     |      yes      |

## Pool difficulties
> [!IMPORTANT]  
> Values from github are outdated, new ones are hidden from the public (so much for a transparent coin)  

|     name      |   value (from github)   |    value (from test)    |  reward (from github)   |  max hashrate (from github)  | kolka decrease percent  |
|---------------|-------------------------|-------------------------|-------------------------|------------------------------|-------------------------|
|    EXTREME    |         950000          |         1000000         |            0            |          999999999           |           80            |
|    XXHASH     |         100000          |         200000          |         0.0003          |            900000            |           80            |
|      NET      |         100000          |         200000          |        0.0015811        |           1000000            |           80            |
|    MEDIUM     |          75000          |         100000          |        0.0012811        |            500000            |           80            |
|      LOW      |          7500           |          20000          |        0.0022811        |            250000            |           80            |
|     ESP32     |          1500           |          8200           |         0.00375         |            16000             |           96            |
|    ESP8266    |          1000           |          4000           |         0.0045          |            13000             |           96            |
|      DUE      |           500           |           512           |          0.003          |             5000             |           96            |
|      ARM      |           100           |           128           |          0.003          |             1000             |           96            |
|     MEGA      |           16            |           24            |          0.004          |             400              |           96            |
|      AVR      |            6            |           10            |          0.005          |             240              |           96            |

## Sources
Obsolete [official Duino-Coin docs](https://github.com/duino-coin/duino-coin/blob/a4ef71e1bafe08ccf796a45a1ce8cd6bd382f05a/README.md)  
[official Duino-Coin AVR miner](https://github.com/duino-coin/duino-coin/blob/c5ef03dbf9fd4eebf9888ce9a1fff36ee264d118/AVR_Miner.py)  
[official Duino-Coin Arduino code](https://github.com/duino-coin/duino-coin/tree/c5ef03dbf9fd4eebf9888ce9a1fff36ee264d118/Arduino_Code/)  
[official Duino-Coin web miner](https://github.com/duino-coin/duco-webservices/blob/74add920d01b3e28e8d526c8474c2f5b490a38e5/webminer.html)  
[official Duino-Coin web miner worker](https://github.com/duino-coin/duco-webservices/blob/74add920d01b3e28e8d526c8474c2f5b490a38e5/js/webminer/worker.js)  
Obsolete [official Duino-Coin pool rewards config](https://github.com/duino-coin/duino-coin-pools/blob/709a83ff3e48afcde817ddffa1ab324a77daeb99/config/poolRewards.json)  
[Kind person on official DUCO's Discord](https://discord.com/channels/677615191793467402/677615906322382862/1506334231360569605)  
Me making 7 billion requests to mining pools  
