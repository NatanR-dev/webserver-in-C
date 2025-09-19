#include "json.h"

#include "../../http/response/response.h"

#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Platform 
#include "../../platform/platform.h"

void jsonEscapeString(const char* input, char* output, size_t outputSize) {
    size_t i = 0;
    size_t j = 0;
    while (input[i] != '\0' && j < outputSize - 1) {
        switch (input[i]) {
            case '"': 
                if (j + 2 >= outputSize) break;
                output[j++] = '\\'; 
                output[j++] = '"'; 
                break;
            case '\\':
                if (j + 2 >= outputSize) break;
                output[j++] = '\\';
                output[j++] = '\\';
                break;
            case '\b':
                if (j + 2 >= outputSize) break;
                output[j++] = '\\';
                output[j++] = 'b';
                break;
            case '\f':
                if (j + 2 >= outputSize) break;
                output[j++] = '\\';
                output[j++] = 'f';
                break;
            case '\n':
                if (j + 2 >= outputSize) break;
                output[j++] = '\\';
                output[j++] = 'n';
                break;
            case '\r':
                if (j + 2 >= outputSize) break;
                output[j++] = '\\';
                output[j++] = 'r';
                break;
            case '\t':
                if (j + 2 >= outputSize) break;
                output[j++] = '\\';
                output[j++] = 't';
                break;
            default:
                if (input[i] < ' ') {
                } else if (j < outputSize - 1) {
                    output[j++] = input[i];
                }
                break;
        }
        i++;
    }
    output[j] = '\0';
}

int jsonExtractString(const char* json, const char* key, char* value, size_t value_size) {
    if (!json || !key || !value || value_size == 0) {
        return 0;
    }

    char key_pattern[128];
    snprintf(key_pattern, sizeof(key_pattern), "\"%s\"", key);
    size_t key_pattern_len = strlen(key_pattern);
    
    const char* key_pos = strstr(json, key_pattern);
    if (!key_pos) {
        return 0;
    }
    
    const char* colon_pos = strchr(key_pos + key_pattern_len, ':');
    if (!colon_pos) {
        return 0;
    }
    
    colon_pos++;
    while (isspace(*colon_pos)) {
        colon_pos++;
    }
    
    if (*colon_pos != '\"') {
        return 0;
    }
    
    const char* value_start = colon_pos + 1;

    const char* value_end = value_start;

    while (*value_end && *value_end != '"') {
        if (*value_end == '\\' && *(value_end + 1) == '"') {
            value_end += 2; 
        } else {
            value_end++;
        }
    }

    if (*value_end != '"') {
        return 0; 
    }

    size_t value_len = value_end - value_start;
    if (value_len >= value_size) {
        value_len = value_size - 1; 
    }

    strncpy(value, value_start, value_len);
    value[value_len] = '\0';

    char* dest = value;
    const char* src = value;
    while (*src) {
        if (*src == '\\' && *(src + 1) == '"') {
            *dest++ = '"';
            src += 2;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';

    return 1;
}

char* createJsonObject(char* buffer, size_t bufferSize, int count, ...) {
    if (count == 0 || bufferSize < 3) {
        if (bufferSize > 0) buffer[0] = '\0';
        return 0;
    }

    va_list args;
    va_start(args, count);
    
    size_t pos = 0;
    buffer[pos++] = '{';
    
    bool first = true;
    for (int i = 0; i < count; i++) {
        const char* key = va_arg(args, const char*);
        const char* value = va_arg(args, const char*);
        
        if (!key || !value) continue;
        
        if (!first) {
            if (pos < bufferSize - 1) {
                buffer[pos++] = ',';
            } else {
                break;
            }
        }
        
        size_t keyLen = strlen(key);
        if (pos + keyLen + 4 > bufferSize - 1) break; 
        
        buffer[pos++] = '"';
        strncpy(buffer + pos, key, keyLen);
        pos += keyLen;
        buffer[pos++] = '"';
        buffer[pos++] = ':';
        buffer[pos++] = '"';
        
        char escapedValue[1024];
        jsonEscapeString(value, escapedValue, sizeof(escapedValue));
        size_t valueLen = strlen(escapedValue);
        
        if (pos + valueLen + 2 > bufferSize - 1) { 
            valueLen = bufferSize - pos - 2;
        }
        
        strncpy(buffer + pos, escapedValue, valueLen);
        pos += valueLen;
        buffer[pos++] = '"';
        
        first = false;
    }
    
    va_end(args);
    
    if (pos < bufferSize - 1) {
        buffer[pos++] = '}';
        buffer[pos] = '\0';
    } else {
        buffer[bufferSize - 1] = '\0';
        pos = bufferSize - 1;
    }
    
    return buffer;
}
