/* ================================================================
   GYMCONTROL — main.c
   Ponto de entrada: inicia diretórios e o servidor HTTP.
   ================================================================ */
#include "gymcontrol.h"

#define PORTA_API 8080

int main(void) {
    utils_criar_dirs();
    server_start(PORTA_API);
    return 0;
}
