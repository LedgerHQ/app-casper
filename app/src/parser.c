/*******************************************************************************
*   (c) 2019 Zondax GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include <stdio.h>
#include <zxmacros.h>
#include "parser_impl.h"
#include "parser.h"
#include "coin.h"

#if defined(TARGET_NANOX)
// For some reason NanoX requires this function
void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function){
    while(1) {};
}
#endif

parser_error_t parser_parse(parser_context_t *ctx, const uint8_t *data, size_t dataLen) {
    CHECK_PARSER_ERR(parser_init(ctx, data, dataLen))
    parser_error_t err =  _read(ctx, &parser_tx_obj);
    return err;
}

parser_error_t parser_validate(const parser_context_t *ctx) {
    CHECK_PARSER_ERR(_validateTx(ctx, &parser_tx_obj))

    // Iterate through all items to check that all can be shown and are valid
    uint8_t numItems = 0;
    CHECK_PARSER_ERR(parser_getNumItems(ctx, &numItems));

    char tmpKey[40];
    char tmpVal[40];

    for (uint8_t idx = 0; idx < numItems; idx++) {
        uint8_t pageCount = 0;
        CHECK_PARSER_ERR(parser_getItem(ctx, idx, tmpKey, sizeof(tmpKey), tmpVal, sizeof(tmpVal), 0, &pageCount))
    }

    return parser_ok;
}

parser_error_t parser_getNumItems(const parser_context_t *ctx, uint8_t *num_items) {
    *num_items = _getNumItems(ctx, &parser_tx_obj);
    return parser_ok;
}

parser_error_t parser_getItem(const parser_context_t *ctx,
                              uint8_t displayIdx,
                              char *outKey, uint16_t outKeyLen,
                              char *outVal, uint16_t outValLen,
                              uint8_t pageIdx, uint8_t *pageCount) {
    MEMZERO(outKey, outKeyLen);
    MEMZERO(outVal, outValLen);
    snprintf(outKey, outKeyLen, "?");
    snprintf(outVal, outValLen, "?");
    *pageCount = 1;

    uint8_t numItems = 0;
    CHECK_PARSER_ERR(parser_getNumItems(ctx, &numItems))
    CHECK_APP_CANARY()

    if (displayIdx < 0 || displayIdx >= numItems) {
        return parser_no_data;
    }

//    if (displayIdx == 0) {
//        snprintf(outKey, outKeyLen, "To");
//        return parser_printAddress(&parser_tx_obj.to,
//                                   outVal, outValLen, pageIdx, pageCount);
//    }
//
//    if (displayIdx == 1) {
//        snprintf(outKey, outKeyLen, "From");
//        return parser_printAddress(&parser_tx_obj.from,
//                                   outVal, outValLen, pageIdx, pageCount);
//    }
//
//    if (displayIdx == 2) {
//        snprintf(outKey, outKeyLen, "Nonce");
//        if (uint64_to_str(outVal, outValLen, parser_tx_obj.nonce) != NULL) {
//            return parser_unexepected_error;
//        }
//        *pageCount = 1;
//        return parser_ok;
//    }
//
//    if (displayIdx == 3) {
//        snprintf(outKey, outKeyLen, "Value");
//        return parser_printBigIntFixedPoint(&parser_tx_obj.value, outVal, outValLen, pageIdx, pageCount);
//    }
//
//    if (displayIdx == 4) {
//        snprintf(outKey, outKeyLen, "Gas Limit");
//        if (int64_to_str(outVal, outValLen, parser_tx_obj.gaslimit) != NULL) {
//            return parser_unexepected_error;
//        }
//        *pageCount = 1;
//        return parser_ok;
//    }
//
//    if (displayIdx == 5) {
//        snprintf(outKey, outKeyLen, "Gas Premium");
//        return parser_printBigIntFixedPoint(&parser_tx_obj.gaspremium, outVal, outValLen, pageIdx, pageCount);
//    }
//
//    if (displayIdx == 6) {
//        snprintf(outKey, outKeyLen, "Gas Fee Cap");
//        return parser_printBigIntFixedPoint(&parser_tx_obj.gasfeecap, outVal, outValLen, pageIdx, pageCount);
//    }
//
//    if (displayIdx == 7) {
//        snprintf(outKey, outKeyLen, "Method");
//        *pageCount = 1;
//        switch(parser_tx_obj.method) {
//            case method0:
//                snprintf(outVal, outValLen, "Transfer");
//                return parser_ok;
//            case method1:
//                snprintf(outVal, outValLen, "Method1");
//                return parser_ok;
//            case method2:
//                snprintf(outVal, outValLen, "Method2");
//                return parser_ok;
//            case method3:
//                snprintf(outVal, outValLen, "Method3");
//                return parser_ok;
//            case method4:
//                snprintf(outVal, outValLen, "Method4");
//                return parser_ok;
//            case method5:
//                snprintf(outVal, outValLen, "Method5");
//                return parser_ok;
//            case method6:
//                snprintf(outVal, outValLen, "Method6");
//                return parser_ok;
//            case method7:
//                snprintf(outVal, outValLen, "Method7");
//                return parser_ok;
//        }
//        return parser_unexpected_method;
//    }
//
//    if (displayIdx == 8) {
//        *pageCount = 1;
//        snprintf(outKey, outKeyLen, "Params");
//        snprintf(outVal, outValLen, "Not Available");
//        return parser_ok;
//    }

    return parser_ok;
}
