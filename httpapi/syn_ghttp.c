#include <ghttp.h>
int main(int argc, char *argv[])
{
    char *uri = "http://10.167.255.246/liwei.php";
    ghttp_request *request = NULL;
    ghttp_status status;
    char *buf;
    int bytes_read;
    
    request = ghttp_request_new();
    if(ghttp_set_uri(request, uri) == -1)
        exit(-1);
    if(ghttp_set_type(request, ghttp_type_get) == -1)
        exit(-1);
    ghttp_prepare(request);
    status = ghttp_process(request);
    if(status == ghttp_error)
        exit(-1);
    /* OK, done */
    printf("Status code -> %d\n", ghttp_status_code(request));
    buf = ghttp_get_body(request);
    printf("buf is %s\n",buf);
    bytes_read = ghttp_get_body_len(request);
    printf("buf len is %d\n", bytes_read);
    return 0;
}