# GymControl

Sistema de gerenciamento de academia.

**Interface:** HTML + CSS — sem alterações visuais
**Lógica/Backend:** Linguagem C pura — sockets, CRUD, arquivos binários

---

## Estrutura do projeto

```
gymcontrol/
│
├── src/                    ← Todo o código C
│   ├── main.c              ← Ponto de entrada (inicia o servidor)
│   ├── server.c            ← Servidor HTTP em C puro (sockets)
│   ├── routes.c            ← Roteador: mapeia URL → função C
│   ├── gymcontrol.h        ← Header: structs, enums, protótipos
│   ├── alunos.c            ← CRUD de alunos
│   ├── professores.c       ← CRUD de professores
│   ├── turmas.c            ← CRUD de turmas + cálculo de vagas
│   ├── fichas.c            ← CRUD de fichas de treino
│   ├── agenda.c            ← CRUD de agenda de aulas
│   ├── json_parser.c       ← Parser JSON minimalista
│   └── utils.c             ← Helpers: IDs, datas, status
│
├── data/                   ← Dados persistidos (gerado automaticamente)
│   ├── alunos.dat
│   ├── professores.dat
│   ├── turmas.dat
│   ├── fichas.dat
│   └── agenda.dat
│
├── public/                 ← Interface web (HTML + CSS intactos)
│   ├── login.html
│   ├── dashboard.html
│   ├── alunos.html
│   ├── professores.html
│   ├── turmas.html
│   ├── controle.html
│   ├── fichas.html
│   ├── style.css           ← CSS original, sem alterações
│   └── gymcontrol.js       ← Conecta HTML ↔ backend C via fetch
│
└── README.md
```

---

## Como compilar

### Linux / macOS
```bash
cd src

gcc -Wall -std=c99 \
  main.c server.c routes.c json_parser.c \
  utils.c alunos.c professores.c turmas.c fichas.c agenda.c \
  -o ../gymcontrol
```

### Windows (MinGW / Git Bash)
```cmd
cd src

gcc -Wall -std=c99 ^
  main.c server.c routes.c json_parser.c ^
  utils.c alunos.c professores.c turmas.c fichas.c agenda.c ^
  -o ../gymcontrol.exe -lws2_32
```

---

## Como executar

```bash
# A partir da raiz do projeto (onde ficam as pastas src/ data/ public/)
./gymcontrol        # Linux/macOS
gymcontrol.exe      # Windows
```

Abra o navegador em: **http://localhost:8080**

O servidor C serve os arquivos HTML automaticamente.

---

## Como funciona

```
Navegador                    Backend C (porta 8080)
─────────────────────────────────────────────────────
GET /dashboard.html    →  server.c lê public/dashboard.html
GET /api/alunos        →  routes.c → alunos.c → lê data/alunos.dat → JSON
POST /api/alunos       →  routes.c → alunos.c → grava data/alunos.dat
PUT /api/alunos?id=3   →  routes.c → alunos.c → edita data/alunos.dat
DELETE /api/alunos?id=3 → routes.c → alunos.c → remove de data/alunos.dat
```

O `gymcontrol.js` faz o `fetch()` para a API C e atualiza a tela.
O C nunca toca em HTML. O HTML nunca toca nos dados diretamente.

---

## Rotas da API C

| Método | Rota | Ação |
|--------|------|------|
| GET | /api/alunos | Lista todos |
| GET | /api/alunos?id=N | Busca por ID |
| GET | /api/alunos?turma=N | Filtra por turma |
| POST | /api/alunos | Cadastra novo |
| PUT | /api/alunos?id=N | Edita |
| DELETE | /api/alunos?id=N | Remove |
| GET | /api/professores | Lista todos |
| POST | /api/professores | Cadastra |
| PUT | /api/professores?id=N | Edita |
| DELETE | /api/professores?id=N | Remove |
| GET | /api/turmas | Lista com vagas calculadas |
| POST | /api/turmas | Cadastra |
| PUT | /api/turmas?id=N | Edita |
| DELETE | /api/turmas?id=N | Remove |
| GET | /api/fichas | Lista todas |
| GET | /api/fichas?aluno=N | Fichas de um aluno |
| POST | /api/fichas | Cadastra |
| DELETE | /api/fichas?id=N | Remove |
| GET | /api/agenda | Lista toda agenda |
| GET | /api/agenda?data=DD/MM/AAAA | Por data |
| GET | /api/agenda?professor=N | Por professor |
| POST | /api/agenda | Agenda aula |
| PUT | /api/agenda?id=N | Edita / marca realizada |
| DELETE | /api/agenda?id=N | Remove |
| GET | /api/dashboard | Totais gerais |

---

## Arquitetura (resumo para o trabalho)

```
┌─────────────────────┐        HTTP/JSON        ┌──────────────────────────┐
│   Interface Web     │ ◄──────────────────────► │   Backend em C           │
│                     │                          │                          │
│  HTML  → estrutura  │   GET /api/alunos        │  server.c  → sockets     │
│  CSS   → visual     │   POST /api/turmas       │  routes.c  → roteador    │
│  JS    → fetch()    │   DELETE /api/fichas?id= │  alunos.c  → CRUD + .dat │
│                     │                          │  turmas.c  → vagas       │
│  public/            │                          │  fichas.c  → exercícios  │
└─────────────────────┘                          └──────────────────────────┘
                                                           │
                                                    data/*.dat
                                               (arquivos binários)
```
