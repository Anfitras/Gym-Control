/* ================================================================
   GYMCONTROL — aluno_turma.c
   Gerenciamento da relação many-to-many entre alunos e turmas
   ================================================================ */
#include "gymcontrol.h"

/* ----------------------------------------------------------------
   aluno_turma_vincular: Acrescenta um vínculo aluno ↔ turma
   ---------------------------------------------------------------- */
int aluno_turma_vincular(int id_aluno, int id_turma)
{
    if (id_aluno <= 0 || id_turma <= 0)
        return GC_ERRO;

    utils_criar_dirs();

    /* Verifica se já existe vínculo */
    FILE *fp = fopen(ARQ_ALUNO_TURMA, "rb");
    if (fp) {
        AlunoTurma at;
        while (fread(&at, sizeof(AlunoTurma), 1, fp) == 1) {
            if (at.id_aluno == id_aluno && at.id_turma == id_turma) {
                fclose(fp);
                return GC_OK; /* já existe */
            }
        }
        fclose(fp);
    }

    /* Cria novo vínculo */
    AlunoTurma novo;
    novo.id = utils_proximo_id(ARQ_ALUNO_TURMA, sizeof(AlunoTurma));
    novo.id_aluno = id_aluno;
    novo.id_turma = id_turma;

    fp = fopen(ARQ_ALUNO_TURMA, "ab");
    if (!fp) return GC_ERRO;
    fwrite(&novo, sizeof(AlunoTurma), 1, fp);
    fclose(fp);
    return GC_OK;
}

/* ----------------------------------------------------------------
   aluno_turma_desvincular: Remove o vínculo aluno ↔ turma
   ---------------------------------------------------------------- */
int aluno_turma_desvincular(int id_aluno, int id_turma)
{
    if (id_aluno <= 0 || id_turma <= 0)
        return GC_ERRO;

    FILE *fp = fopen(ARQ_ALUNO_TURMA, "rb");
    FILE *tmp = fopen("data/aluno_turma_tmp.dat", "wb");
    if (!fp || !tmp) {
        if (fp) fclose(fp);
        if (tmp) fclose(tmp);
        return GC_ERRO;
    }

    AlunoTurma at;
    int found = 0;
    while (fread(&at, sizeof(AlunoTurma), 1, fp) == 1) {
        if (at.id_aluno == id_aluno && at.id_turma == id_turma) {
            found = 1;
        } else {
            fwrite(&at, sizeof(AlunoTurma), 1, tmp);
        }
    }
    fclose(fp);
    fclose(tmp);

    if (!found) {
        remove("data/aluno_turma_tmp.dat");
        return GC_NAO_ENCONTRADO;
    }

    remove(ARQ_ALUNO_TURMA);
    rename("data/aluno_turma_tmp.dat", ARQ_ALUNO_TURMA);
    return GC_OK;
}

/* ----------------------------------------------------------------
   aluno_turma_listar_turmas_json: Lista todas as turmas de um aluno
   Retorna: [{ id: 1, nome: "...", ... }, ...]
   ---------------------------------------------------------------- */
int aluno_turma_listar_turmas_json(int id_aluno, char *buf, int maxlen)
{
    if (id_aluno <= 0) {
        snprintf(buf, maxlen, "[]");
        return 2;
    }

    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");

    FILE *fp_at = fopen(ARQ_ALUNO_TURMA, "rb");
    if (fp_at) {
        AlunoTurma at;
        int primeiro = 1;
        while (fread(&at, sizeof(AlunoTurma), 1, fp_at) == 1) {
            if (at.id_aluno != id_aluno) continue;

            Turma t;
            if (turma_buscar_id(at.id_turma, &t) != GC_OK) continue;

            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");

            int alunos_turma = aluno_turma_contar_de_turma(t.id);
            int vagas = t.capacidade_max - alunos_turma;
            int pct = (t.capacidade_max > 0)
                      ? (alunos_turma * 100 / t.capacidade_max) : 0;

            pos += snprintf(buf + pos, maxlen - pos,
                "{\"id\":%d,\"nome\":\"%s\",\"modalidade\":\"%s\","
                "\"id_professor\":%d,\"horario\":\"%s\","
                "\"dias_semana\":\"%s\",\"capacidade_max\":%d,"
                "\"total_alunos\":%d,\"vagas\":%d,\"ocupacao_pct\":%d,"
                "\"status\":\"%s\"}",
                t.id, t.nome, t.modalidade, t.id_professor,
                t.horario, t.dias_semana, t.capacidade_max,
                alunos_turma, vagas, pct, utils_status_str(t.status));
            primeiro = 0;
        }
        fclose(fp_at);
    }

    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}

