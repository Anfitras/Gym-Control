/* ================================================================
   GYMCONTROL — routes.c
   Roteador HTTP: recebe method + path + body,
   chama a função C correta e preenche response.

   Rotas disponíveis:
   GET  /api/alunos              → lista todos
   GET  /api/alunos?id=N         → busca por id
   GET  /api/alunos?turma=N      → lista por turma
   POST /api/alunos              → cadastrar
   PUT  /api/alunos?id=N         → editar
   DELETE /api/alunos?id=N       → excluir

   GET  /api/professores         → lista todos
   POST /api/professores         → cadastrar
   PUT  /api/professores?id=N    → editar
   DELETE /api/professores?id=N  → excluir

   GET  /api/turmas              → lista todas
   GET  /api/turmas?id=N         → vagas/ocupação de uma turma
   POST /api/turmas              → cadastrar
   PUT  /api/turmas?id=N         → editar
   DELETE /api/turmas?id=N       → excluir

   GET  /api/fichas              → lista todas
   GET  /api/fichas?aluno=N      → por aluno
   POST /api/fichas              → cadastrar
   PUT  /api/fichas?id=N         → editar
   DELETE /api/fichas?id=N       → excluir

   GET  /api/agenda              → lista todas
   GET  /api/agenda?data=DD/MM/AAAA
   GET  /api/agenda?professor=N
   POST /api/agenda              → cadastrar
   PUT  /api/agenda?id=N         → editar / marcar realizada
   DELETE /api/agenda?id=N       → excluir

   GET  /api/dashboard           → resumo geral
   ================================================================ */
#include "gymcontrol.h"

/* ----------------------------------------------------------------
   Helpers internos
   ---------------------------------------------------------------- */

