# 20_Demo-for-RISC-V-Day-on-2020

## Branches information
master --> basic Amazon FreeRTOS demo `aws_demo` for Andes Corvette-F1 board with hardware security (ATECC608A device)
Update_OTA --> Fully tested released source code

## Setup environment
### Clone repository
```
$ git clone https://github.com/SH-CONSULTING/20_Demo-for-RISC-V-Day-on-2020.git
```

### Prepare Development Environment (on Windows)

**Software tools:**
* Set Up Cygwin (for Windows only)
* Set Up the Toolchain
* Set Up ICEman driver
* Install CMake (versions 3.13 and later)

All tools necessary for developing Amazon FreeRTOS demo applications on the
Corvette-F1 N25 Development Board are available at the [GitHub
repository](https://github.com/andestech/aws_development_tools.git).

    $ git clone https://github.com/andestech/aws_development_tools.git

Install the ICEman driver `zadig_20171002_win7.exe` from `<aws-development-tools>/windows/ICEman driver/zadig_20171002_win7/` first.

Use command line tool as "cmd.exe" to extract `Cygwin` and `toolchain`
```
$ cd <aws_development_tools>/windows/Cygwin
$ tar xvf Cygwin.tar.gz
$ cd <aws_development_tools>/windows/toolchain
$ tar xvf nds32le-elf-newlib-v5.txz
```
Add Cygwin and toolchain to PATH environment variables
<aws_development_tools>\windows\Cygwin\Cygwin
<aws_development_tools>\windows\toolchain\nds32le-elf-newlib-v5\bin

If you are using the Windows operating system, CMake is ready-to-use in the
Cygwin you downloaded.

**For detail:** read (Getting Started with the Andes Corvette-F1 N25)[https://github.com/andestech/amazon-freertos/blob/master/vendors/andes/boards/corvette_f1_n25/Getting_Started_Guide_Corvette-F1_N25.md] or Read **Getting_Started_Guide_Corvette-F1_N25.pdf** in `amazon-freertos/vendors/andes/boards/corvette_f1_n25/` directory 


### Build binary demo

    $ cd amazon-freertos
    $ mkdir build
    $ cmake -DVENDOR=andes -DBOARD=corvette_f1_n25 -DAFR_IS_TESTING=0 -DCOMPILER=riscv32-elf -B build
    $ cd build
    $ make

The binary `aws_demos.bin` will be generated in the directory `<amazon-freertos>/build/`.

### Board Serial Connection

## Generate root CA and root CA certificate for ATECC608A
Download and follow instruction in README.md file from https://github.com/aws/amazon-freertos/tree/master/vendors/microchip/secure_elements/app/example_trust_chain_tool

### For more information read
- [Getting started with the Microchip ATECC608A Secure Element with Windows simulator](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_atecc608a.html)
- [Developer-mode key provisioning](https://docs.aws.amazon.com/freertos/latest/userguide/dev-mode-key-provisioning.html)
AWS CLI configuration  https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-quickstart.html