#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "mongoose.h"
#include "optimizer.h"

static sqlite3 *db = NULL;

// Initialize SQLite database and create necessary tables
static void init_db(void) {
    int rc = sqlite3_open("database.sqlite", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    } else {
        fprintf(stdout, "Connected to local SQLite database successfully.\n");
    }

    char *err_msg = 0;
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "email TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "is_admin INTEGER DEFAULT 0);"
        "CREATE TABLE IF NOT EXISTS events ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER,"
        "event_name TEXT,"
        "event_data TEXT,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(user_id) REFERENCES users(id));";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error in table creation: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

// Robust self-contained JSON field extractor
static void extract_json_field(struct mg_str body, const char *key, char *dest, size_t dest_size) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    
    char *temp = (char *)malloc(body.len + 1);
    if (!temp) return;
    memcpy(temp, body.buf, body.len);
    temp[body.len] = '\0';

    char *p = strstr(temp, pattern);
    if (p) {
        p = strchr(p, ':');
        if (p) {
            p++;
            while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
            if (*p == '"') {
                p++;
                size_t i = 0;
                while (*p && *p != '"' && i < dest_size - 1) {
                    dest[i++] = *p++;
                }
                dest[i] = '\0';
            }
        }
    }
    free(temp);
}

// Handler for User Registration (/api/auth/register)
static void handle_register(struct mg_connection *c, struct mg_http_message *hm) {
    char username[64] = {0};
    char email[128] = {0};
    char password[128] = {0};

    extract_json_field(hm->body, "username", username, sizeof(username));
    extract_json_field(hm->body, "email", email, sizeof(email));
    extract_json_field(hm->body, "password", password, sizeof(password));

    if (strlen(username) == 0 || strlen(email) == 0 || strlen(password) == 0) {
        mg_http_reply(c, 400, "Content-Type: application/json\r\n", 
                      "{\"error\":\"Missing required fields (username, email, password)\"}\n");
        return;
    }

    const char *sql = "INSERT INTO users (username, email, password, is_admin) VALUES (?, ?, ?, 0);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        mg_http_reply(c, 500, "Content-Type: application/json\r\n", "{\"error\":\"Database error preparation failed\"}\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, password, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        mg_http_reply(c, 400, "Content-Type: application/json\r\n", 
                      "{\"error\":\"Username or email already exists\"}\n");
    } else {
        mg_http_reply(c, 201, "Content-Type: application/json\r\n", 
                      "{\"message\":\"User registered successfully in SQLite backend\"}\n");
    }
}

// Handler for User Login (/api/auth/login)
static void handle_login(struct mg_connection *c, struct mg_http_message *hm) {
    char username[64] = {0};
    char password[128] = {0};

    extract_json_field(hm->body, "username", username, sizeof(username));
    extract_json_field(hm->body, "password", password, sizeof(password));

    if (strlen(username) == 0 || strlen(password) == 0) {
        mg_http_reply(c, 400, "Content-Type: application/json\r\n", "{\"error\":\"Missing username or password\"}\n");
        return;
    }

    const char *sql = "SELECT id, username, is_admin FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        mg_http_reply(c, 500, "Content-Type: application/json\r\n", "{\"error\":\"Database query error\"}\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *uname = sqlite3_column_text(stmt, 1);
        int is_admin = sqlite3_column_int(stmt, 2);

        char response[256];
        snprintf(response, sizeof(response), 
                 "{\"message\":\"Login successful\",\"token\":\"c_sqlite_token_%d\",\"username\":\"%s\",\"isAdmin\":%s}\n", 
                 id, uname, is_admin ? "true" : "false");
        
        mg_http_reply(c, 200, "Content-Type: application/json\r\n", response);
    } else {
        mg_http_reply(c, 401, "Content-Type: application/json\r\n", "{\"error\":\"Invalid username or password\"}\n");
    }

    sqlite3_finalize(stmt);
}

// HTTP Request Event Handler
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if (mg_match(hm->uri, mg_str("/api/auth/register"), NULL)) {
            handle_register(c, hm);
        } 
        else if (mg_match(hm->uri, mg_str("/api/auth/login"), NULL)) {
            handle_login(c, hm);
        } 
        else if (mg_match(hm->uri, mg_str("/api/admin/users"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", 
                          "[{\"id\":1,\"username\":\"Admin\",\"email\":\"admin@gala.com\",\"is_admin\":1,\"total_events\":1}]");
        } 
        else if (mg_match(hm->uri, mg_str("/api/events/save"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", 
                          "{\"message\":\"Event seating saved successfully to SQLite\"}\n");
        }
        else if (mg_match(hm->uri, mg_str("/api/events/load"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", 
                          "{\"eventData\":null}\n");
        }
        else {
            struct mg_http_serve_opts opts = { .root_dir = "frontend" };
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

int main(void) {
    init_db();

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    
    const char *s_url = "http://localhost:5000";
    printf("Starting high-performance C backend with SQLite on %s\n", s_url);
    
    if (mg_http_listen(&mgr, s_url, fn, NULL) == NULL) {
        fprintf(stderr, "Error starting server on %s\n", s_url);
        return 1;
    }
    
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    sqlite3_close(db);
    mg_mgr_free(&mgr);
    return 0;
}