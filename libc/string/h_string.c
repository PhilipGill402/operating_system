#include <string.h>

string_t string_create() {
    string_t string;

    string.str = malloc(sizeof(char) * 10);
    string.capacity = 10;
    string.len = 0;

    return string;
}

string_t string_literal(const char* str) {
    string_t string;
    string.len = c_strlen(str);
    string.capacity = string.len * 2;
    string.str = malloc(sizeof(char) * string.capacity); // this needs to be allocated on the heap so that when we release the string we can just call 'free()'
    h_strcpy_cstr(&string, str);

    return string;
}

void string_free(string_t* str) {
    free(str->str);

    str->str = NULL;
    str->len = 0;
    str->capacity = 0;
}

char* string_to_literal(string_t* str) {
    if (!str) return NULL;

    char* cstr = malloc(str->len + 1);
    if (!cstr) return NULL;

    memcpy(cstr, str->str, str->len);
    cstr[str->len] = '\0';

    return cstr;
}

string_t string_clone(string_t* str) {
    string_t new = string_create();
    new.len = str->len;
    new.capacity = str->capacity;
    
    // we dont wan them to point to the same heap array though
    new.str = malloc(new.capacity);
    if (!new.str) {
        return new;
    }

    memcpy(new.str, str->str, new.len);

    return new;
}

void string_clear(string_t* str) {
    memset(str->str, 0, str->capacity); 
    str->len = 0;
}

int string_resize(string_t* str, int capacity) {
    char* tmp = realloc(str, sizeof(char) * capacity);
    if (!tmp)
        return -1;

    str->str = tmp;
    str->capacity = capacity;

    return capacity;
}

void h_strcat_c(string_t* string, char ch) {
    if (string->len + 1 > string->capacity) {
        string->str = realloc(string->str, sizeof(char) * 2 * (string->len + 1));

        if (!string->str) {
            return;
        }

        string->capacity = 2 * (string->len + 1);
    }

    string->str[string->len] = ch;
    string->len++;
}

void h_strcat_hstr(string_t* dst, string_t* src) {
    if (dst->capacity < src->len + dst->len) {
        dst->str = realloc(dst->str, sizeof(char) * 2 * (src->len + dst->len));

        if (!dst->str) {
            return;
        }

        dst->capacity = 2 * (src->len + dst->len);
    }
    
    for (uint32_t i = dst->len, j = 0; i < dst->len + src->len; i++, j++) {
        dst->str[i] = src->str[j];
    }

    dst->len += src->len;
}

void h_strcat_cstr(string_t* dst, const char* src) {
    while (*src != '\0') {
        h_strcat_c(dst, *src);
        src++;
    } 
}

int h_strcmp_hstr(string_t* a, string_t* b) {
    if (a->len == 0 && b->len == 0) return 0;
    else if (a->len == 0) return b->str[0];
    else if (b->len == 0) return a->str[0];

    for (uint32_t i = 0; i < a->len && i < b->len; i++) {
        if (a->str[i] != b->str[i]) {
            return a->str[i] - b->str[i];
        }
    } 

    return 0;
}

int h_strcmp_cstr(string_t* a, const char* b) {
    for (uint32_t i = 0; i < a->len && b[i] != '\0'; i++) {
        if (a->str[i] != b[i]) {
            return a->str[i] - b[i];
        }
    }

    return 0;
}

void h_strcpy_hstr(string_t* dst, string_t* src) {
    if (src->len > dst->capacity) {
        dst->str = realloc(dst->str, sizeof(char) * 2 * src->len);
        
        if (!dst->str) {
            return;
        }

        dst->capacity = 2 * src->len;
    } 

    for (uint32_t i = 0; i < src->len; i++) {
        dst->str[i] = src->str[i];
    }

    dst->len = src->len;
}

void h_strcpy_cstr(string_t* dst, const char* src) {
    int src_len = c_strlen(src);

    if (src_len > (int)dst->capacity) {
        dst->str = realloc(dst->str, sizeof(char) * 2 * src_len);

        if (!dst->str)
            return;

        dst->capacity = sizeof(char) * 2 * src_len;
    }

    for (int i = 0; i < src_len; i++)
        dst->str[i] = src[i];

    dst->len = src_len;
}


uint32_t h_strlen(string_t* string) {
    return string->len;
}

vector_t h_strtok(string_t* str, char a) {
    vector_t tokens = vector_create(sizeof(string_t));
    string_t token = string_create();

    for (uint32_t i = 0; i < str->len; i++) {
        if (str->str[i] == a && token.len > 0) {
            string_t copy = string_clone(&token);
            vector_push_back(&tokens, &copy);
            string_clear(&token);
        }
        else {
            h_strcat_c(&token, str->str[i]);
        }  
    }

    string_t copy = string_clone(&token);
    vector_push_back(&tokens, &copy);
    
    string_free(&token);

    return tokens;
}

int h_strncmp_hstr(string_t* a, string_t* b, size_t len) {
    for (uint32_t idx = 0; idx < a->len && idx < b->len && idx < len; idx++) {
        int diff = (int)a->str[idx] - (int)b->str[idx];

        if (diff != 0)
            return diff;
    }

    return 0;
}

int h_strncmp_cstr(string_t* a, const char* b, size_t len) {
    for (uint32_t idx = 0; idx < a->len && b[idx] != '\0' && idx < len; idx++) {
        int diff = (int)a->str[idx] - (int)b[idx];

        if (diff != 0)
            return diff;
    }

    return 0;
}

void h_strncpy_hstr(string_t* dst, string_t* src, size_t len) {
    uint32_t i;
    for (i = 0; i < src->len && i < len; i++) {
        if (i >= dst->capacity) {
            int ret = string_resize(dst, dst->capacity * 2);
            // we failed to allocate memory so just exit 
            if (ret < 0)
                return;
        }

        dst->str[i] = src->str[i];
    }
    
    dst->len = i;
}

void h_strncpy_cstr(string_t* dst, char* src, size_t len) {
    uint32_t i;
    for (i = 0; src[i] != '\0' && i < len; i++) {
        if (i >= dst->capacity) {
            int ret = string_resize(dst, dst->capacity * 2);
            // we failed to allocate memory so just exit 
            if (ret < 0)
                return;
        }

        dst->str[i] = src[i];
    }
    
    dst->len = i; 
}

void h_strncat_hstr(string_t* dst, string_t* src, size_t len) {
    for (uint32_t i = 0; i < src->len && i < len; i++)
        h_strcat_c(dst, src->str[i]);
}

void h_strncat_cstr(string_t* dst, char* src, size_t len) {
    for (uint32_t i = 0; src[i] != '\0' && i < len; i++)
        h_strcat_c(dst, src[i]);
}

