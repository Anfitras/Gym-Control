/* ================================================================
   GYMCONTROL — utils.c
   Funções auxiliares: diretórios, IDs, datas, enums → string
   ================================================================ */
#include "gymcontrol.h"

#ifdef _WIN32
  #include <direct.h>
  #define mkdir_p(p) _mkdir(p)
#else
  #include <sys/stat.h>
  #define mkdir_p(p) mkdir(p, 0755)
#endif

void utils_criar_dirs(void) {
    mkdir_p("data");
}

/* Lê o maior id do arquivo binário e retorna maior+1 */
int utils_proximo_id(const char *arq, size_t sz) {
    FILE *fp = fopen(arq, "rb");
    if (!fp) return 1;
    int maior = 0, id = 0;
    while (fread(&id, sizeof(int), 1, fp) == 1) {
        if (id > maior) maior = id;
        fseek(fp, (long)(sz - sizeof(int)), SEEK_CUR);
    }
    fclose(fp);
    return maior + 1;
}

void utils_data_hoje(char *dest) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(dest, 12, "%d/%m/%Y", tm);
}

const char *utils_status_str(StatusCadastro s) {
    switch (s) {
        case STATUS_ATIVO:    return "Ativo";
        case STATUS_INATIVO:  return "Inativo";
        case STATUS_PENDENTE: return "Pendente";
        default:              return "Desconhecido";
    }
}

const char *utils_objetivo_str(ObjetivoTreino o) {
    switch (o) {
        case OBJ_HIPERTROFIA:     return "Hipertrofia";
        case OBJ_EMAGRECIMENTO:   return "Emagrecimento";
        case OBJ_CONDICIONAMENTO: return "Condicionamento";
        case OBJ_REABILITACAO:    return "Reabilitacao";
        default:                  return "Nao definido";
    }
}

const char *utils_sexo_str(Sexo s) {
    switch (s) {
        case SEXO_MASCULINO: return "Masculino";
        case SEXO_FEMININO:  return "Feminino";
        default:             return "Outro";
    }
}
