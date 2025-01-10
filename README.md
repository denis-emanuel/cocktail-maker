# Cocktail Maker DIY

- [Cocktail Maker DIY](#cocktail-maker-diy)
  - [Documentation](#documentation)
    - [Table of contents generation](#table-of-contents-generation)
  - [Config and setup](#config-and-setup)
    - [PlatformIO](#platformio)
      - [Upload code](#upload-code)
    - [Constants and Function Prototypes](#constants-and-function-prototypes)

## Documentation

### Table of contents generation

Install `Markdown All in One` extension.
Then use `CMD + SHIFT + P` or `CTRL + SHIFT + P` and search for `Create table of contents`

## Config and setup

Everything is set up using VSCode.

### PlatformIO

Install the PlatformIO extension.

In the `platform.ini` file you can change the `PORT` on which the Arduino is plugged in via the `monitor_port` variable.

Any other PlatformIO configs are found here.

#### Upload code

Use `CMD + SHIFT + P` or `CTRL + SHIFT + P` and then type in `PlatformIO` and in order:
1. `build`
1. `upload`

### Constants and Function Prototypes

They will be configured in `./include/constants.h` and `./include/functions.h` respectively