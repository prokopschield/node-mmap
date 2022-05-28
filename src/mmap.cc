#include <errno.h>
#include <fcntl.h>
#include <node_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void node_unmmap(napi_env env, void* data, void* size)
{
    munmap(data, *((size_t*)size));
    free(size);
}

#define NODE_MMAP_ARGC 2
#define NODE_MMAP_PATHSTR_SIZE 4096

napi_value node_mmap(napi_env env, napi_callback_info info)
{
    napi_value ret;

    size_t argc = NODE_MMAP_ARGC;

    napi_value argv[NODE_MMAP_ARGC];

    char* pathstr = (char*)malloc(NODE_MMAP_PATHSTR_SIZE);

    if (pathstr == NULL) {
        napi_throw_error(env, "1", "Node.js is out of memory.");
    }

    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    napi_get_value_string_utf8(env, argv[0], pathstr, NODE_MMAP_PATHSTR_SIZE, nullptr);

    int32_t readonly = 0;

    if (argc >= 2) {
        napi_get_value_bool(env, argv[1], (bool*)&readonly);
        if (!readonly) {
            napi_get_value_int32(env, argv[1], &readonly);
        }
    }

    int fd = open(pathstr, readonly ? O_RDONLY : O_RDWR);

    free(pathstr);

    if (fd <= 0) {
        static char code[32];
        sprintf(code, "%d", errno);
        napi_throw_error(env, code, strerror(errno));
    }

    struct stat s;

    if (fstat(fd, &s)) {
        close(fd);

        static char code[32];
        sprintf(code, "%d", errno);
        napi_throw_error(env, code, strerror(errno));
    }

    size_t* size = (size_t*)malloc(sizeof(size_t));
    *size = s.st_size;

    void* mapping = mmap(NULL, *size, readonly ? PROT_READ : PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    napi_create_external_buffer(env, *size, mapping, node_unmmap, size, &ret);

    close(fd);

    return ret;
}

napi_value init(napi_env env, napi_value exports)
{
    napi_value mmap;

    napi_create_function(env, "mmap", 4, node_mmap, nullptr, &mmap);

    return mmap;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
