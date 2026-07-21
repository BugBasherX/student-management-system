/*
=========================================
 Student Management System — Web Server
 File: server.c
 Serves a web GUI on port 5000
=========================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define PORT       5000
#define BACKLOG    10
#define MAX_STU    1000
#define FIELD_LEN  128
#define DATA_FILE  "students.csv"

#define ADMIN_USER "admin"
#define ADMIN_PASS "admin123"

/* ---- Student ---- */
typedef struct {
    int   id;
    char  name[FIELD_LEN];
    int   age;
    char  course[FIELD_LEN];
    float gpa;
} Student;

static Student students[MAX_STU];
static int     num_students = 0;

/* ================================================================
   Data I/O  (CSV: id|name|age|course|gpa)
   ================================================================ */
static int next_id() {
    int max = 0;
    for (int i = 0; i < num_students; i++)
        if (students[i].id > max) max = students[i].id;
    return max + 1;
}

static void load_students() {
    FILE *f = fopen(DATA_FILE, "r");
    if (!f) { num_students = 0; return; }
    num_students = 0;
    char line[512];
    while (fgets(line, sizeof(line), f) && num_students < MAX_STU) {
        Student *s = &students[num_students];
        char gpa_str[16];
        /* Use | as delimiter so commas can appear in names/courses */
        if (sscanf(line, "%d|%127[^|]|%d|%127[^|]|%15s",
                   &s->id, s->name, &s->age, s->course, gpa_str) == 5) {
            s->gpa = (float)atof(gpa_str);
            num_students++;
        }
    }
    fclose(f);
}

static void save_students() {
    FILE *f = fopen(DATA_FILE, "w");
    if (!f) return;
    for (int i = 0; i < num_students; i++)
        fprintf(f, "%d|%s|%d|%s|%.2f\n",
                students[i].id, students[i].name,
                students[i].age, students[i].course,
                students[i].gpa);
    fclose(f);
}

/* ================================================================
   JSON helpers
   ================================================================ */
static void json_escape(const char *in, char *out, int max) {
    int j = 0;
    for (int i = 0; in[i] && j < max - 2; i++) {
        if (in[i] == '"' || in[i] == '\\') out[j++] = '\\';
        else if (in[i] == '\n') { out[j++] = '\\'; out[j++] = 'n'; continue; }
        else if (in[i] == '\r') continue;
        out[j++] = in[i];
    }
    out[j] = '\0';
}

/* Extract a string field from a flat JSON object */
static int json_get_str(const char *json, const char *key, char *out, int max) {
    char pat[FIELD_LEN + 4];
    snprintf(pat, sizeof(pat), "\"%s\"", key);
    const char *p = strstr(json, pat);
    if (!p) return 0;
    p += strlen(pat);
    while (*p == ' ' || *p == ':' || *p == '\t') p++;
    if (*p != '"') return 0;
    p++;
    int i = 0;
    while (*p && *p != '"' && i < max - 1) {
        if (*p == '\\' && *(p + 1)) { p++; }
        out[i++] = *p++;
    }
    out[i] = '\0';
    return 1;
}

static int json_get_int(const char *json, const char *key, int *out) {
    char pat[FIELD_LEN + 4];
    snprintf(pat, sizeof(pat), "\"%s\"", key);
    const char *p = strstr(json, pat);
    if (!p) return 0;
    p += strlen(pat);
    while (*p == ' ' || *p == ':' || *p == '\t') p++;
    *out = atoi(p);
    return 1;
}

static int json_get_float(const char *json, const char *key, float *out) {
    char pat[FIELD_LEN + 4];
    snprintf(pat, sizeof(pat), "\"%s\"", key);
    const char *p = strstr(json, pat);
    if (!p) return 0;
    p += strlen(pat);
    while (*p == ' ' || *p == ':' || *p == '\t') p++;
    *out = (float)atof(p);
    return 1;
}

/* Build a JSON array of all students */
static void build_students_json(char *buf, int max) {
    int pos = 0;
    pos += snprintf(buf + pos, max - pos, "[");
    for (int i = 0; i < num_students && pos < max - 200; i++) {
        Student *s = &students[i];
        char en[FIELD_LEN * 2], ec[FIELD_LEN * 2];
        json_escape(s->name,   en, sizeof(en));
        json_escape(s->course, ec, sizeof(ec));
        pos += snprintf(buf + pos, max - pos,
            "%s{\"id\":%d,\"name\":\"%s\",\"age\":%d,\"course\":\"%s\",\"gpa\":%.2f}",
            i > 0 ? "," : "", s->id, en, s->age, ec, s->gpa);
    }
    pos += snprintf(buf + pos, max - pos, "]");
}

