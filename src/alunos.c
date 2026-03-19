/* ================================================================
   GYMCONTROL — alunos.c
   CRUD de alunos: persistência em .dat  +  saída em JSON
   ================================================================ */
#include "gymcontrol.h"

/* ----------------------------------------------------------------
   Serializa um Aluno para JSON (objeto) e acrescenta em buf
   ---------------------------------------------------------------- */
static int aluno_to_json(const Aluno *a, char *buf, int maxlen)
{
    return snprintf(buf, maxlen,
                    "{"
                    "\"id\":%d,"
                    "\"nome\":\"%s\","
                    "\"cpf\":\"%s\","
                    "\"idade\":%d,"
                    "\"sexo\":\"%s\","
                    "\"telefone\":\"%s\","
                    "\"email\":\"%s\","
                    "\"id_turma\":%d,"
                    "\"id_professor\":%d,"
                    "\"status\":\"%s\","
                    "\"data_matricula\":\"%s\","
                    "\"peso_kg\":%.1f,"
                    "\"altura_m\":%.2f"
                    "}",
                    a->id, a->nome, a->cpf, a->idade,
                    utils_sexo_str(a->sexo),
                    a->telefone, a->email,
                    a->id_turma, a->id_professor,
                    utils_status_str(a->status),
                    a->data_matricula,
                    a->peso_kg, a->altura_m);
}

/* ----------------------------------------------------------------
   aluno_cadastrar
   ---------------------------------------------------------------- */
int aluno_cadastrar(Aluno *a)
{
    utils_criar_dirs();
    a->id = utils_proximo_id(ARQ_ALUNOS, sizeof(Aluno));
    if (strlen(a->data_matricula) == 0)
        utils_data_hoje(a->data_matricula);

    FILE *fp = fopen(ARQ_ALUNOS, "ab");
    if (!fp)
        return GC_ERRO;
    fwrite(a, sizeof(Aluno), 1, fp);
    fclose(fp);
    return a->id; /* retorna o id gerado */
}

/* ----------------------------------------------------------------
   aluno_listar_json  →  "[{...},{...}]"
   ---------------------------------------------------------------- */
int aluno_listar_json(char *buf, int maxlen)
{
    FILE *fp = fopen(ARQ_ALUNOS, "rb");
    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");

    if (fp)
    {
        Aluno a;
        int primeiro = 1;
        while (fread(&a, sizeof(Aluno), 1, fp) == 1)
        {
            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");
            char obj[1024];
            aluno_to_json(&a, obj, sizeof(obj));
            pos += snprintf(buf + pos, maxlen - pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }

    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}

/* ----------------------------------------------------------------
   aluno_buscar_id
   ---------------------------------------------------------------- */
int aluno_buscar_id(int id, Aluno *dest)
{
    FILE *fp = fopen(ARQ_ALUNOS, "rb");
    if (!fp)
        return GC_NAO_ENCONTRADO;
    Aluno a;
    while (fread(&a, sizeof(Aluno), 1, fp) == 1)
    {
        if (a.id == id)
        {
            *dest = a;
            fclose(fp);
            return GC_OK;
        }
    }
    fclose(fp);
    return GC_NAO_ENCONTRADO;
}

/* ----------------------------------------------------------------
   aluno_editar  (rewrite com arquivo temporário)
   ---------------------------------------------------------------- */
int aluno_editar(int id, Aluno *novo)
{
    FILE *fp = fopen(ARQ_ALUNOS, "rb");
    FILE *tmp = fopen("data/alunos_tmp.dat", "wb");
    if (!fp || !tmp)
    {
        if (fp)
            fclose(fp);
        if (tmp)
            fclose(tmp);
        return GC_ERRO;
    }

    Aluno a;
    int found = 0;
    while (fread(&a, sizeof(Aluno), 1, fp) == 1)
    {
        if (a.id == id)
        {
            novo->id = id;
            fwrite(novo, sizeof(Aluno), 1, tmp);
            found = 1;
        }
        else
            fwrite(&a, sizeof(Aluno), 1, tmp);
    }
    fclose(fp);
    fclose(tmp);
    if (!found)
    {
        remove("data/alunos_tmp.dat");
        return GC_NAO_ENCONTRADO;
    }
    remove(ARQ_ALUNOS);
    rename("data/alunos_tmp.dat", ARQ_ALUNOS);
    return GC_OK;
}

/* ----------------------------------------------------------------
   aluno_excluir
   ---------------------------------------------------------------- */
int aluno_excluir(int id)
{
    FILE *fp = fopen(ARQ_ALUNOS, "rb");
    FILE *tmp = fopen("data/alunos_tmp.dat", "wb");
    if (!fp || !tmp)
    {
        if (fp)
            fclose(fp);
        if (tmp)
            fclose(tmp);
        return GC_ERRO;
    }

    Aluno a;
    int found = 0;
    while (fread(&a, sizeof(Aluno), 1, fp) == 1)
    {
        if (a.id == id)
            found = 1;
        else
            fwrite(&a, sizeof(Aluno), 1, tmp);
    }
    fclose(fp);
    fclose(tmp);
    if (!found)
    {
        remove("data/alunos_tmp.dat");
        return GC_NAO_ENCONTRADO;
    }
    remove(ARQ_ALUNOS);
    rename("data/alunos_tmp.dat", ARQ_ALUNOS);
    return GC_OK;
}

/* ----------------------------------------------------------------
   aluno_total
   ---------------------------------------------------------------- */
int aluno_total(void)
{
    FILE *fp = fopen(ARQ_ALUNOS, "rb");
    if (!fp)
        return 0;
    fseek(fp, 0, SEEK_END);
    long t = ftell(fp);
    fclose(fp);
    return (int)(t / (long)sizeof(Aluno));
}

/* ----------------------------------------------------------------
   aluno_listar_turma_json  →  filtra por id_turma
   ---------------------------------------------------------------- */
int aluno_listar_turma_json(int id_turma, char *buf, int maxlen)
{
    FILE *fp = fopen(ARQ_ALUNOS, "rb");
    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");
    if (fp)
    {
        Aluno a;
        int primeiro = 1;
        while (fread(&a, sizeof(Aluno), 1, fp) == 1)
        {
            if (a.id_turma != id_turma)
                continue;
            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");
            char obj[1024];
            aluno_to_json(&a, obj, sizeof(obj));
            pos += snprintf(buf + pos, maxlen - pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }
    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}
