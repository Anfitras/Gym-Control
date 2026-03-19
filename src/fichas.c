/* ================================================================
   GYMCONTROL — fichas.c
   ================================================================ */
#include "gymcontrol.h"

static int exercicio_to_json(const Exercicio *e, char *buf, int maxlen)
{
    return snprintf(buf, maxlen,
                    "{"
                    "\"numero\":%d,"
                    "\"nome\":\"%s\","
                    "\"grupo_muscular\":\"%s\","
                    "\"series\":%d,"
                    "\"repeticoes\":%d,"
                    "\"carga_kg\":%.1f,"
                    "\"observacao\":\"%s\""
                    "}",
                    e->numero, e->nome, e->grupo_muscular,
                    e->series, e->repeticoes, e->carga_kg, e->observacao);
}

static int ficha_to_json(const FichaTreino *f, char *buf, int maxlen)
{
    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos,
                    "{"
                    "\"id\":%d,"
                    "\"id_aluno\":%d,"
                    "\"id_professor\":%d,"
                    "\"objetivo\":\"%s\","
                    "\"data_criacao\":\"%s\","
                    "\"observacoes\":\"%s\","
                    "\"total_exercicios\":%d,"
                    "\"exercicios\":[",
                    f->id, f->id_aluno, f->id_professor,
                    utils_objetivo_str(f->objetivo),
                    f->data_criacao, f->observacoes,
                    f->total_exercicios);

    int i;
    for (i = 0; i < f->total_exercicios; i++)
    {
        if (i > 0)
            pos += snprintf(buf + pos, maxlen - pos, ",");
        char ej[512];
        exercicio_to_json(&f->exercicios[i], ej, sizeof(ej));
        pos += snprintf(buf + pos, maxlen - pos, "%s", ej);
    }
    pos += snprintf(buf + pos, maxlen - pos, "]}");
    return pos;
}

int ficha_cadastrar(FichaTreino *f)
{
    utils_criar_dirs();
    f->id = utils_proximo_id(ARQ_FICHAS, sizeof(FichaTreino));
    if (strlen(f->data_criacao) == 0)
        utils_data_hoje(f->data_criacao);
    FILE *fp = fopen(ARQ_FICHAS, "ab");
    if (!fp)
        return GC_ERRO;
    fwrite(f, sizeof(FichaTreino), 1, fp);
    fclose(fp);
    return f->id;
}

int ficha_listar_json(char *buf, int maxlen)
{
    FILE *fp = fopen(ARQ_FICHAS, "rb");
    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");
    if (fp)
    {
        FichaTreino f;
        int primeiro = 1;
        while (fread(&f, sizeof(FichaTreino), 1, fp) == 1)
        {
            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");
            char obj[16384];
            ficha_to_json(&f, obj, sizeof(obj));
            pos += snprintf(buf + pos, maxlen - pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }
    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}

int ficha_buscar_id(int id, FichaTreino *dest)
{
    FILE *fp = fopen(ARQ_FICHAS, "rb");
    if (!fp)
        return GC_NAO_ENCONTRADO;
    FichaTreino f;
    while (fread(&f, sizeof(FichaTreino), 1, fp) == 1)
    {
        if (f.id == id)
        {
            *dest = f;
            fclose(fp);
            return GC_OK;
        }
    }
    fclose(fp);
    return GC_NAO_ENCONTRADO;
}

int ficha_buscar_id_json(int id, char *buf, int maxlen)
{
    FichaTreino f;
    if (ficha_buscar_id(id, &f) != GC_OK)
        return GC_NAO_ENCONTRADO;
    ficha_to_json(&f, buf, maxlen);
    return GC_OK;
}

int ficha_buscar_aluno_json(int id_aluno, char *buf, int maxlen)
{
    FILE *fp = fopen(ARQ_FICHAS, "rb");
    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");
    if (fp)
    {
        FichaTreino f;
        int primeiro = 1;
        while (fread(&f, sizeof(FichaTreino), 1, fp) == 1)
        {
            if (f.id_aluno != id_aluno)
                continue;
            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");
            char obj[16384];
            ficha_to_json(&f, obj, sizeof(obj));
            pos += snprintf(buf + pos, maxlen - pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }
    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}

int ficha_editar(int id, FichaTreino *novo)
{
    FILE *fp = fopen(ARQ_FICHAS, "rb");
    FILE *tmp = fopen("data/fichas_tmp.dat", "wb");
    if (!fp || !tmp)
    {
        if (fp)
            fclose(fp);
        if (tmp)
            fclose(tmp);
        return GC_ERRO;
    }
    FichaTreino f;
    int found = 0;
    while (fread(&f, sizeof(FichaTreino), 1, fp) == 1)
    {
        if (f.id == id)
        {
            novo->id = id;
            fwrite(novo, sizeof(FichaTreino), 1, tmp);
            found = 1;
        }
        else
            fwrite(&f, sizeof(FichaTreino), 1, tmp);
    }
    fclose(fp);
    fclose(tmp);
    if (!found)
    {
        remove("data/fichas_tmp.dat");
        return GC_NAO_ENCONTRADO;
    }
    remove(ARQ_FICHAS);
    rename("data/fichas_tmp.dat", ARQ_FICHAS);
    return GC_OK;
}

int ficha_excluir(int id)
{
    FILE *fp = fopen(ARQ_FICHAS, "rb");
    FILE *tmp = fopen("data/fichas_tmp.dat", "wb");
    if (!fp || !tmp)
    {
        if (fp)
            fclose(fp);
        if (tmp)
            fclose(tmp);
        return GC_ERRO;
    }
    FichaTreino f;
    int found = 0;
    while (fread(&f, sizeof(FichaTreino), 1, fp) == 1)
    {
        if (f.id == id)
            found = 1;
        else
            fwrite(&f, sizeof(FichaTreino), 1, tmp);
    }
    fclose(fp);
    fclose(tmp);
    if (!found)
    {
        remove("data/fichas_tmp.dat");
        return GC_NAO_ENCONTRADO;
    }
    remove(ARQ_FICHAS);
    rename("data/fichas_tmp.dat", ARQ_FICHAS);
    return GC_OK;
}

/* ----------------------------------------------------------------
   ficha_adicionar_exercicio
   Carrega a ficha, insere o exercício no array e regrava.
   ---------------------------------------------------------------- */
int ficha_adicionar_exercicio(int id_ficha, Exercicio *ex)
{
    FichaTreino f;
    if (ficha_buscar_id(id_ficha, &f) != GC_OK)
        return GC_NAO_ENCONTRADO;
    if (f.total_exercicios >= MAX_EXERCICIOS)
        return GC_CHEIO;

    ex->numero = f.total_exercicios + 1;
    f.exercicios[f.total_exercicios] = *ex;
    f.total_exercicios++;

    return ficha_editar(id_ficha, &f);
}
