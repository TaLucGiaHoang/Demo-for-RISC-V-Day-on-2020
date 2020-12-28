# 20_Demo-for-RISC-V-Day-on-2020

## Branches information
master --> )
Update_OTA --> Fully tested released source code
## Important branches to know
master            --> Basic Amazon FreeRTOS demo `aws_demo` for Andes Corvette-F1 board with hardware security (ATECC608A device)  
Update_OTA           --> Developing bootloader demo for supporting Update OTA  


## Setup environment
For more detail of guideline please read (Getting Started with the Andes Corvette-F1 N25)[https://github.com/andestech/amazon-freertos/blob/master/vendors/andes/boards/corvette_f1_n25/Getting_Started_Guide_Corvette-F1_N25.md] or read **Getting_Started_Guide_Corvette-F1_N25.pdf** in `amazon-freertos/vendors/andes/boards/corvette_f1_n25/` directory 

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

Use command line tool as "cmd.exe" to extract the compressed Cygwin package (`Cygwin.tar.gz`) and the compressed toolchain `nds32le-elf-newlib-v5.txz`

```
$ cd <aws_development_tools>/windows/Cygwin
$ tar xvf Cygwin.tar.gz
$ cd <aws_development_tools>/windows/toolchain
$ tar xvf nds32le-elf-newlib-v5.txz
```

Add Cygwin and toolchain to `PATH` environment variables
<aws_development_tools>\windows\Cygwin\Cygwin
<aws_development_tools>\windows\toolchain\nds32le-elf-newlib-v5\bin

If you are using the Windows operating system, CMake is ready-to-use in the
Cygwin you downloaded.

### Build binary demo
You will find the Cygwin batch file (`Cygwin.bat`) in the extracted directory `<aws_development_tools>\windows\Cygwin\Cygwin`. Execute the batch file to launch the Cygwin terminal which allows you to perform any operation for developing Amazon FreeRTOS applications on Windows. 
Then change directory to amazon-freertos root source to create `Makefile` file and build

    $ cd <amazon-freertos>
    $ mkdir build
    $ cmake -DVENDOR=andes -DBOARD=corvette_f1_n25 -DAFR_IS_TESTING=0 -DCOMPILER=riscv32-elf -B build
    $ cd build
    $ make

The binary `aws_demos.bin` will be generated in the directory `<amazon-freertos>/build/`.

### UART Serial Connection

	+ BAUD rate: 115200
	+ Data: 8 bit 
	+ Parity: None
	+ Stop: 1 bit
	+ Flow control: None

### Burn the Application to Flash and Run
Make sure to run the ICEMan program before burning. 
Open the first Cygwin terminal and issue the following commands

    $ cd "<aws_development_tools>/windows/ICEman"
    $ ./ICEman.exe -Z v5

Then, open the second Cygwin terminal and issue the following commands to burn binary onto flash

    $ cd <amazon-freertos>/vendors/andes/tools
    $ ./target_burn_windows.sh <amazon-freertos>/build/aws_demos.bin

You can also directly use this command to burn onto flash
    
    $ ./windows_burner_tools/target_burn_frontend --erase-all --verify -P 4444 --algorithm-bin=./target_bin/target_SPI_v5_3
2.bin --image=<amazon-freertos>/build/aws_demos.bin

>**NOTE**<br>
>Be sure to keep ICEman running when you are debugging or burning a flash.


## Generate root CA and root CA certificate for ATECC608A
Download scripts from https://github.com/aws/amazon-freertos/tree/master/vendors/microchip/secure_elements/app/example_trust_chain_tool and follow instruction in README.md file 

### For more information read
- [Getting started with the Microchip ATECC608A Secure Element with Windows simulator](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_atecc608a.html)
- [Developer-mode key provisioning](https://docs.aws.amazon.com/freertos/latest/userguide/dev-mode-key-provisioning.html)
AWS CLI configuration  https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-quickstart.html