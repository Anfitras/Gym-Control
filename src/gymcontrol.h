/* ================================================================
   GYMCONTROL — gymcontrol.h
   Header principal: structs, constantes e protótipos de todos
   os módulos do backend C.
   ================================================================ */
#ifndef GYMCONTROL_H
#define GYMCONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* ----------------------------------------------------------------
   ARQUIVOS DE DADOS
   ---------------------------------------------------------------- */
#define ARQ_ALUNOS      "data/alunos.dat"
#define ARQ_PROFESSORES "data/professores.dat"
#define ARQ_TURMAS      "data/turmas.dat"
#define ARQ_FICHAS      "data/fichas.dat"
#define ARQ_AGENDA      "data/agenda.dat"

/* ----------------------------------------------------------------
   LIMITES
   ---------------------------------------------------------------- */
#define MAX_NOME          100
#define MAX_CPF            20
#define MAX_TEL            20
#define MAX_EMAIL         100
#define MAX_HORARIO        50
#define MAX_OBS           300
#define MAX_EXERCICIOS     20
#define MAX_ESPECIALIDADE  50
#define MAX_MODALIDADE     50
#define MAX_OBJETIVO       50
#define MAX_DIAS           30

/* Códigos de retorno */
#define GC_OK              0
#define GC_ERRO           -1
#define GC_NAO_ENCONTRADO -2
#define GC_CHEIO          -3

/* ----------------------------------------------------------------
   ENUMERAÇÕES
   ---------------------------------------------------------------- */
typedef enum { STATUS_INATIVO=0, STATUS_ATIVO=1, STATUS_PENDENTE=2 } StatusCadastro;
typedef enum { SEXO_MASCULINO=1, SEXO_FEMININO=2, SEXO_OUTRO=3 }     Sexo;
typedef enum {
    OBJ_HIPERTROFIA=1, OBJ_EMAGRECIMENTO=2,
    OBJ_CONDICIONAMENTO=3, OBJ_REABILITACAO=4
} ObjetivoTreino;

/* ----------------------------------------------------------------
   STRUCTS
   ---------------------------------------------------------------- */
typedef struct {
    int    numero;
    char   nome[MAX_NOME];
    char   grupo_muscular[MAX_NOME];
    int    series;
    int    repeticoes;
    float  carga_kg;
    char   observacao[MAX_OBS];
} Exercicio;

typedef struct {
    int            id;
    char           nome[MAX_NOME];
    char           cpf[MAX_CPF];
    int            idade;
    Sexo           sexo;
    char           telefone[MAX_TEL];
    char           email[MAX_EMAIL];
    int            id_turma;
    int            id_professor;
    StatusCadastro status;
    char           data_matricula[12];
    float          peso_kg;
    float          altura_m;
} Aluno;

typedef struct {
    int            id;
    char           nome[MAX_NOME];
    char           cpf[MAX_CPF];
    char           telefone[MAX_TEL];
    char           email[MAX_EMAIL];
    char           especialidade[MAX_ESPECIALIDADE];
    char           horario_disponivel[MAX_HORARIO];
    StatusCadastro status;
} Professor;

typedef struct {
    int            id;
    char           nome[MAX_NOME];
    char           modalidade[MAX_MODALIDADE];
    int            id_professor;
    char           horario[MAX_HORARIO];
    char           dias_semana[MAX_DIAS];
    int            capacidade_max;
    StatusCadastro status;
} Turma;

typedef struct {
    int            id;
    int            id_aluno;
    int            id_professor;
    ObjetivoTreino objetivo;
    char           data_criacao[12];
    char           observacoes[MAX_OBS];
    int            total_exercicios;
    Exercicio      exercicios[MAX_EXERCICIOS];
} FichaTreino;

typedef struct {
    int  id;
    int  id_professor;
    int  id_turma;
    char data[12];
    char hora_inicio[8];
    char hora_fim[8];
    char atividade[MAX_NOME];
    char observacao[MAX_OBS];
    int  realizada;
} Agenda;
/* ----------------------------------------------------------------
   PROTÓTIPOS — utils.c
   ---------------------------------------------------------------- */
void        utils_criar_dirs(void);
int         utils_proximo_id(const char *arq, size_t sz);
void        utils_data_hoje(char *dest);
const char *utils_status_str(StatusCadastro s);
const char *utils_objetivo_str(ObjetivoTreino o);
const char *utils_sexo_str(Sexo s);

/* ----------------------------------------------------------------
   PROTÓTIPOS — alunos.c
   ---------------------------------------------------------------- */
int  aluno_cadastrar(Aluno *a);
int  aluno_listar_json(char *buf, int maxlen);
int  aluno_buscar_id(int id, Aluno *dest);
int  aluno_editar(int id, Aluno *novo);
int  aluno_excluir(int id);
int  aluno_total(void);
int  aluno_listar_turma_json(int id_turma, char *buf, int maxlen);

/* ----------------------------------------------------------------
   PROTÓTIPOS — professores.c
   ---------------------------------------------------------------- */
int  professor_cadastrar(Professor *p);
int  professor_listar_json(char *buf, int maxlen);
int  professor_buscar_id(int id, Professor *dest);
int  professor_editar(int id, Professor *novo);
int  professor_excluir(int id);
int  professor_total(void);

/* ----------------------------------------------------------------
   PROTÓTIPOS — turmas.c
   ---------------------------------------------------------------- */
int  turma_cadastrar(Turma *t);
int  turma_listar_json(char *buf, int maxlen);
int  turma_buscar_id(int id, Turma *dest);
int  turma_editar(int id, Turma *novo);
int  turma_excluir(int id);
int  turma_total(void);
int  turma_vagas_disponiveis(int id_turma);
int  turma_ocupacao_percent(int id_turma);

/* ----------------------------------------------------------------
   PROTÓTIPOS — fichas.c
   ---------------------------------------------------------------- */
int  ficha_adicionar_exercicio(int id_ficha, Exercicio *ex);
int  ficha_cadastrar(FichaTreino *f);
int  ficha_listar_json(char *buf, int maxlen);
int  ficha_buscar_id(int id, FichaTreino *dest);
int  ficha_buscar_id_json(int id, char *buf, int maxlen);
int  ficha_buscar_aluno_json(int id_aluno, char *buf, int maxlen);
int  ficha_editar(int id, FichaTreino *novo);
int  ficha_excluir(int id);

/* ----------------------------------------------------------------
   PROTÓTIPOS — agenda.c
   ---------------------------------------------------------------- */
int  agenda_cadastrar(Agenda *ag);
int  agenda_listar_json(char *buf, int maxlen);
int  agenda_listar_data_json(const char *data, char *buf, int maxlen);
int  agenda_listar_professor_json(int id_prof, char *buf, int maxlen);
int  agenda_buscar_id(int id, Agenda *dest);
int  agenda_editar(int id, Agenda *novo);
int  agenda_excluir(int id);
int  agenda_marcar_realizada(int id);

/* ----------------------------------------------------------------
   PROTÓTIPOS — json_parser.c
   ---------------------------------------------------------------- */
int  json_get_string(const char *json, const char *key, char *dest, int maxlen);
int  json_get_int(const char *json, const char *key);
float json_get_float(const char *json, const char *key);

/* ----------------------------------------------------------------
   PROTÓTIPOS — server.c
   ---------------------------------------------------------------- */
void server_start(int porta);

/* ----------------------------------------------------------------
   PROTÓTIPOS — routes.c
   ---------------------------------------------------------------- */
void route_handle(const char *method, const char *path,
                  const char *body, char *response);

#endif /* GYMCONTROL_H */
