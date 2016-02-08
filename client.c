#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define log(x) printf("%s\n", x);

uv_loop_t *loop;

void on_connect(uv_connect_t *req, int status);
void on_write_end(uv_write_t *req, int status);
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void echo_read(uv_stream_t *server, ssize_t nread, const uv_buf_t* buf);

void echo_read(uv_stream_t *server, ssize_t nread, const uv_buf_t* buf) {
    if (nread == -1) {
        fprintf(stderr, "error echo_read");
        return;
    }

    printf("result: %s\n", buf->base);
}

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    //uv_buf_init((char *) malloc(suggested_size), suggested_size);
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void on_write_end(uv_write_t *req, int status) {
    if (status == -1) {
        fprintf(stderr, "error on_write_end");
        return;
    }
    uv_read_start(req->handle, alloc_buffer, echo_read);
}

void on_connect(uv_connect_t * req, int status) {
    if (status == -1) {
        fprintf(stderr, "error on_write_end");
        return;
    }
    char *message = "message.txt";
    int len = strlen(message);
    char buffer[100];
    uv_buf_t buf = uv_buf_init(buffer, sizeof(buffer));
    buf.len = len;
    buf.base = message;
    uv_stream_t *tcp = req->handle;
    uv_write_t write_req;
    int buf_count = 1;
    uv_write(&write_req, tcp, &buf, buf_count, on_write_end);
}

int main(void) {
    loop = uv_default_loop();
    uv_tcp_t client;
    uv_tcp_init(loop, &client);
    struct sockaddr_in req_addr;
    uv_ip4_addr("127.0.0.1", 7000, &req_addr);
    uv_connect_t connect_req;
    uv_tcp_connect(&connect_req, &client, (struct sockaddr *)&req_addr, on_connect);
    return uv_run(loop, UV_RUN_DEFAULT);
}