/* ================================================================
   HTTP helpers
   ================================================================ */
static const char *status_text(int code) {
    switch (code) {
        case 200: return "OK";
        case 301: return "Moved Permanently";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 404: return "Not Found";
        default:  return "Internal Server Error";
    }
}

static void send_headers(int fd, int code, const char *ct, int len) {
    char hdr[512];
    snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n"
        "\r\n",
        code, status_text(code), ct, len);
    write(fd, hdr, strlen(hdr));
}

static void send_json(int fd, int code, const char *body) {
    send_headers(fd, code, "application/json; charset=utf-8", (int)strlen(body));
    write(fd, body, strlen(body));
}

static void serve_file(int fd, const char *fpath) {
    const char *ct = "text/plain";
    if (strstr(fpath, ".html")) ct = "text/html; charset=utf-8";
    else if (strstr(fpath, ".css")) ct = "text/css";
    else if (strstr(fpath, ".js"))  ct = "application/javascript";

    FILE *f = fopen(fpath, "rb");
    if (!f) {
        const char *body = "404 Not Found";
        send_headers(fd, 404, "text/plain", (int)strlen(body));
        write(fd, body, strlen(body));
        return;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f); rewind(f);
    char *buf = malloc(sz);
    if (!buf) { fclose(f); return; }
    fread(buf, 1, sz, f);
    fclose(f);
    send_headers(fd, 200, ct, (int)sz);
    write(fd, buf, sz);
    free(buf);
}

/* ================================================================
   Request parsing
   ================================================================ */
typedef struct {
    char method[8];
    char path[256];
    char query[256];
    char body[16384];
} Req;

static void parse_request(const char *raw, Req *r) {
    memset(r, 0, sizeof(*r));
    const char *p = raw;
    int i = 0;
    while (*p && *p != ' ' && i < 7) r->method[i++] = *p++;
    if (*p == ' ') p++;
    char full[512]; i = 0;
    while (*p && *p != ' ' && *p != '\r' && i < 511) full[i++] = *p++;
    full[i] = '\0';
    char *q = strchr(full, '?');
    if (q) { strncpy(r->query, q + 1, 255); *q = '\0'; }
    strncpy(r->path, full, 255);
    const char *body = strstr(raw, "\r\n\r\n");
    if (body) {
        body += 4;
        int len = (int)strlen(body);
        if (len > 16383) len = 16383;
        memcpy(r->body, body, len);
        r->body[len] = '\0';
    }
}

static int qparam(const char *query, const char *key, char *out, int max) {
    char pat[64];
    snprintf(pat, sizeof(pat), "%s=", key);
    const char *p = strstr(query, pat);
    if (!p) return 0;
    p += strlen(pat);
    int i = 0;
    while (*p && *p != '&' && i < max - 1) out[i++] = *p++;
    out[i] = '\0';
    return 1;
}

/* ================================================================
   API handlers
   ================================================================ */