/* ----------------------------------------------------------------
   aluno_turma_listar_de_turma_json: Lista todos os alunos de uma turma
   Retorna: [{ id: 1, nome: "...", ... }, ...]
   ---------------------------------------------------------------- */
int aluno_turma_listar_de_turma_json(int id_turma, char *buf, int maxlen)
{
    if (id_turma <= 0) {
        snprintf(buf, maxlen, "[]");
        return 2;
    }

    int pos = 0;
    pos += snprintf(buf + pos, maxlen - pos, "[");

    FILE *fp_at = fopen(ARQ_ALUNO_TURMA, "rb");
    if (fp_at) {
        AlunoTurma at;
        int primeiro = 1;
        while (fread(&at, sizeof(AlunoTurma), 1, fp_at) == 1) {
            if (at.id_turma != id_turma) continue;

            Aluno a;
            if (aluno_buscar_id(at.id_aluno, &a) != GC_OK) continue;

            if (!primeiro)
                pos += snprintf(buf + pos, maxlen - pos, ",");

            char obj[1024];
            /* Adaptado de aluno_to_json, incluindo id_turma=0 (legado) */
            snprintf(obj, sizeof(obj),
                "{\"id\":%d,\"nome\":\"%s\",\"cpf\":\"%s\","
                "\"idade\":%d,\"sexo\":\"%s\",\"telefone\":\"%s\",\"email\":\"%s\","
                "\"id_turma\":0,\"id_professor\":%d,\"status\":\"%s\","
                "\"data_matricula\":\"%s\",\"peso_kg\":%.1f,\"altura_m\":%.2f}",
                a.id, a.nome, a.cpf, a.idade, utils_sexo_str(a.sexo),
                a.telefone, a.email, a.id_professor,
                utils_status_str(a.status), a.data_matricula,
                a.peso_kg, a.altura_m);
            pos += snprintf(buf + pos, maxlen - pos, "%s", obj);
            primeiro = 0;
        }
        fclose(fp_at);
    }

    pos += snprintf(buf + pos, maxlen - pos, "]");
    return pos;
}

/* ----------------------------------------------------------------
   aluno_turma_contar_de_turma: Conta quantos alunos ativos em uma turma
   ---------------------------------------------------------------- */
int aluno_turma_contar_de_turma(int id_turma)
{
    if (id_turma <= 0) return 0;

    int count = 0;
    FILE *fp_at = fopen(ARQ_ALUNO_TURMA, "rb");
    if (fp_at) {
        AlunoTurma at;
        while (fread(&at, sizeof(AlunoTurma), 1, fp_at) == 1) {
            if (at.id_turma != id_turma) continue;

            Aluno a;
            if (aluno_buscar_id(at.id_aluno, &a) == GC_OK && a.status != STATUS_INATIVO) {
                count++;
            }
        }
        fclose(fp_at);
    }
    return count;
}

/* ----------------------------------------------------------------
   aluno_turma_remover_tudo_aluno: Remove todos os vínculos de um aluno
   (quando aluno é deletado)
   ---------------------------------------------------------------- */
int aluno_turma_remover_tudo_aluno(int id_aluno)
{
    if (id_aluno <= 0)
        return GC_ERRO;

    FILE *fp = fopen(ARQ_ALUNO_TURMA, "rb");
    FILE *tmp = fopen("data/aluno_turma_tmp.dat", "wb");
    if (!fp || !tmp) {
        if (fp) fclose(fp);
        if (tmp) fclose(tmp);
        return GC_ERRO;
    }

    AlunoTurma at;
    while (fread(&at, sizeof(AlunoTurma), 1, fp) == 1) {
        if (at.id_aluno != id_aluno) {
            fwrite(&at, sizeof(AlunoTurma), 1, tmp);
        }
    }
    fclose(fp);
    fclose(tmp);

    remove(ARQ_ALUNO_TURMA);
    rename("data/aluno_turma_tmp.dat", ARQ_ALUNO_TURMA);
    return GC_OK;
}
