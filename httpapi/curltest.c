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
    return size * nmemb;     //���뷵�������С, ����ֻ�ص�һ��, �����Ϊ��.
 }

 void *get_head_thread()
 {
    CURL *curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1/liwei.php"); //�������ص�URI
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);        //���ó�ʱ
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);        //���������ź�
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);          //�������ݰ���HTTPͷ��
    curl_easy_setopt(curl, CURLOPT_RANGE, "0-500");     //���ڶϵ�����, �������صķ�Ƭ
    
    char buffer[1000] = {0x0};
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_get_head); //�����������ݵĻص�����
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);  

    curl_easy_perform(curl);  
    curl_easy_cleanup(curl);

    //��ʱ��վHTTPͷ��Ϣ�Ѿ������buffer��.
	printf("buffer is %s\n",buffer);
	return 0;
 }

 int main()
 {
	 get_head_thread();
 }