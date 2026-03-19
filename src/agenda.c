/* ================================================================
   GYMCONTROL — agenda.c
   CRUD completo da agenda com persistência em data/agenda.dat
   ================================================================ */
#include "gymcontrol.h"

/* ----------------------------------------------------------------
   Serializa Agenda para JSON
   ---------------------------------------------------------------- */
static int agenda_to_json(const Agenda *ag, char *buf, int maxlen)
{
    return snprintf(buf, maxlen,
                    "{"
                    "\"id\":%d,"
                    "\"id_professor\":%d,"
                    "\"id_turma\":%d,"
                    "\"data\":\"%s\","
                    "\"hora_inicio\":\"%s\","
                    "\"hora_fim\":\"%s\","
                    "\"atividade\":\"%s\","
                    "\"observacao\":\"%s\","
                    "\"realizada\":%d"
                    "}",
                    ag->id,
                    ag->id_professor,
                    ag->id_turma,
                    ag->data,
                    ag->hora_inicio,
                    ag->hora_fim,
                    ag->atividade,
                    ag->observacao,
                    ag->realizada);
}

/* ----------------------------------------------------------------
   agenda_cadastrar
   ---------------------------------------------------------------- */
int agenda_cadastrar(Agenda *ag)
{
    utils_criar_dirs();
    ag->id = utils_proximo_id(ARQ_AGENDA, sizeof(Agenda));

    FILE *fp = fopen(ARQ_AGENDA, "ab");
    if (!fp)
        return GC_ERRO;
    fwrite(ag, sizeof(Agenda), 1, fp);
    fclose(fp);
    return ag->id;
}

/* ----------------------------------------------------------------
   agenda_listar_json
   ---------------------------------------------------------------- */
int agenda_listar_json(char *buf, int maxlen)
{
    FILE *fp = fopen(ARQ_AGENDA, "rb");
    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");

    if (fp)
    {
        Agenda ag;
        int primeiro = 1;
        while (fread(&ag, sizeof(Agenda), 1, fp) == 1)
        {
            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");
            char obj[1024];
            agenda_to_json(&ag, obj, sizeof(obj));
            pos += snprintf(buf + pos, maxlen - pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }

    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}

/* ----------------------------------------------------------------
   agenda_listar_data_json
   ---------------------------------------------------------------- */
int agenda_listar_data_json(const char *data, char *buf, int maxlen)
{
    FILE *fp = fopen(ARQ_AGENDA, "rb");
    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");

    if (fp)
    {
        Agenda ag;
        int primeiro = 1;
        while (fread(&ag, sizeof(Agenda), 1, fp) == 1)
        {
            if (strcmp(ag.data, data) != 0)
                continue;
            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");
            char obj[1024];
            agenda_to_json(&ag, obj, sizeof(obj));
            pos += snprintf(buf + pos, maxlen - pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }

    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}

/* ----------------------------------------------------------------
   agenda_listar_professor_json
   ---------------------------------------------------------------- */
int agenda_listar_professor_json(int id_prof, char *buf, int maxlen)
{
    FILE *fp = fopen(ARQ_AGENDA, "rb");
    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");

    if (fp)
    {
        Agenda ag;
        int primeiro = 1;
        while (fread(&ag, sizeof(Agenda), 1, fp) == 1)
        {
            if (ag.id_professor != id_prof)
                continue;
            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");
            char obj[1024];
            agenda_to_json(&ag, obj, sizeof(obj));
            pos += snprintf(buf + pos, maxlen - pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp);
    }

    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}

/* ----------------------------------------------------------------
   agenda_buscar_id
   ---------------------------------------------------------------- */
int agenda_buscar_id(int id, Agenda *dest)
{
    FILE *fp = fopen(ARQ_AGENDA, "rb");
    if (!fp)
        return GC_NAO_ENCONTRADO;

    Agenda ag;
    while (fread(&ag, sizeof(Agenda), 1, fp) == 1)
    {
        if (ag.id == id)
        {
            *dest = ag;
            fclose(fp);
            return GC_OK;
        }
    }

    fclose(fp);
    return GC_NAO_ENCONTRADO;
}

/* ----------------------------------------------------------------
   agenda_editar
   ---------------------------------------------------------------- */
int agenda_editar(int id, Agenda *novo)
{
    FILE *fp = fopen(ARQ_AGENDA, "rb");
    FILE *tmp = fopen("data/agenda_tmp.dat", "wb");

    if (!fp || !tmp)
    {
        if (fp)
            fclose(fp);
        if (tmp)
            fclose(tmp);
        return GC_ERRO;
    }

    Agenda ag;
    int found = 0;

    while (fread(&ag, sizeof(Agenda), 1, fp) == 1)
    {
        if (ag.id == id)
        {
            novo->id = id;
            fwrite(novo, sizeof(Agenda), 1, tmp);
            found = 1;
        }
        else
        {
            fwrite(&ag, sizeof(Agenda), 1, tmp);
        }
    }

    fclose(fp);
    fclose(tmp);

    if (!found)
    {
        remove("data/agenda_tmp.dat");
        return GC_NAO_ENCONTRADO;
    }

    remove(ARQ_AGENDA);
    rename("data/agenda_tmp.dat", ARQ_AGENDA);
    return GC_OK;
}

/* ----------------------------------------------------------------
   agenda_excluir
   ---------------------------------------------------------------- */
int agenda_excluir(int id)
{
    FILE *fp = fopen(ARQ_AGENDA, "rb");
    FILE *tmp = fopen("data/agenda_tmp.dat", "wb");

    if (!fp || !tmp)
    {
        if (fp)
            fclose(fp);
        if (tmp)
            fclose(tmp);
        return GC_ERRO;
    }

    Agenda ag;
    int found = 0;

    while (fread(&ag, sizeof(Agenda), 1, fp) == 1)
    {
        if (ag.id == id)
            found = 1;
        else
            fwrite(&ag, sizeof(Agenda), 1, tmp);
    }

    fclose(fp);
    fclose(tmp);

    if (!found)
    {
        remove("data/agenda_tmp.dat");
        return GC_NAO_ENCONTRADO;
    }

    remove(ARQ_AGENDA);
    rename("data/agenda_tmp.dat", ARQ_AGENDA);
    return GC_OK;
}

/* ----------------------------------------------------------------
   agenda_marcar_realizada
   ---------------------------------------------------------------- */
int agenda_marcar_realizada(int id)
{
    Agenda ag;
    if (agenda_buscar_id(id, &ag) != GC_OK)
        return GC_NAO_ENCONTRADO;
    ag.realizada = 1;
    return agenda_editar(id, &ag);
}