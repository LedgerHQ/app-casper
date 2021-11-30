cmake_minimum_required(VERSION 3.10)

project(CasperFuzzer VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,bugprone-*,cert-*,clang-analyzer-*,-cert-err58-cpp,misc-*)

# BOLOS SDK
set(BOLOS_SDK $ENV{BOLOS_SDK})
add_compile_definitions(IO_HID_EP_LENGTH=64)

# Fuzzer target
set(APP_SRC_DIR "../")

add_compile_definitions(HAVE_ECC HAVE_HASH HAVE_SHA256 HAVE_RIPEMD160 HAVE_BLAKE2)
include_directories(.
        "${BOLOS_SDK}/include/"
        "${BOLOS_SDK}/lib_ux/include/"
        "${BOLOS_SDK}/lib_cxng/include/"
        #"${BOLOS_SDK}/lib_cxng/src/"
        "${APP_SRC_DIR}/deps/BLAKE2/ref/"
        "${APP_SRC_DIR}/app/src/"
        "${APP_SRC_DIR}/app/src/common/"
        #"${APP_SRC_DIR}/app/glyphs/"
        "${APP_SRC_DIR}/deps/ledger-zxlib/include/"
        #"${APP_SRC_DIR}/deps/ledger-zxlib/src/" 
        #"${APP_SRC_DIR}/deps/ledger-zxlib/app/common/"
)

add_definitions(
    #-DAPP_STANDARD
    #-DHAVE_PENDING_REVIEW_SCREEN
    #-DAPPVERSION=2.2.6
    #-DLEDGER_MAJOR_VERSION=2
    #-DLEDGER_MINOR_VERSION=2
    #-DLEDGER_PATCH_VERSION=6
    #-DOS_IO_SEPROXYHAL
    #-DHAVE_BAGL -DHAVE_SPRINTF
    #-DHAVE_IO_USB -DHAVE_L4_USBLIB -DIO_USB_MAX_ENDPOINTS=7 #-DIO_HID_EP_LENGTH=64 -DHAVE_USB_APDU
    #-DUSB_SEGMENT_SIZE=64
    #-DHAVE_BOLOS_APP_STACK_CANARY
    #-DHAVE_WEBUSB -DWEBUSB_URL_SIZE_B=0 -DWEBUSB_URL=""
    #-DIO_SEPROXYHAL_BUFFER_SIZE_B=128

    #-DHDPATH_LEN_DEFAULT=5
    #-DCX_RIPEMD160_SIZE=20
    -DTARGET_NANOS
    -DCX_CURVE_256K1=33
    #-DHAVE_BLAKE2
)

add_compile_options(-g -ggdb2 -O3)

# Build with code coverage generation
if(CODE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
        add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
        add_link_options(-fprofile-instr-generate -fcoverage-mapping)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
        add_compile_options(-fprofile-arcs -ftest-coverage)
        link_libraries(gcov)
    else()
        message(FATAL_ERROR "Unsupported compiler used with code coverage generation")
    endif()
endif()



set(APP_SOURCES
        #${APP_SRC_DIR}/deps/nanos-secure-sdk/src/pic.c

        #${APP_SRC_DIR}/deps/ledger-zxlib/src/hexutils.c
        ${APP_SRC_DIR}/deps/ledger-zxlib/src/app_mode.c
        ${APP_SRC_DIR}/deps/ledger-zxlib/src/bignum.c
        ${APP_SRC_DIR}/deps/ledger-zxlib/src/zxmacros.c
        #${APP_SRC_DIR}/deps/ledger-zxlib/src/zxformat.c
        ${APP_SRC_DIR}/deps/ledger-zxlib/src/timeutils.c
        ${APP_SRC_DIR}/deps/ledger-zxlib/src/sigutils.c
        #${APP_SRC_DIR}/deps/BLAKE2/ref/blake2b-ref.c
        #${APP_SRC_DIR}/deps/BLAKE2/ref/blake2s-ref.c
        #${APP_SRC_DIR}/deps/BLAKE2/neon/blake2b.c
        
        #${APP_SRC_DIR}/deps/tinycbor/src/cborparser.c
        #${APP_SRC_DIR}/deps/tinycbor/src/cborvalidation.c

        ${APP_SRC_DIR}/app/src/parser.c
        ${APP_SRC_DIR}/app/src/parser_special.c
        ${APP_SRC_DIR}/app/src/parser_impl.c
        ${APP_SRC_DIR}/app/src/crypto.c
)

add_executable(fuzz_message
        fuzz_casper.c
        os_mocks.c
        ${APP_SOURCES}
)

target_compile_options(fuzz_message PUBLIC -include "bsd/string.h" -fsanitize=fuzzer,address,undefined)
target_link_options(fuzz_message PUBLIC -lbsd -fsanitize=fuzzer,address,undefined)