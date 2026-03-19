/* ================================================================
   GYMCONTROL — turmas.c
   ================================================================ */
#include "gymcontrol.h"

/* Calcula alunos ativos nessa turma cruzando com alunos.dat */
static int contar_alunos_turma(int id_turma) {
    FILE *fp = fopen(ARQ_ALUNOS, "rb");
    if (!fp) return 0;
    /* importa struct Aluno inline lendo só os campos necessários */
    /* Lemos o registro completo da struct real */
    typedef struct {
        int            id;
        char           nome[100];
        char           cpf[20];
        int            idade;
        int            sexo;
        char           telefone[20];
        char           email[100];
        int            id_turma;
        int            id_professor;
        int            status;
        char           data_matricula[12];
        float          peso_kg;
        float          altura_m;
    } AlunoFull;

    AlunoFull a; int count = 0;
    while (fread(&a, sizeof(AlunoFull), 1, fp) == 1)
        if (a.id_turma == id_turma && a.status != 0) count++;
    fclose(fp);
    return count;
}

int turma_vagas_disponiveis(int id_turma) {
    Turma t;
    if (turma_buscar_id(id_turma, &t) != GC_OK) return GC_NAO_ENCONTRADO;
    return t.capacidade_max - contar_alunos_turma(id_turma);
}

int turma_ocupacao_percent(int id_turma) {
    Turma t;
    if (turma_buscar_id(id_turma, &t) != GC_OK) return 0;
    if (t.capacidade_max == 0) return 0;
    return (contar_alunos_turma(id_turma) * 100) / t.capacidade_max;
}

static int turma_to_json(const Turma *t, char *buf, int maxlen) {
    int alunos = contar_alunos_turma(t->id);
    int vagas  = t->capacidade_max - alunos;
    int pct    = (t->capacidade_max > 0)
                 ? (alunos * 100 / t->capacidade_max) : 0;
    return snprintf(buf, maxlen,
        "{"
        "\"id\":%d,"
        "\"nome\":\"%s\","
        "\"modalidade\":\"%s\","
        "\"id_professor\":%d,"
        "\"horario\":\"%s\","
        "\"dias_semana\":\"%s\","
        "\"capacidade_max\":%d,"
        "\"total_alunos\":%d,"
        "\"vagas\":%d,"
        "\"ocupacao_pct\":%d,"
        "\"status\":\"%s\""
        "}",
        t->id, t->nome, t->modalidade, t->id_professor,
        t->horario, t->dias_semana, t->capacidade_max,
        alunos, vagas, pct,
        utils_status_str(t->status));
}

int turma_cadastrar(Turma *t) {
    utils_criar_dirs();
    t->id = utils_proximo_id(ARQ_TURMAS, sizeof(Turma));
    FILE *fp = fopen(ARQ_TURMAS, "ab");
    if (!fp) return GC_ERRO;
    fwrite(t, sizeof(Turma), 1, fp);
    fclose(fp);
    return t->id;
}

int turma_listar_json(char *buf, int maxlen) {
    FILE *fp = fopen(ARQ_TURMAS, "rb");
    int pos = 0;
    pos += snprintf(buf+pos, maxlen-pos, "[");
    if (fp) {
        Turma t; int primeiro = 1;
        while (fread(&t, sizeof(Turma), 1, fp) == 1) {
            if (!primeiro) pos += snprintf(buf+pos, maxlen-pos, ",");
            char obj[512];
            turma_to_json(&t, obj, sizeof(obj));
            pos += snprintf(buf+pos, maxlen-pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }
    pos += snprintf(buf+pos, maxlen-pos, "]");
    return pos;
}

int turma_buscar_id(int id, Turma *dest) {
    FILE *fp = fopen(ARQ_TURMAS, "rb");
    if (!fp) return GC_NAO_ENCONTRADO;
    Turma t;
    while (fread(&t, sizeof(Turma), 1, fp) == 1) {
        if (t.id == id) { *dest = t; fclose(fp); return GC_OK; }
    }
    fclose(fp); return GC_NAO_ENCONTRADO;
}

int turma_editar(int id, Turma *novo) {
    FILE *fp  = fopen(ARQ_TURMAS, "rb");
    FILE *tmp = fopen("data/turmas_tmp.dat", "wb");
    if (!fp || !tmp) { if(fp)fclose(fp); if(tmp)fclose(tmp); return GC_ERRO; }
    Turma t; int found = 0;
    while (fread(&t, sizeof(Turma), 1, fp) == 1) {
        if (t.id == id) { novo->id=id; fwrite(novo,sizeof(Turma),1,tmp); found=1; }
        else fwrite(&t, sizeof(Turma), 1, tmp);
    }
    fclose(fp); fclose(tmp);
    if (!found) { remove("data/turmas_tmp.dat"); return GC_NAO_ENCONTRADO; }
    remove(ARQ_TURMAS); rename("data/turmas_tmp.dat", ARQ_TURMAS);
    return GC_OK;
}

int turma_excluir(int id) {
    FILE *fp  = fopen(ARQ_TURMAS, "rb");
    FILE *tmp = fopen("data/turmas_tmp.dat", "wb");
    if (!fp || !tmp) { if(fp)fclose(fp); if(tmp)fclose(tmp); return GC_ERRO; }
    Turma t; int found = 0;
    while (fread(&t, sizeof(Turma), 1, fp) == 1) {
        if (t.id == id) found = 1;
        else fwrite(&t, sizeof(Turma), 1, tmp);
    }
    fclose(fp); fclose(tmp);
    if (!found) { remove("data/turmas_tmp.dat"); return GC_NAO_ENCONTRADO; }
    remove(ARQ_TURMAS); rename("data/turmas_tmp.dat", ARQ_TURMAS);
    return GC_OK;
}

int turma_total(void) {
    FILE *fp = fopen(ARQ_TURMAS, "rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END); long t = ftell(fp); fclose(fp);
    return (int)(t / (long)sizeof(Turma));
}
