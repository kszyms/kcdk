# Hashing method
A small document to better and easier understand absolutely abysmal process of how Duino-Coin utilizes SHA-1 for mining. Also has some benchmarks of my own  

## Overview
1. Pool sends job with three comma separated values  
  - last hash (hex string)  
  - expected hash (hex string)  
  - difficulty (dec string)  
2. Convert target hash into 20 raw bytes  
3. Read ASCII values of hexadecimal representation of last hash  
4. Read ASCII values of decimal representation of nonce  
5. Concatenate result of step 3. with result of step 4.  
6. SHA-1 hash concatenated string  
7. Compare output with converted target hash  
8. If they match, submit nonce; if they don't match, increase nonce and repeat from step 4  

## Example
1. Pool sends job: `"b26692f088ffe0c453e8325082c4de5e4ba6837a,abc7f1bbf70f98f4fc996492d641c267449cd9b5,4000"`  
2. Store target hash as bytes: `"abc7f1bbf70f98f4fc996492d641c267449cd9b5"` into `abc7f1bbf70f98f4fc996492d641c267449cd9b5`  
3. Parse last hash as ASCII: `"b26692f088ffe0c453e8325082c4de5e4ba6837a"` into `62323636393266303838666665306334353365383332353038326334646535653462613638333761`  
4. Parse nonce as ASCII: `111244` into `313131323434`  
5. Concat hash with nonce: `62323636393266303838666665306334353365383332353038326334646535653462613638333761313131323434`  
6. SHA-1 hash that data stream: `62323636393266303838666665306334353365383332353038326334646535653462613638333761313131323434` into `abc7f1bbf70f98f4fc996492d641c267449cd9b5`  
7. Byte values of the hash should match hash that pool sent as string  

## Hashing speed tests
### Raw hashing; no compiler optimization
|               unit                |     value     |
|-----------------------------------|---------------|
|    (ticks / 100000 hashes) / 8    |    87862501   |
|       ticks / 100000 hashes       |   702900008   |
|            ticks / hash           |      7029     |
|           ticks / second          |    48000000   |
|          hashes / second          |      6828     |

### Raw hashing; -O1 and -funroll-loops
|               unit                |     value     |
|-----------------------------------|---------------|
|    (ticks / 100000 hashes) / 8    |    66271875   |
|       ticks / 100000 hashes       |   530175000   |
|            ticks / hash           |      5301     |
|           ticks / second          |    48000000   |
|          hashes / second          |      9054     |