static void handle_api(int fd, Req *r) {
    /* CORS preflight */
    if (strcmp(r->method, "OPTIONS") == 0) {
        const char *h = "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Content-Length: 0\r\n\r\n";
        write(fd, h, strlen(h));
        return;
    }

    /* POST /api/login */
    if (!strcmp(r->path, "/api/login") && !strcmp(r->method, "POST")) {
        char user[64] = "", pass[64] = "";
        json_get_str(r->body, "username", user, sizeof(user));
        json_get_str(r->body, "password", pass, sizeof(pass));
        if (!strcmp(user, ADMIN_USER) && !strcmp(pass, ADMIN_PASS))
            send_json(fd, 200, "{\"ok\":true}");
        else
            send_json(fd, 401, "{\"ok\":false,\"error\":\"Invalid credentials\"}");
        return;
    }

    /* GET /api/students */
    if (!strcmp(r->path, "/api/students") && !strcmp(r->method, "GET")) {
        load_students();
        char *buf = malloc(MAX_STU * 300);
        if (!buf) { send_json(fd, 500, "{\"ok\":false}"); return; }
        build_students_json(buf, MAX_STU * 300);
        send_json(fd, 200, buf);
        free(buf);
        return;
    }

    /* POST /api/students — add */
    if (!strcmp(r->path, "/api/students") && !strcmp(r->method, "POST")) {
        if (num_students >= MAX_STU) {
            send_json(fd, 400, "{\"ok\":false,\"error\":\"Student limit reached\"}");
            return;
        }
        Student s = {0};
        s.id = next_id();
        json_get_str(r->body, "name",   s.name,   FIELD_LEN);
        json_get_int(r->body, "age",    &s.age);
        json_get_str(r->body, "course", s.course, FIELD_LEN);
        json_get_float(r->body, "gpa",  &s.gpa);
        if (!s.name[0]) { send_json(fd, 400, "{\"ok\":false,\"error\":\"Name is required\"}"); return; }
        /* Strip pipe chars from name/course to protect CSV format */
        for (int i = 0; s.name[i];   i++) if (s.name[i]   == '|') s.name[i]   = '-';
        for (int i = 0; s.course[i]; i++) if (s.course[i] == '|') s.course[i] = '-';
        students[num_students++] = s;
        save_students();
        char resp[64];
        snprintf(resp, sizeof(resp), "{\"ok\":true,\"id\":%d}", s.id);
        send_json(fd, 200, resp);
        return;
    }

    /* PUT /api/students?id=N — update */
    if (!strcmp(r->path, "/api/students") && !strcmp(r->method, "PUT")) {
        char ids[16]; int id;
        if (!qparam(r->query, "id", ids, sizeof(ids))) {
            send_json(fd, 400, "{\"ok\":false,\"error\":\"Missing id\"}"); return;
        }
        id = atoi(ids);
        for (int i = 0; i < num_students; i++) {
            if (students[i].id != id) continue;
            char tmp[FIELD_LEN];
            if (json_get_str(r->body, "name", tmp, FIELD_LEN)) {
                strncpy(students[i].name, tmp, FIELD_LEN - 1);
                for (int j = 0; students[i].name[j]; j++)
                    if (students[i].name[j] == '|') students[i].name[j] = '-';
            }
            int age = 0;
            if (json_get_int(r->body, "age", &age)) students[i].age = age;
            if (json_get_str(r->body, "course", tmp, FIELD_LEN)) {
                strncpy(students[i].course, tmp, FIELD_LEN - 1);
                for (int j = 0; students[i].course[j]; j++)
                    if (students[i].course[j] == '|') students[i].course[j] = '-';
            }
            float gpa = 0;
            if (json_get_float(r->body, "gpa", &gpa)) students[i].gpa = gpa;
            save_students();
            send_json(fd, 200, "{\"ok\":true}");
            return;
        }
        send_json(fd, 404, "{\"ok\":false,\"error\":\"Student not found\"}");
        return;
    }

    /* DELETE /api/students?id=N */
    if (!strcmp(r->path, "/api/students") && !strcmp(r->method, "DELETE")) {
        char ids[16]; int id;
        if (!qparam(r->query, "id", ids, sizeof(ids))) {
            send_json(fd, 400, "{\"ok\":false,\"error\":\"Missing id\"}"); return;
        }
        id = atoi(ids);
        for (int i = 0; i < num_students; i++) {
            if (students[i].id != id) continue;
            memmove(&students[i], &students[i+1],
                    (num_students - i - 1) * sizeof(Student));
            num_students--;
            save_students();
            send_json(fd, 200, "{\"ok\":true}");
            return;
        }
        send_json(fd, 404, "{\"ok\":false,\"error\":\"Student not found\"}");
        return;
    }

    send_json(fd, 404, "{\"ok\":false,\"error\":\"Unknown endpoint\"}");
}

/* ================================================================
   Client handler
   ================================================================ */
static void handle_client(int fd) {
    char raw[32768];
    int n = (int)read(fd, raw, sizeof(raw) - 1);
    if (n <= 0) { close(fd); return; }
    raw[n] = '\0';

    Req req;
    parse_request(raw, &req);

    if (!strcmp(req.path, "/")) {
        const char *redir =
            "HTTP/1.1 301 Moved Permanently\r\n"
            "Location: /index.html\r\n"
            "Content-Length: 0\r\n\r\n";
        write(fd, redir, strlen(redir));
    } else if (!strncmp(req.path, "/api/", 5) ||
               !strcmp(req.path, "/api/login") ||
               !strcmp(req.path, "/api/students")) {
        handle_api(fd, &req);
    } else {
        char fpath[300];
        snprintf(fpath, sizeof(fpath), "static%s", req.path);
        serve_file(fd, fpath);
    }

    close(fd);
}

/* ================================================================
   Main
   ================================================================ */
int main() {
    load_students();

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    listen(srv, BACKLOG);
    printf("Student Management System running at http://0.0.0.0:%d\n", PORT);
    fflush(stdout);

    while (1) {
        struct sockaddr_in ca; socklen_t cl = sizeof(ca);
        int client = accept(srv, (struct sockaddr *)&ca, &cl);
        if (client < 0) continue;
        handle_client(client);
    }
    return 0;
}
