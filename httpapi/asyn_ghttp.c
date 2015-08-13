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
    /* NOTE: Set async request */  
    ghttp_set_sync(request, ghttp_async);  
    ghttp_prepare(request);  
      
    while(1) {  
        status = ghttp_process(request);  
        printf( "status is %d\n", status );
	if(status == ghttp_error)  
            break;  
        /* NOTE: buf may NULL, notice it */  
        buf = ghttp_get_body(request);  
        printf( "buf is %s\n", buf );
        bytes_read = ghttp_get_body_len(request);
        printf( "bytes_read is %d\n", bytes_read ) ; 
        if(status == ghttp_done) {  
            /* NOTE: Ok, done */  
            break;  
        }  
    }  
    return 0;  
} 