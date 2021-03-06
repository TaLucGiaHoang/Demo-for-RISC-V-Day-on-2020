set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
set(bsp_dir "${CMAKE_CURRENT_LIST_DIR}/../../V5/bsp")
set(v5_source "${CMAKE_CURRENT_LIST_DIR}/../../V5")
set(amsi_dir "${CMAKE_CURRENT_LIST_DIR}/../../V5/AMSI")
set(wifi_dir "${CMAKE_CURRENT_LIST_DIR}/../../V5/wifi")
set(secure_elements_dir "${CMAKE_CURRENT_LIST_DIR}/../../secure_elements")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TESTS_DIR}/include")
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "Corvette-F1 N25")
afr_set_board_metadata(DISPLAY_NAME "Corvette-F1 N25")
afr_set_board_metadata(DESCRIPTION "Development kit for Corvette-F1 N25")
afr_set_board_metadata(VENDOR_NAME "Andes")
afr_set_board_metadata(FAMILY_NAME "AE250")
afr_set_board_metadata(DATA_RAM_MEMORY "200KB")
afr_set_board_metadata(PROGRAM_MEMORY "1MB")
afr_set_board_metadata(CODE_SIGNER "null")
afr_set_board_metadata(IS_ACTIVE "TRUE")

afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)
if(AFR_IS_TESTING)
    target_compile_definitions(
        AFR::compiler::mcu_port
        INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
        -DportasmHANDLE_INTERRUPT=FreeRTOS_IRQ_handler
        -D__freertos_irq_stack_top=_stack
        -DUNITY_INCLUDE_CONFIG_H
        -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS
        #-DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"
        -DCFG_MAKEFILE
        -DCFG_XIP
        -DCFG_CACHE_ENABLE
        -DCFG_FLASHEXEC
    )
else()
    target_compile_definitions(
        AFR::compiler::mcu_port
        INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
        -DportasmHANDLE_INTERRUPT=FreeRTOS_IRQ_handler
        -D__freertos_irq_stack_top=_stack
        -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"
        -DCFG_MAKEFILE
        -DCFG_XIP
        -DCFG_CACHE_ENABLE
        -DCFG_FLASHEXEC
    )
endif()


target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
    ## predefined macro used for atecc608a lib
    #-DMBEDTLS_CONFIG_FILE=\"aws_mbedtls_config.h\"
    # use CONFIG_MEDTLS_USE_AFR_MEMORY macro for iot_crypto.c V1.0.8 and above
    #-DCONFIG_MEDTLS_USE_AFR_MEMORY
    -D__free_rtos__
    -D_CRT_SECURE_NO_WARNINGS
    -DATCA_NO_HEAP
)

target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:ASM>:${assembler_defined_symbols}>
    -D__ASSEMBLY__
)

set (compiler_flags -O3 -g3 -mcmodel=large -fomit-frame-pointer -fno-strict-aliasing -fno-builtin -funroll-loops -ffunction-sections -fdata-sections -std=gnu99)
set (assembler_flags -O3 -g3 -mcmodel=large -fomit-frame-pointer -fno-strict-aliasing -fno-builtin -funroll-loops -ffunction-sections -fdata-sections)

target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${compiler_flags}>
)
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:ASM>:${assembler_flags}>
)


# Linker flags
set(
   linker_flags -T${board_dir}/application_code/nds_code/ae250.ld -O3 -g3 -static -nostartfiles -Wl,-error-limit=0,--gc-sections,--defsym=__rtos_signature_freertos_v10_1_1=0x0 -mcmodel=large
)

target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${linker_flags}>
)