/* Monta resposta HTTP completa */
static void http_ok(char *resp, const char *json) {
    sprintf(resp,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", json);
}

static void http_created(char *resp, const char *json) {
    sprintf(resp,
        "HTTP/1.1 201 Created\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", json);
}

static void http_error(char *resp, int code, const char *msg) {
    char body[256];
    snprintf(body, sizeof(body), "{\"erro\":\"%s\"}", msg);
    sprintf(resp,
        "HTTP/1.1 %d Error\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", code, body);
}

static void http_options(char *resp) {
    sprintf(resp,
        "HTTP/1.1 204 No Content\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n"
        "\r\n");
}

/* Extrai valor de query string: ?key=VALUE */
static int qs_get(const char *path, const char *key, char *dest, int maxlen) {
    char search[64];
    snprintf(search, sizeof(search), "%s=", key);
    const char *p = strstr(path, search);
    if (!p) return 0;
    p += strlen(search);
    int i = 0;
    while (*p && *p != '&' && *p != ' ' && i < maxlen-1)
        dest[i++] = *p++;
    dest[i] = '\0';
    return i > 0;
}

/* Retorna 1 se path começa com prefix */
static int path_starts(const char *path, const char *prefix) {
    return strncmp(path, prefix, strlen(prefix)) == 0;
}

/* ----------------------------------------------------------------
   Deserializa JSON do body para cada struct
   ---------------------------------------------------------------- */
static void json_to_aluno(const char *body, Aluno *a) {
    memset(a, 0, sizeof(Aluno));
    json_get_string(body, "nome",           a->nome,           MAX_NOME);
    json_get_string(body, "cpf",            a->cpf,            MAX_CPF);
    json_get_string(body, "telefone",       a->telefone,       MAX_TEL);
    json_get_string(body, "email",          a->email,          MAX_EMAIL);
    json_get_string(body, "data_matricula", a->data_matricula, 12);
    a->idade        = json_get_int(body,   "idade");
    a->sexo         = (Sexo)json_get_int(body, "sexo");
    a->id_turma     = json_get_int(body,   "id_turma");
    a->id_professor = json_get_int(body,   "id_professor");
    a->status       = (StatusCadastro)json_get_int(body, "status");
    a->peso_kg      = json_get_float(body, "peso_kg");
    a->altura_m     = json_get_float(body, "altura_m");
    if (a->sexo < 1 || a->sexo > 3) a->sexo = SEXO_OUTRO;
}

static void json_to_professor(const char *body, Professor *p) {
    memset(p, 0, sizeof(Professor));
    json_get_string(body, "nome",               p->nome,               MAX_NOME);
    json_get_string(body, "cpf",                p->cpf,                MAX_CPF);
    json_get_string(body, "telefone",           p->telefone,           MAX_TEL);
    json_get_string(body, "email",              p->email,              MAX_EMAIL);
    json_get_string(body, "especialidade",      p->especialidade,      MAX_ESPECIALIDADE);
    json_get_string(body, "horario_disponivel", p->horario_disponivel, MAX_HORARIO);
    p->status = (StatusCadastro)json_get_int(body, "status");
}

static void json_to_turma(const char *body, Turma *t) {
    memset(t, 0, sizeof(Turma));
    json_get_string(body, "nome",        t->nome,        MAX_NOME);
    json_get_string(body, "modalidade",  t->modalidade,  MAX_MODALIDADE);
    json_get_string(body, "horario",     t->horario,     MAX_HORARIO);
    json_get_string(body, "dias_semana", t->dias_semana, MAX_DIAS);
    t->id_professor  = json_get_int(body, "id_professor");
    t->capacidade_max= json_get_int(body, "capacidade_max");
    t->status        = (StatusCadastro)json_get_int(body, "status");
}

static void json_to_ficha(const char *body, FichaTreino *f) {
    memset(f, 0, sizeof(FichaTreino));
    json_get_string(body, "data_criacao", f->data_criacao, 12);
    json_get_string(body, "observacoes",  f->observacoes,  MAX_OBS);
    f->id_aluno     = json_get_int(body, "id_aluno");
    f->id_professor = json_get_int(body, "id_professor");
    f->objetivo     = (ObjetivoTreino)json_get_int(body, "objetivo");
    if (f->objetivo < 1 || f->objetivo > 4) f->objetivo = OBJ_HIPERTROFIA;
}

static void json_to_agenda(const char *body, Agenda *ag) {
    memset(ag, 0, sizeof(Agenda));
    json_get_string(body, "data",        ag->data,        12);
    json_get_string(body, "hora_inicio", ag->hora_inicio, 8);
    json_get_string(body, "hora_fim",    ag->hora_fim,    8);
    json_get_string(body, "atividade",   ag->atividade,   MAX_NOME);
    json_get_string(body, "observacao",  ag->observacao,  MAX_OBS);
    ag->id_professor = json_get_int(body, "id_professor");
    ag->id_turma     = json_get_int(body, "id_turma");
    ag->realizada    = json_get_int(body, "realizada");
}
/* ----------------------------------------------------------------
   route_handle  — ponto de entrada principal do roteador
   ---------------------------------------------------------------- */
void route_handle(const char *method, const char *path,
                  const char *body,   char *response) {

    static char buf[65536];   /* buffer para JSON de resposta */

    /* --- OPTIONS (preflight CORS) --- */
    if (strcmp(method, "OPTIONS") == 0) {
        http_options(response);
        return;
    }

    /* =========================================================
       /api/alunos
       ========================================================= */
    if (path_starts(path, "/api/alunos")) {

        char sid[16] = "", sturma[16] = "";
        qs_get(path, "id",    sid,    sizeof(sid));
        qs_get(path, "turma", sturma, sizeof(sturma));

        if (strcmp(method, "GET") == 0) {
            if (strlen(sturma) > 0) {
                aluno_listar_turma_json(atoi(sturma), buf, sizeof(buf));
            } else if (strlen(sid) > 0) {
                Aluno a;
                if (aluno_buscar_id(atoi(sid), &a) == GC_OK) {
                    snprintf(buf, sizeof(buf),
                        "{\"id\":%d,\"nome\":\"%s\",\"cpf\":\"%s\","
                        "\"idade\":%d,\"sexo\":%d,\"telefone\":\"%s\",\"email\":\"%s\","
                        "\"id_turma\":%d,\"id_professor\":%d,"
                        "\"status\":\"%s\",\"data_matricula\":\"%s\","
                        "\"peso_kg\":%.1f,\"altura_m\":%.2f}",
                        a.id, a.nome, a.cpf, a.idade, a.sexo,
                        a.telefone, a.email,
                        a.id_turma, a.id_professor,
                        utils_status_str(a.status),
                        a.data_matricula, a.peso_kg, a.altura_m);
                } else {
                    http_error(response, 404, "Aluno nao encontrado");
                    return;
                }
            } else {
                aluno_listar_json(buf, sizeof(buf));
            }
            http_ok(response, buf);

        } else if (strcmp(method, "POST") == 0) {
            Aluno a;
            json_to_aluno(body, &a);
            int id = aluno_cadastrar(&a);
            if (id > 0) {
                snprintf(buf, sizeof(buf),
                         "{\"ok\":true,\"id\":%d}", id);
                http_created(response, buf);
            } else {
                http_error(response, 500, "Erro ao cadastrar aluno");
            }

        } else if (strcmp(method, "PUT") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            Aluno a;
            json_to_aluno(body, &a);
            int r = aluno_editar(atoi(sid), &a);
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);

        } else if (strcmp(method, "DELETE") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            int r = aluno_excluir(atoi(sid));
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);
        }
        return;
    }

    /* =========================================================
       /api/professores
       ========================================================= */
    if (path_starts(path, "/api/professores")) {
        char sid[16] = "";
        qs_get(path, "id", sid, sizeof(sid));

        if (strcmp(method, "GET") == 0) {
            professor_listar_json(buf, sizeof(buf));
            http_ok(response, buf);

        } else if (strcmp(method, "POST") == 0) {
            Professor p;
            json_to_professor(body, &p);
            int id = professor_cadastrar(&p);
            snprintf(buf, sizeof(buf), "{\"ok\":true,\"id\":%d}", id);
            http_created(response, buf);

        } else if (strcmp(method, "PUT") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            Professor p;
            json_to_professor(body, &p);
            int r = professor_editar(atoi(sid), &p);
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);

        } else if (strcmp(method, "DELETE") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            int r = professor_excluir(atoi(sid));
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);
        }
        return;
    }

    /* =========================================================
       /api/turmas
       ========================================================= */
    if (path_starts(path, "/api/turmas")) {
        char sid[16] = "";
        qs_get(path, "id", sid, sizeof(sid));

        if (strcmp(method, "GET") == 0) {
            if (strlen(sid) > 0) {
                int id    = atoi(sid);
                int vagas = turma_vagas_disponiveis(id);
                int pct   = turma_ocupacao_percent(id);
                Turma t;
                if (turma_buscar_id(id, &t) == GC_OK) {
                    snprintf(buf, sizeof(buf),
                        "{\"id\":%d,\"nome\":\"%s\",\"vagas\":%d,\"ocupacao_pct\":%d}",
                        t.id, t.nome, vagas, pct);
                } else {
                    http_error(response, 404, "Turma nao encontrada");
                    return;
                }
            } else {
                turma_listar_json(buf, sizeof(buf));
            }
            http_ok(response, buf);

        } else if (strcmp(method, "POST") == 0) {
            Turma t;
            json_to_turma(body, &t);
            int id = turma_cadastrar(&t);
            snprintf(buf, sizeof(buf), "{\"ok\":true,\"id\":%d}", id);
            http_created(response, buf);

        } else if (strcmp(method, "PUT") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            Turma t;
            json_to_turma(body, &t);
            int r = turma_editar(atoi(sid), &t);
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);

        } else if (strcmp(method, "DELETE") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            int r = turma_excluir(atoi(sid));
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);
        }
        return;
    }

    /* =========================================================
       /api/fichas
       ========================================================= */
    if (strcmp(path, "/api/fichas") == 0 || path_starts(path, "/api/fichas?")) {
        char sid[16]="", saluno[16]="";
        qs_get(path, "id",    sid,    sizeof(sid));
        qs_get(path, "aluno", saluno, sizeof(saluno));

        if (strcmp(method, "GET") == 0) {
            if (strlen(sid) > 0) {
                if (ficha_buscar_id_json(atoi(sid), buf, sizeof(buf)) != GC_OK) {
                    http_error(response, 404, "Ficha nao encontrada");
                    return;
                }
            } else if (strlen(saluno) > 0)
                ficha_buscar_aluno_json(atoi(saluno), buf, sizeof(buf));
            else
                ficha_listar_json(buf, sizeof(buf));
            http_ok(response, buf);

        } else if (strcmp(method, "POST") == 0) {
            FichaTreino f;
            json_to_ficha(body, &f);
            if (f.id_aluno <= 0 || f.id_professor <= 0) {
                http_error(response, 400, "id_aluno e id_professor obrigatorios");
                return;
            }
            int id = ficha_cadastrar(&f);
            if (id <= 0) {
                http_error(response, 500, "Erro ao cadastrar ficha");
                return;
            }
            snprintf(buf, sizeof(buf), "{\"ok\":true,\"id\":%d}", id);
            http_created(response, buf);

        } else if (strcmp(method, "PUT") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            FichaTreino f;
            json_to_ficha(body, &f);
            int r = ficha_editar(atoi(sid), &f);
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);

        } else if (strcmp(method, "DELETE") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            int r = ficha_excluir(atoi(sid));
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);
        }
        return;
    }

    /* =========================================================
       /api/fichas/N/exercicio  — adiciona exercício a uma ficha
       ========================================================= */
    {
        /* ex: POST /api/fichas/3/exercicio */
        char id_str[16] = "";
        if (strncmp(path, "/api/fichas/", 12) == 0 &&
            strstr(path, "/exercicio") != NULL &&
            strcmp(method, "POST") == 0)
        {
            const char *p = path + 12;
            int i = 0;
            while (*p && *p != '/' && i < 15) id_str[i++] = *p++;
            id_str[i] = '\0';
            int id_ficha = atoi(id_str);
            if (id_ficha <= 0) {
                http_error(response, 400, "id_ficha invalido");
                return;
            }

            Exercicio ex;
            memset(&ex, 0, sizeof(Exercicio));
            json_get_string(body, "nome",           ex.nome,          MAX_NOME);
            json_get_string(body, "grupo_muscular",  ex.grupo_muscular, MAX_NOME);
            json_get_string(body, "observacao",      ex.observacao,    MAX_OBS);
            if (strlen(ex.nome) == 0) {
                http_error(response, 400, "nome do exercicio obrigatorio");
                return;
            }
            ex.series     = json_get_int(body,   "series");
            ex.repeticoes = json_get_int(body,   "repeticoes");
            ex.carga_kg   = json_get_float(body, "carga_kg");
            if (ex.series < 1)     ex.series     = 1;
            if (ex.repeticoes < 1) ex.repeticoes = 1;

            int r = ficha_adicionar_exercicio(id_ficha, &ex);
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);
            return;
        }
    }

    /* =========================================================
       /api/agenda
       ========================================================= */
    if (path_starts(path, "/api/agenda")) {
        char sid[16]="", sdata[16]="", sprof[16]="", srealizada[4]="";
        qs_get(path, "id",        sid,        sizeof(sid));
        qs_get(path, "data",      sdata,      sizeof(sdata));
        qs_get(path, "professor", sprof,      sizeof(sprof));

        if (strcmp(method, "GET") == 0) {
            if (strlen(sdata) > 0)
                agenda_listar_data_json(sdata, buf, sizeof(buf));
            else if (strlen(sprof) > 0)
                agenda_listar_professor_json(atoi(sprof), buf, sizeof(buf));
            else
                agenda_listar_json(buf, sizeof(buf));
            http_ok(response, buf);

        } else if (strcmp(method, "POST") == 0) {
            Agenda ag;
            json_to_agenda(body, &ag);
            int id = agenda_cadastrar(&ag);
            snprintf(buf, sizeof(buf), "{\"ok\":true,\"id\":%d}", id);
            http_created(response, buf);

        } else if (strcmp(method, "PUT") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            /* Se body tiver "realizada":1, apenas marca */
            json_get_string(body, "realizada", srealizada, sizeof(srealizada));
            if (strcmp(srealizada, "1") == 0) {
                agenda_marcar_realizada(atoi(sid));
                snprintf(buf, sizeof(buf), "{\"ok\":true}");
            } else {
                Agenda ag;
                json_to_agenda(body, &ag);
                int r = agenda_editar(atoi(sid), &ag);
                snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            }
            http_ok(response, buf);

        } else if (strcmp(method, "DELETE") == 0) {
            if (strlen(sid) == 0) { http_error(response,400,"id obrigatorio"); return; }
            int r = agenda_excluir(atoi(sid));
            snprintf(buf, sizeof(buf), "{\"ok\":%s}", r==GC_OK?"true":"false");
            http_ok(response, buf);
        }
        return;
    }

    /* =========================================================
       /api/dashboard  — resumo geral
       ========================================================= */
    if (path_starts(path, "/api/dashboard") && strcmp(method,"GET")==0) {
        int total_alunos      = aluno_total();
        int total_professores = professor_total();
        int total_turmas      = turma_total();

        int total_fichas = 0;
        { FILE *fp = fopen(ARQ_FICHAS,"rb"); if(fp){ fseek(fp,0,SEEK_END); long s=ftell(fp); fclose(fp); if(s>0) total_fichas=(int)(s/sizeof(FichaTreino)); } }

        int total_agenda = 0;
        { FILE *fp = fopen(ARQ_AGENDA,"rb"); if(fp){ fseek(fp,0,SEEK_END); long s=ftell(fp); fclose(fp); if(s>0) total_agenda=(int)(s/sizeof(Agenda)); } }

        snprintf(buf, sizeof(buf),
            "{"
            "\"total_alunos\":%d,"
            "\"total_professores\":%d,"
            "\"total_turmas\":%d,"
            "\"total_fichas\":%d,"
            "\"total_agenda\":%d"
            "}",
            total_alunos, total_professores, total_turmas,
            total_fichas, total_agenda);
        http_ok(response, buf);
        return;
    }

    /* 404 */
    http_error(response, 404, "Rota nao encontrada");
}
