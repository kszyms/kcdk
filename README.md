# KCDK - Kszyśna CH32V003 DUCO Koparka
Duino-Coin miner written in C using ch32fun.  
Gateway not yet written.  

## Requirements
- `make`  
- `riscv64-elf` or `riscv64-unknown-elf` toolchain  
- `ch32fun` (downloaded with this repo as a submodule)  

## Building
First, clone repo and enter it:  
```bash
git clone --recursive https://github.com/kszyms/kcdk
cd kcdk
```

### Master
Not yet implemented. WIP.

### Slave
From the repo root:  
```BASH
cd slave
make build
```
`main.elf` and `main.bin` files will be in the newly created `build` directory.  

## Flashing slave
Flash the `main.elf` or `main.bin` file from the `build` directory using your preferred flasher (e.g. `wlink`, `minichlink`, WCHISPTool.exe).  
```bash
wlink flash ./build/main.elf
```

## Notes
By default KCDK uses alternate USART pins - TX on pin `D6` and RX on pin `D5` - which makes it easier to work with MCU over SWDIO after flashing it (especially for J4M6 variant).  
This and other settings can be changed in `slave/src/kcdk_config.h`.  