target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE ${link_dependent_libs}
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Normally the portable layer for kernel should be vendor's driver code.
afr_mcu_port(kernel)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE

        "${AFR_KERNEL_DIR}/portable/GCC/RISC-V/port.c"
	"${AFR_KERNEL_DIR}/portable/GCC/RISC-V/portASM.S"
	"${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"

	"${v5_source}/FreeRTOS_tick_config.c"
        "${bsp_dir}/reset.c"
        "${bsp_dir}/initfini.c"
        "${bsp_dir}/trap.c"
        "${bsp_dir}/cache.c"
	"${bsp_dir}/start.S"
        "${bsp_dir}/ae250/ae250.c"
        "${bsp_dir}/ae250/interrupt.c"
        "${bsp_dir}/ae250/iochar.c"
        "${bsp_dir}/libc/stdio/do_printf.c"
        "${bsp_dir}/libc/stdio/printf.c"
        "${bsp_dir}/driver/uart.c"
        "${afr_ports_dir}/i2c_eeprom/src/i2c_eeprom.c"
        "${amsi_dir}/src/ae250/dma_ae250.c"
        "${amsi_dir}/src/ae250/gpio_ae250.c"
        "${amsi_dir}/src/ae250/i2c_ae250.c"
        "${amsi_dir}/src/ae250/spi_ae250.c"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${bsp_dir}"
        "${bsp_dir}/ae250"
        "${AFR_KERNEL_DIR}/portable/GCC/RISC-V"
        "${AFR_KERNEL_DIR}/portable/GCC/RISC-V/chip_specific_extensions/Andes_V5_RV32RV64"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/GCC"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/test"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/tls/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/pkcs11/include"
	"${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/utils/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/aws/ota/test"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/wifi/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/platform/freertos/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/platform/include"
	"${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/mbedtls"
	"${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
	"${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/src"
        "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/include"
        "${bsp_dir}/driver/include"
        "${afr_ports_dir}/i2c_eeprom/inc"
        "${amsi_dir}/src/ae250"
        "${amsi_dir}/src/ae250/config"
        "${amsi_dir}/include"

        "${board_dir}/config_files"
        "${board_dir}/application_code"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
        "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/Include"
	"${AFR_3RDPARTY_DIR}/pkcs11"
	"${AFR_DEMOS_DIR}/dev_mode_key_provisioning/include"
        "${secure_elements_dir}/lib"
        "${board_dir}/config_files"
)

# SHC added
# "${afr_ports_dir}/wifi/esp32_AT.c"
#
# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE
        "${afr_ports_dir}/wifi/iot_wifi.c"
        "${wifi_dir}/esp32/esp8266_wifi.c"
#        "${afr_ports_dir}/wifi/iot_wifi_assert.c"
#        "${wifi_dir}/esp32/esp32_AT.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/wdrvext_wilc1000.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/wilc1000_task.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/common/source/nm_common.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/spi_flash/source/spi_flash.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/nmdrv.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/nmbus.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/m2m_periph.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/m2m_wifi.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/m2m_hif.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/nmasic.c"
#        "${wifi_dir}/wilc1000/dev/spi/wdrv_wilc1000_spi.c"
#        "${wifi_dir}/wilc1000/dev/gpio/wdrv_wilc1000_gpio.c"
#        "${wifi_dir}/wilc1000/dev/gpio/wdrv_wilc1000_eint.c"
#        "${wifi_dir}/wilc1000/osal/wdrv_wilc1000_osal.c"
#        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_main.c"
#        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_connmgr.c"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/nmspi.c"
#        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_scan_helper.c"
#        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_events.c"
#        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_config.c"
#        "${wifi_dir}/wilc1000/include/tmr/sys_tmr.c"
#        "${wifi_dir}/framework/osal/src/osal_freertos.c"
#        "${wifi_dir}/framework/tcpip/src/tcpip_helpers.c"
)

target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
    	"${wifi_dir}/esp32/"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source"
#	"${wifi_dir}/wilc1000/wireless_driver_extension"
#        "${wifi_dir}/wilc1000/include"
#        "${wifi_dir}/wilc1000/include/tmr"
#        "${wifi_dir}/wilc1000/wireless_driver/include"
#        "${wifi_dir}/framework"
	"${v5_source}"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"

)

# FreeRTOS Plus TCP
#afr_mcu_port(freertos_plus_tcp)
#target_sources(
#    AFR::freertos_plus_tcp::mcu_port
#    INTERFACE
#        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/andes/BufferAllocation_2.c"
#        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/andes/NetworkInterface.c"
#	"${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/FreeRTOS_IP.c"
#)

#target_include_directories(
#    AFR::freertos_plus_tcp::mcu_port
#    INTERFACE
#        "${wifi_dir}/wilc1000/wireless_driver_extension"
#        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/include"
#	"${wifi_dir}/wilc1000/include"
#	"${wifi_dir}/wilc1000/include/tmr"
#	"${wifi_dir}/framework"
#	"${v5_source}"
#)


