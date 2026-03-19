/* ================================================================
   GYMCONTROL — json_parser.c
   Parser JSON minimalista: extrai string, int e float de um JSON
   plano (sem arrays nem objetos aninhados no mesmo nível).
   ================================================================ */
#include "gymcontrol.h"

/* Busca "key": no json e retorna ponteiro para o início do valor */
static const char *find_value(const char *json, const char *key) {
    char search[128];
    snprintf(search, sizeof(search), "\"%s\"", key);
    const char *p = strstr(json, search);
    if (!p) return NULL;
    p += strlen(search);
    while (*p == ' ' || *p == ':' || *p == '\t') p++;
    return p;
}

/* Extrai string: "key":"valor"  →  dest = "valor" */
int json_get_string(const char *json, const char *key,
                    char *dest, int maxlen) {
    const char *p = find_value(json, key);
    if (!p) { dest[0] = '\0'; return GC_NAO_ENCONTRADO; }

    if (*p == '"') {
        p++;                          /* pula a aspas de abertura */
        int i = 0;
        while (*p && *p != '"' && i < maxlen - 1) {
            /* trata escape simples \n \t \\ \" */
            if (*p == '\\' && *(p+1)) {
                p++;
                switch (*p) {
                    case 'n':  dest[i++] = '\n'; break;
                    case 't':  dest[i++] = '\t'; break;
                    default:   dest[i++] = *p;   break;
                }
            } else {
                dest[i++] = *p;
            }
            p++;
        }
        dest[i] = '\0';
    } else {
        /* valor sem aspas (número como string) */
        int i = 0;
        while (*p && *p != ',' && *p != '}' && *p != '\n' && i < maxlen-1)
            dest[i++] = *p++;
        dest[i] = '\0';
    }
    return GC_OK;
}

/* Extrai inteiro: "key":42 */
int json_get_int(const char *json, const char *key) {
    char buf[32];
    if (json_get_string(json, key, buf, sizeof(buf)) != GC_OK)
        return 0;
    return atoi(buf);
}

/* Extrai float: "key":3.14 */
float json_get_float(const char *json, const char *key) {
    char buf[32];
    if (json_get_string(json, key, buf, sizeof(buf)) != GC_OK)
        return 0.0f;
    return (float)atof(buf);
}
