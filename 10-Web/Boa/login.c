#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, char *argv[])
{
    char *data;
    char name[50], pwd[20];
 
    printf("content-type:text/html;charset=gb2312\n\n");
    printf("<title>output</title>");
    printf("<h2>output</h2>");
    data = getenv("QUERY_STRING");
    if (data == NULL) {
        printf("<p>There is no input.</p>");
    } else {
        sscanf(data, "name=%[^&]&pwd=%s", name, pwd);
        printf("<p>name = %s</p>", name);
        printf("<p>pwd = %s</p>", pwd);
        printf("%s",data);
    }
    return 0;
}