# PKCS11
afr_mcu_port(pkcs11_implementation DEPENDS AFR::pkcs11_mbedtls)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/iot_pkcs11_pal.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_attrib.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_cert.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_config.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_digest.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_find.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_info.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_init.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_key.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_main.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_mech.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_object.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_os.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_session.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_signature.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_slot.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_token.c"
        "${secure_elements_dir}/lib/pkcs11/pkcs11_util.c"
        "${secure_elements_dir}/lib/host/atca_host.c"
        "${secure_elements_dir}/lib/hal/atca_hal.c"
        "${secure_elements_dir}/lib/hal/hal_freertos.c"
        "${secure_elements_dir}/lib/hal/hal_andes_i2c.c"
        "${secure_elements_dir}/lib/crypto/hashes/sha1_routines.c"
        "${secure_elements_dir}/lib/crypto/hashes/sha2_routines.c"
        "${secure_elements_dir}/lib/crypto/atca_crypto_sw_sha1.c"
        "${secure_elements_dir}/lib/crypto/atca_crypto_sw_sha2.c"
        "${secure_elements_dir}/lib/basic/atca_basic.c"
        "${secure_elements_dir}/lib/basic/atca_basic_gendig.c"
        "${secure_elements_dir}/lib/basic/atca_basic_genkey.c"
        "${secure_elements_dir}/lib/basic/atca_basic_info.c"
        "${secure_elements_dir}/lib/basic/atca_basic_lock.c"
        "${secure_elements_dir}/lib/basic/atca_basic_nonce.c"
        "${secure_elements_dir}/lib/basic/atca_basic_privwrite.c"
        "${secure_elements_dir}/lib/basic/atca_basic_random.c"
        "${secure_elements_dir}/lib/basic/atca_basic_read.c"
        "${secure_elements_dir}/lib/basic/atca_basic_sign.c"
        "${secure_elements_dir}/lib/basic/atca_basic_updateextra.c"
        "${secure_elements_dir}/lib/basic/atca_basic_verify.c"
        "${secure_elements_dir}/lib/basic/atca_basic_write.c"
        "${secure_elements_dir}/lib/basic/atca_helpers.c"
        "${secure_elements_dir}/lib/atcacert/atcacert_client.c"
        "${secure_elements_dir}/lib/atcacert/atcacert_date.c"
        "${secure_elements_dir}/lib/atcacert/atcacert_def.c"
        "${secure_elements_dir}/lib/atcacert/atcacert_der.c"
        "${secure_elements_dir}/lib/atca_command.c"
        "${secure_elements_dir}/lib/atca_device.c"
        "${secure_elements_dir}/lib/atca_execution.c"
        "${secure_elements_dir}/lib/atca_iface.c"
        "${secure_elements_dir}/test/atca_basic_tests.c"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/ecc608a/atca_cert_chain.c"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/ecc608a/iot_pkcs11_secure_element.c"
#"${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/mbedtls/iot_pkcs11_mbedtls.c"
)
target_link_libraries(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE 3rdparty::mbedtls
)

# Secure sockets
afr_mcu_port(secure_sockets)

target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE 
        "${afr_ports_dir}/secure_sockets/iot_secure_sockets.c"
)

target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE
        AFR::tls
        AFR::wifi
)

target_include_directories(
    AFR::secure_sockets::mcu_port
    INTERFACE
        "${wifi_dir}/esp32"
)

## Secure sockets
#afr_mcu_port(secure_sockets)
## Link to AFR::secure_sockets_freertos_tcp if you want use default implementation based on
## FreeRTOS-Plus-TCP.
#target_link_libraries(
#    AFR::secure_sockets::mcu_port
#    INTERFACE AFR::secure_sockets_freertos_plus_tcp
#)
## Or provide your own implementation.
#target_sources(
#    AFR::secure_sockets::mcu_port
#    INTERFACE "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/freertos_plus_tcp/iot_secure_sockets.c"
#)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")
add_executable(
    ${exe_target}
    ${config_files}
    "${board_dir}/application_code/nds_code/ae250.ld"
    "${board_dir}/application_code/main.c"
    "${board_dir}/application_code/demo_control_led.c"
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
	${link_extra_flags}
)

if(AFR_METADATA_MODE)
    return()
endif()


# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")


set_source_files_properties(${AFR_MODULES_C_SDK_DIR}/common/logging/aws_logging_task_dynamic_buffers.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format -Wno-uninitialized"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_pal.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign -Wno-sizeof-pointer-memaccess"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_agent.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign"
)

set_source_files_properties(${AFR_DEMOS_DIR}/posix/aws_test_posix_pthread.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-int-conversion"
)

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")


find_program(riscv32-elf-objdump riscv32-elf-objdump)
find_program(riscv32-elf-objcopy riscv32-elf-objcopy)
find_program(riscv32-elf-gcc riscv32-elf-gcc)

set(output_bin_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.bin")
set(output_elf_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.elf")

add_custom_command(
    TARGET ${exe_target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}" 
    COMMAND "echo" "Creating .bin file"
    COMMAND "${riscv32-elf-objcopy}" -O binary -S ${output_elf_file} ${output_bin_file}
    COMMAND "${CMAKE_COMMAND}" -E copy "${output_bin_file}" "${CMAKE_BINARY_DIR}" 
)
