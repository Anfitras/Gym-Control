/* ================================================================
   GYMCONTROL — professores.c
   ================================================================ */
#include "gymcontrol.h"

static int professor_to_json(const Professor *p, char *buf, int maxlen) {
    return snprintf(buf, maxlen,
        "{"
        "\"id\":%d,"
        "\"nome\":\"%s\","
        "\"cpf\":\"%s\","
        "\"telefone\":\"%s\","
        "\"email\":\"%s\","
        "\"especialidade\":\"%s\","
        "\"horario_disponivel\":\"%s\","
        "\"status\":\"%s\""
        "}",
        p->id, p->nome, p->cpf, p->telefone, p->email,
        p->especialidade, p->horario_disponivel,
        utils_status_str(p->status));
}

int professor_cadastrar(Professor *p) {
    utils_criar_dirs();
    p->id = utils_proximo_id(ARQ_PROFESSORES, sizeof(Professor));
    FILE *fp = fopen(ARQ_PROFESSORES, "ab");
    if (!fp) return GC_ERRO;
    fwrite(p, sizeof(Professor), 1, fp);
    fclose(fp);
    return p->id;
}

int professor_listar_json(char *buf, int maxlen) {
    FILE *fp = fopen(ARQ_PROFESSORES, "rb");
    int pos = 0;
    pos += snprintf(buf+pos, maxlen-pos, "[");
    if (fp) {
        Professor p; int primeiro = 1;
        while (fread(&p, sizeof(Professor), 1, fp) == 1) {
            if (!primeiro) pos += snprintf(buf+pos, maxlen-pos, ",");
            char obj[512];
            professor_to_json(&p, obj, sizeof(obj));
            pos += snprintf(buf+pos, maxlen-pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }
    pos += snprintf(buf+pos, maxlen-pos, "]");
    return pos;
}

int professor_buscar_id(int id, Professor *dest) {
    FILE *fp = fopen(ARQ_PROFESSORES, "rb");
    if (!fp) return GC_NAO_ENCONTRADO;
    Professor p;
    while (fread(&p, sizeof(Professor), 1, fp) == 1) {
        if (p.id == id) { *dest = p; fclose(fp); return GC_OK; }
    }
    fclose(fp);
    return GC_NAO_ENCONTRADO;
}

int professor_editar(int id, Professor *novo) {
    FILE *fp  = fopen(ARQ_PROFESSORES, "rb");
    FILE *tmp = fopen("data/professores_tmp.dat", "wb");
    if (!fp || !tmp) { if(fp)fclose(fp); if(tmp)fclose(tmp); return GC_ERRO; }
    Professor p; int found = 0;
    while (fread(&p, sizeof(Professor), 1, fp) == 1) {
        if (p.id == id) { novo->id=id; fwrite(novo,sizeof(Professor),1,tmp); found=1; }
        else fwrite(&p, sizeof(Professor), 1, tmp);
    }
    fclose(fp); fclose(tmp);
    if (!found) { remove("data/professores_tmp.dat"); return GC_NAO_ENCONTRADO; }
    remove(ARQ_PROFESSORES);
    rename("data/professores_tmp.dat", ARQ_PROFESSORES);
    return GC_OK;
}

int professor_excluir(int id) {
    FILE *fp  = fopen(ARQ_PROFESSORES, "rb");
    FILE *tmp = fopen("data/professores_tmp.dat", "wb");
    if (!fp || !tmp) { if(fp)fclose(fp); if(tmp)fclose(tmp); return GC_ERRO; }
    Professor p; int found = 0;
    while (fread(&p, sizeof(Professor), 1, fp) == 1) {
        if (p.id == id) found = 1;
        else fwrite(&p, sizeof(Professor), 1, tmp);
    }
    fclose(fp); fclose(tmp);
    if (!found) { remove("data/professores_tmp.dat"); return GC_NAO_ENCONTRADO; }
    remove(ARQ_PROFESSORES);
    rename("data/professores_tmp.dat", ARQ_PROFESSORES);
    return GC_OK;
}

int professor_total(void) {
    FILE *fp = fopen(ARQ_PROFESSORES, "rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    long t = ftell(fp); fclose(fp);
    return (int)(t / (long)sizeof(Professor));
}
