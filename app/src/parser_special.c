/*******************************************************************************
*  (c) 2019 Zondax GmbH
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

#include "parser_special.h"
#include "parser_impl.h"
#include "parser_common.h"
#include "parser_txdef.h"
#include "parser.h"

#include "app_mode.h"

parser_error_t searchRuntimeArgs(char *argstr, uint8_t *type, uint8_t *internal_type, uint32_t deploy_argLen, parser_context_t *ctx) {
    uint16_t start = ctx->offset;
    char buffer[300];
    uint8_t dummy_type = 0;
    uint8_t dummy_internal = 0;
    for (uint32_t i = 0; i < deploy_argLen; i++) {
        //key
        CHECK_PARSER_ERR(copy_item_into_charbuffer(ctx, buffer, sizeof(buffer)));
        if (strcmp(buffer, argstr) == 0) {
            //value
            CHECK_PARSER_ERR(parse_item(ctx));

            CHECK_PARSER_ERR(get_type(ctx, type, internal_type));

            ctx->offset = start;
            return parser_ok;
        }
        //value
        CHECK_PARSER_ERR(parse_item(ctx));

        CHECK_PARSER_ERR(get_type(ctx, &dummy_type, &dummy_internal));

    }
    return parser_runtimearg_notfound;
}

#define CHECK_RUNTIME_ARGTYPE(CTX, NUM_ITEMS, STR, CONDITION) { \
    type = 255;                     \
    internal_type = 255;                                           \
    CHECK_PARSER_ERR(searchRuntimeArgs((STR), &type, &internal_type, (NUM_ITEMS), (CTX)));          \
    PARSER_ASSERT_OR_ERROR((CONDITION), parser_unexpected_type);                                      \
}


parser_error_t parser_getItem_NativeTransfer(ExecutableDeployItem item, parser_context_t *ctx,
                                       uint8_t displayIdx,
                                       char *outKey, uint16_t outKeyLen,
                                       char *outVal, uint16_t outValLen,
                                       uint8_t pageIdx, uint8_t *pageCount) {
    ctx->offset++;
    uint32_t num_items = 0;
    CHECK_PARSER_ERR(readU32(ctx, &num_items));

    if(item.UI_runtime_items > num_items){
        return parser_unexpected_number_items;
    }

    uint8_t new_displayIdx = displayIdx - item.UI_fixed_items;

    if (new_displayIdx < 0 || new_displayIdx > item.UI_runtime_items) {
        return parser_unexpected_number_items;
    }
    uint32_t dataLength = 0;
    uint8_t datatype = 255;

    if(!app_mode_expert()){
        if(new_displayIdx == 0) {
            snprintf(outKey, outKeyLen, "Target");
            CHECK_PARSER_ERR(parser_runtimeargs_getData("target", &dataLength, &datatype, num_items, ctx))
            return parser_display_runtimeArg(datatype, dataLength, ctx,
                                             outVal, outValLen,
                                             pageIdx, pageCount);

        }

        if(new_displayIdx == 1) {
            snprintf(outKey, outKeyLen, "Amount");
            CHECK_PARSER_ERR(parser_runtimeargs_getData("amount", &dataLength, &datatype,num_items, ctx))
            return parser_display_runtimeArg(datatype, dataLength, ctx,
                                             outVal, outValLen,
                                             pageIdx, pageCount);
        }

        return parser_no_data;
    }else{
        if(new_displayIdx == 0 && item.UI_runtime_items == 4) {
            snprintf(outKey, outKeyLen, "From");
            CHECK_PARSER_ERR(parser_runtimeargs_getData("source", &dataLength, &datatype,num_items, ctx))
            return parser_display_runtimeArg(datatype, dataLength, ctx,
                                             outVal, outValLen,
                                             pageIdx, pageCount);
        }

        if(item.UI_runtime_items == 4){
            new_displayIdx -= 1;
        }

        if(new_displayIdx == 0) {
            snprintf(outKey, outKeyLen, "Target");
            CHECK_PARSER_ERR(parser_runtimeargs_getData("target", &dataLength, &datatype, num_items, ctx))
            return parser_display_runtimeArg(datatype, dataLength, ctx,
                                             outVal, outValLen,
                                             pageIdx, pageCount);

        }

        if(new_displayIdx == 1) {
            snprintf(outKey, outKeyLen, "Amount");
            CHECK_PARSER_ERR(parser_runtimeargs_getData("amount", &dataLength, &datatype,num_items, ctx))
            return parser_display_runtimeArg(datatype, dataLength, ctx,
                                             outVal, outValLen,
                                             pageIdx, pageCount);
        }

        if(new_displayIdx == 2) {
            snprintf(outKey, outKeyLen, "ID");
            CHECK_PARSER_ERR(parser_runtimeargs_getData("id", &dataLength, &datatype,num_items, ctx))
            return parser_display_runtimeArg(datatype, dataLength, ctx,
                                             outVal, outValLen,
                                             pageIdx, pageCount);
        }

        return parser_no_data;
    }
}

parser_error_t parseNativeTransfer(parser_context_t *ctx, ExecutableDeployItem *item, uint32_t num_items) {
    PARSER_ASSERT_OR_ERROR(3 <= num_items && num_items <= 4, parser_unexpected_number_items);
    uint8_t type = 0;
    uint8_t internal_type = 0;
    CHECK_RUNTIME_ARGTYPE(ctx, num_items, "amount", type == 8);
    CHECK_RUNTIME_ARGTYPE(ctx, num_items, "id", type == 13 && internal_type == 5);
    CHECK_RUNTIME_ARGTYPE(ctx, num_items, "target", type == 11 || type == 12 || type == 15);
    if(num_items == 4){
        CHECK_RUNTIME_ARGTYPE(ctx, num_items, "source", type == 13 && internal_type == 12);
    }
    if(app_mode_expert()){
        item->UI_runtime_items += num_items;
    }else{
        item->UI_runtime_items += 2; //amount and target only
    }
    item->special_type = NativeTransfer;
    return parser_ok;
}

parser_error_t parseSystemPayment(parser_context_t *ctx, ExecutableDeployItem *item, uint32_t num_items){

    PARSER_ASSERT_OR_ERROR(num_items == 1, parser_unexpected_number_items);

    uint8_t type = 0;
    uint8_t internal_type = 0;
    CHECK_RUNTIME_ARGTYPE(ctx, num_items, "amount", type == 8);
    item->UI_runtime_items += 1; //amount only
    item->special_type = SystemPayment;
    return parser_ok;
}

parser_error_t parser_getItem_SystemPayment(ExecutableDeployItem item, parser_context_t *ctx,
                                          uint8_t displayIdx,
                                          char *outKey, uint16_t outKeyLen,
                                          char *outVal, uint16_t outValLen,
                                          uint8_t pageIdx, uint8_t *pageCount) {
    ctx->offset++;
    uint32_t dataLen = 0;
    CHECK_PARSER_ERR(readU32(ctx, &dataLen));
    if(dataLen > ctx->bufferLen - ctx->offset){
        return parser_unexpected_buffer_end;
    }
    ctx->offset += dataLen;
    CHECK_PARSER_ERR(readU32(ctx, &dataLen));

    uint8_t new_displayIdx = displayIdx - item.UI_fixed_items;
    if (new_displayIdx < 0 || new_displayIdx > item.UI_runtime_items) {
        return parser_no_data;
    }
    uint32_t dataLength = 0;
    uint8_t datatype = 255;
    if(new_displayIdx == 0) {
        snprintf(outKey, outKeyLen, "Fee");
        CHECK_PARSER_ERR(parser_runtimeargs_getData("amount", &dataLength, &datatype, item.UI_runtime_items, ctx))
        return parser_display_runtimeArg(datatype, dataLength, ctx,
                                         outVal, outValLen,
                                         pageIdx, pageCount);

    }
    return parser_no_data;
}

parser_error_t parser_getItem_Delegation(ExecutableDeployItem *item, parser_context_t *ctx,
                                            uint8_t displayIdx,
                                            char *outKey, uint16_t outKeyLen,
                                            char *outVal, uint16_t outValLen,
                                            uint8_t pageIdx, uint8_t *pageCount) {
    ctx->offset++;

    switch (item->type){
        case StoredContractByHash: {
            ctx->offset += HASH_LENGTH;
            CHECK_PARSER_ERR(parse_item(ctx))
            break;
        }
        case StoredVersionedContractByHash: {
            if(displayIdx == 0 && app_mode_expert()) {
                snprintf(outKey, outKeyLen, "Execution");
                snprintf(outVal, outValLen, "by-hash-versioned");
                return parser_ok;
            }
            if(displayIdx == 1 && app_mode_expert()) {
                snprintf(outKey, outKeyLen, "Address");
                return parser_printBytes((const uint8_t *) (ctx->buffer + ctx->offset), HASH_LENGTH, outVal, outValLen,
                                         pageIdx, pageCount);
            }
            ctx->offset += HASH_LENGTH;
            uint32_t version = 0;
            CHECK_PARSER_ERR(parse_version(ctx, &version))
            if(displayIdx == 2 && app_mode_expert()) {
                uint64_t value = 0;
                MEMCPY(&value, &version, 4);
                snprintf(outKey, outKeyLen, "Version");
                return parser_printU64(value, outVal, outValLen, pageIdx, pageCount);
            }
            CHECK_PARSER_ERR(parse_item(ctx))
            break;
        }

        default: {
            return parser_unexpected_type;
        }
    }

    uint32_t dataLen = 0;
    CHECK_PARSER_ERR(readU32(ctx, &dataLen));

    uint8_t new_displayIdx = displayIdx - item->UI_fixed_items;
    if (new_displayIdx < 0 || new_displayIdx > item->UI_runtime_items) {
        return parser_no_data;
    }
    uint32_t dataLength = 0;
    uint8_t datatype = 255;

    if(new_displayIdx == 0) {
        snprintf(outKey, outKeyLen, "Delegator");
        CHECK_PARSER_ERR(parser_runtimeargs_getData("delegator", &dataLength, &datatype, item->UI_runtime_items, ctx))
        return parser_display_runtimeArg(datatype, dataLength, ctx,
                                         outVal, outValLen,
                                         pageIdx, pageCount);

    }

    if(new_displayIdx == 1) {
        snprintf(outKey, outKeyLen, "Validator");
        CHECK_PARSER_ERR(parser_runtimeargs_getData("validator", &dataLength, &datatype, item->UI_runtime_items, ctx))
        return parser_display_runtimeArg(datatype, dataLength, ctx,
                                         outVal, outValLen,
                                         pageIdx, pageCount);

    }

    if(new_displayIdx == 2) {
        snprintf(outKey, outKeyLen, "Amount");
        CHECK_PARSER_ERR(parser_runtimeargs_getData("amount", &dataLength, &datatype, item->UI_runtime_items, ctx))
        return parser_display_runtimeArg(datatype, dataLength, ctx,
                                         outVal, outValLen,
                                         pageIdx, pageCount);

    }

    return parser_no_data;
}


parser_error_t parseDelegation(parser_context_t *ctx, ExecutableDeployItem *item, uint32_t num_items){

    PARSER_ASSERT_OR_ERROR(num_items == 3, parser_unexpected_number_items);
    uint8_t type = 0;
    uint8_t internal_type = 0;
    CHECK_RUNTIME_ARGTYPE(ctx, num_items, "delegator", type == 22);
    CHECK_RUNTIME_ARGTYPE(ctx, num_items, "validator", type == 22);
    CHECK_RUNTIME_ARGTYPE(ctx, num_items, "amount", type == 8);
    item->UI_runtime_items += 3;

    if(app_mode_expert()){
        uint8_t has_version = item->type == StoredVersionedContractByHash ? 1 : 0;
        item->UI_fixed_items = 2 + has_version; //type
    }

    item->special_type = Delegate;
    return parser_ok;
}