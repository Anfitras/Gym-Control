/* ================================================================
   GYMCONTROL — server.c
   ================================================================ */
#include "gymcontrol.h"

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  typedef int socklen_t;
  #define CLOSE_SOCKET(s) closesocket(s)
  typedef SOCKET SOCKET_T;
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #define CLOSE_SOCKET(s) close(s)
  typedef int SOCKET_T;
  #define INVALID_SOCKET  (-1)
  #define SOCKET_ERROR    (-1)
#endif

#define BUF_REQ  (64  * 1024)
#define BUF_RESP (256 * 1024)

static const char *content_type(const char *path) {
    const char *e = strrchr(path, '.');
    if (!e) return "text/plain";
    if (strcmp(e,".html")==0) return "text/html; charset=utf-8";
    if (strcmp(e,".css") ==0) return "text/css; charset=utf-8";
    if (strcmp(e,".js")  ==0) return "application/javascript; charset=utf-8";
    if (strcmp(e,".json")==0) return "application/json";
    if (strcmp(e,".ico") ==0) return "image/x-icon";
    return "text/plain";
}

static void serve_file(const char *url_path, char *resp, int rmax) {
    const char *f = url_path;
    if (strcmp(f,"/")==0 || strcmp(f,"/index.html")==0) f = "/dashboard.html";

    char fp[512];
    snprintf(fp, sizeof(fp), "public%s", f);

    FILE *fd = fopen(fp, "rb");
    if (!fd) {
        snprintf(resp, rmax,
            "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n"
            "Connection: close\r\n\r\n<h2>404: %s</h2>", fp);
        return;
    }
    fseek(fd,0,SEEK_END); long fsz=ftell(fd); rewind(fd);

    int hlen = snprintf(resp, rmax,
        "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n"
        "Content-Length: %ld\r\nAccess-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n\r\n",
        content_type(fp), fsz);

    long space = rmax - hlen - 1;
    long rsz   = fsz < space ? fsz : space;
    fread(resp+hlen, 1, (size_t)rsz, fd);
    resp[hlen+rsz] = '\0';
    fclose(fd);
}

/* ----------------------------------------------------------------
   parse_req — encontra o body após os headers HTTP
   Suporta tanto \r\n\r\n (padrão) quanto \n\n (alguns clientes)
   ---------------------------------------------------------------- */
static void parse_req(const char *raw, char *method,
                      char *fullpath, char *body) {
    sscanf(raw, "%15s %1023s", method, fullpath);

    /* Tenta \r\n\r\n primeiro (padrão HTTP) */
    const char *s = strstr(raw, "\r\n\r\n");
    if (s) {
        strncpy(body, s + 4, 32767);
        body[32767] = '\0';
        return;
    }

    /* Fallback: \n\n (PowerShell, alguns clientes) */
    s = strstr(raw, "\n\n");
    if (s) {
        strncpy(body, s + 2, 32767);
        body[32767] = '\0';
        /* Remove \r se houver no início */
        if (body[0] == '\r') memmove(body, body+1, strlen(body));
        return;
    }

    body[0] = '\0';
}

/* ----------------------------------------------------------------
   Limpa caracteres inválidos do body (BOM UTF-16, chars de controle)
   ---------------------------------------------------------------- */
static void sanitize_body(char *body) {
    /* Remove BOM UTF-8 (EF BB BF) */
    if ((unsigned char)body[0] == 0xEF &&
        (unsigned char)body[1] == 0xBB &&
        (unsigned char)body[2] == 0xBF) {
        memmove(body, body + 3, strlen(body) - 2);
    }
    /* Remove BOM UTF-16 LE (FF FE) */
    if ((unsigned char)body[0] == 0xFF &&
        (unsigned char)body[1] == 0xFE) {
        memmove(body, body + 2, strlen(body) - 1);
    }
    /* Encontra início do JSON { ou [ */
    char *start = body;
    while (*start && *start != '{' && *start != '[') start++;
    if (start != body && *start) {
        memmove(body, start, strlen(start) + 1);
    }
}

void server_start(int porta) {
#ifdef _WIN32
    WSADATA w; WSAStartup(MAKEWORD(2,2),&w);
#endif
    SOCKET_T srv = socket(AF_INET,SOCK_STREAM,0);
    if (srv==INVALID_SOCKET){ perror("socket"); exit(1); }

    int opt = 1;
#ifdef _WIN32
    /* No Windows, SO_REUSEADDR permite binds concorrentes no mesmo host/porta. */
    setsockopt(srv, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&opt, sizeof(opt));
#else
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons((unsigned short)porta);

    if (bind(srv,(struct sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR){
        perror("bind"); CLOSE_SOCKET(srv); exit(1);
    }
    if (listen(srv,32)==SOCKET_ERROR){
        perror("listen"); CLOSE_SOCKET(srv); exit(1);
    }

    printf("\n");
    printf("  +------------------------------------------+\n");
    printf("  |   GymControl  -  Backend C               |\n");
    printf("  |                                          |\n");
    printf("  |   Acesse: http://localhost:%d         |\n", porta);
    printf("  |   API:    http://localhost:%d/api/... |\n", porta);
    printf("  |   Ctrl+C para encerrar                   |\n");
    printf("  +------------------------------------------+\n\n");
    fflush(stdout);

    char *raw  = (char*)malloc(BUF_REQ);
    char *resp = (char*)malloc(BUF_RESP);
    char  method[16], fullpath[1024], body[32768];

    while (1) {
        struct sockaddr_in cli; socklen_t clen=sizeof(cli);
        SOCKET_T client=accept(srv,(struct sockaddr*)&cli,&clen);
        if (client==INVALID_SOCKET) continue;

        memset(raw,0,BUF_REQ);
        int n=(int)recv(client,raw,BUF_REQ-1,0);
        if (n<=0){ CLOSE_SOCKET(client); continue; }
        raw[n]='\0';

        memset(method,0,sizeof(method));
        memset(fullpath,0,sizeof(fullpath));
        memset(body,0,sizeof(body));
        parse_req(raw,method,fullpath,body);

        /* Limpa body de possíveis BOMs e lixo do PowerShell */
        if (strlen(body) > 0) sanitize_body(body);

        printf("  [%s] %s\n",method,fullpath); fflush(stdout);

        memset(resp,0,BUF_RESP);

        if (strncmp(fullpath,"/api/",5)==0 || strcmp(method,"OPTIONS")==0) {
            route_handle(method,fullpath,body,resp);
        } else {
            char clean[512];
            strncpy(clean,fullpath,sizeof(clean)-1);
            char *q=strchr(clean,'?'); if(q)*q='\0';
            serve_file(clean,resp,BUF_RESP);
        }

        send(client,resp,(int)strlen(resp),0);
        CLOSE_SOCKET(client);
    }
    free(raw); free(resp);
    CLOSE_SOCKET(srv);
#ifdef _WIN32
    WSACleanup();
#endif
}