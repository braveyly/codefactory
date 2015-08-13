#include <stdio.h>
#include <string.h>
#include "curl/curl.h"

size_t callback_get_head(void *ptr, size_t size, size_t nmemb, void *userp)
 {
    strcat( (char*)userp, (char*)ptr);
	printf("ptr is %s\n",ptr);
	printf("size is %d\n", size);
	printf("nmemb is %d\n",nmemb);
	printf("==========\n");
    return size * nmemb;     //必须返回这个大小, 否则只回调一次, 不清楚为何.
 }

 void *get_head_thread()
 {
    CURL *curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1/liwei.php"); //设置下载的URI
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);        //设置超时
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);        //屏蔽其它信号
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);          //下载数据包括HTTP头部
    curl_easy_setopt(curl, CURLOPT_RANGE, "0-500");     //用于断点续传, 设置下载的分片
    
    char buffer[1000] = {0x0};
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_get_head); //设置下载数据的回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);  

    curl_easy_perform(curl);  
    curl_easy_cleanup(curl);

    //此时网站HTTP头信息已经存放在buffer内.
	printf("buffer is %s\n",buffer);
	return 0;
 }

 int main()
 {
	 get_head_thread();
 }