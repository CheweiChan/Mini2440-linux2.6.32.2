通過網路頁面輸入用戶名和密碼,網頁再透過server獲取用戶名和密碼.

靜態網頁為loginTest.html(192.168.2.230/loginTest)
按下按鈕login後由login.cgi處理後再顯示網頁


修改boa-0.94.13文件夹下的boa.conf，放入開發板etc/boa（此目录在src下difines.h文件中指定#define SERVER_ROOT "/etc/boa"）
Port 80
User 默认nobody，改为root
Group 默认nogroup，改为root
ErrorLog 默认/var/log/boa/error_log，需要手动建立/var/log/boa目录
AccessLog 默认/var/log/boa/access_log，需要手动建立/var/log/boa目录
ServerName friendly-arm
DocumentRoot 默认/var/www，需手动建立目录本範例修改到/www(輸入192.168.2.230會連到/www/index.html)
DirectoryIndex 默认index.html
MimeTypes 默认/etc/mime.types，需将虚拟机中该文件拷贝至目标机相同路径下(复制ubuntu /etc/mime.type文件到目标板/etc/boa下)
ScriptAlias 默认/cgi-bin/ /usr/lib/cgi-bin/，需手动建立目录，修改至/cgi-bin/ /var/boa/cgi-bin(输入站点和CGI脚本位置)
KeepAliveMax 1000       //一个连接所允许的HTTP持续作用请求最大数目
KeepAliveTimeout 10     //HTTP持续作用中服务器在两次请求之间等待的时间数，以秒为单位
DefaultType text/plain  //文件扩展名没有或未知的话，使用的缺省MIME类型
CGIPath /bin            //提供CGI程序的PATH环境变量值
Alias /doc /usr/doc     //为路径加上别名

用户可以根据自己需要，对boa.conf进行修改，但必须要保证其他的辅助文件和设置必须和boa.conf里的配置相符，不然Boa就不能正常工作。 在上面的例子中，我们还需要创建日志文件所在目录/var/log/boa，创建HTML文档的主目录/var/www。
将mime.types、inittab和passwd文件从任何一个linux系统中的/etc目录下拷贝到开发板文件系统的/etc目录下，创建CGI脚本所在目录/var/www/cgi-bin/。mime.types文件用来指明不同文件扩展名对应的MIME类型，一般可以直接从Linux主机上拷贝一个，大部分也都是在主机的/etc目录下。
