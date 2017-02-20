/*
 * proxy.c - CS:APP Web proxy
 * 
 * IMPORTANT: Include your name, student number and email here
 * Zeng Heming, 5140219405, 972818442@qq.com
 *
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 */ 

#include "csapp.h"

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, int  *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);
void doit(int fd, struct sockaddr_in* sockaddr);
int connect_server(char *hostname, char *pathname, int port);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);
void read_requesthdrs(rio_t *rp);
void *thread(void *vargp);

sem_t w_sem;

struct arg{
    int connfd;
    struct sockaddr_storage sockaddr;
};

/* 
 * main - Main routine for the proxy program 
 */
int main(int argc, char **argv)
{

    int listenfd, port;
    char hostname[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    struct arg* connfdp;
    
    /* Check arguments */
    if (argc != 2) {
	fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
	exit(0);
    }

    /* Initialize semaphore */
    Sem_init(&w_sem, 0, 1);

    /* Create threads to accept requests */
    port = atoi(argv[1]);
    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
    connfdp = (struct arg*)Malloc(sizeof(struct arg));
	connfdp->connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
    connfdp->sockaddr = clientaddr;
    Pthread_create(&tid, NULL, thread, (void*)connfdp);
    }

    exit(0);
}


/*
 * parse_uri - URI parser
 * 
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, int *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) {
	hostname[0] = '\0';
	return -1;
    }
       
    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';
    
    /* Extract the port number */
    *port = 80; /* default */
    if (*hostend == ':')   
	*port = atoi(hostend + 1);
    
    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
	pathname[0] = '\0';
    }
    else {
	pathbegin++;	
	strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring. 
 * 
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, 
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /* 
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s %d", time_str, a, b, c, d, uri, size);
}

/*
 * doit - Execute the request from clients. 
 * 
 * The inputs are the socket address of the requesting client
 * (sockaddr) and the connection file descriptor for client
 */
void doit(int fd, struct sockaddr_in* sockaddr) 
{
    int request_status, port, clientfd, content_length;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], pathname[MAXLINE];
    rio_t rio;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))  
        return;
    sscanf(buf, "%s %s %s", method, uri, version);       
    if (strcasecmp(method, "GET")) {                     
        clienterror(fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    }                                                    
    read_requesthdrs(&rio);                              

    /* Parse URI from GET request */
    request_status = parse_uri(uri, hostname, pathname, &port);       
    
    if (request_status < 0) {                     
	clienterror(fd, uri, "404", "Not found",
		    "Invalid uri");
	return;
    }                                                    
    
    /*
     * Check whether the file was cached, if it wasn't cached, then 
     * connect the server and respond to client, if it was cached, 
     * go directly to the cache directory to fetch it
     */
    char file[50];
    sprintf(file, "cache/%s", pathname);
    int tp = open(file, O_RDONLY, 0);
    
    if(tp == -1){
        clientfd = connect_server(hostname, pathname, port);
        Rio_readinitb(&rio, clientfd);
    
        /* Create a new cache file */
        int cachefd = Open(file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    
        /* Deal with reponse header */
        content_length = 0;
        do{
            int length = rio_readlineb(&rio,buf,MAXLINE-1);
            Rio_writen(fd, buf, length);
            Rio_writen(cachefd, buf, length);
            if(strstr(buf, "Content-length")!=NULL)
                sscanf(buf, "Content-length: %d\r\n", &content_length);
        } while(strcmp(buf, "\r\n"));
    
        int temp = 0;
    
        /* Deal with reponse body */
        do{
            temp = Rio_readnb(&rio,buf,MAXLINE);
            Rio_writen(fd, buf, temp);
            Rio_writen(cachefd, buf, temp);
        } while(temp == MAXLINE);
        Close(cachefd);
    }
    else{
        /* Directly fetch files from cache directory */
        Rio_readinitb(&rio, tp);
        content_length = 0;
        do{
            int length = rio_readlineb(&rio,buf,MAXLINE-1);
            Rio_writen(fd, buf, length);
            if(strstr(buf, "Content-length")!=NULL)
                sscanf(buf, "Content-length: %d\r\n", &content_length);
        } while(strcmp(buf, "\r\n"));
       
            int temp = 0;
            do{
                temp = Rio_readnb(&rio,buf,MAXLINE);
                Rio_writen(fd, buf, temp);
            } while(temp == MAXLINE);
    }
    
    /* Complete log file */
    char logstring[50];
    format_log_entry(logstring, sockaddr, uri, content_length);
    
    P(&w_sem);
    printf("%s\n", logstring);
    FILE* logfile;
    logfile = fopen("proxy.log", "a+");
    fprintf(logfile, "%s\n", logstring);
    fclose(logfile);
    V(&w_sem);
    
    return;
}

/*
 * thread - The function that each thread executes. 
 */
void *thread(void *vargp)
{
    int connfd = ((struct arg*)vargp)->connfd;
    struct sockaddr_storage sockaddr = ((struct arg*)vargp)->sockaddr;
    Pthread_detach(pthread_self());
    Free(vargp);
    doit(connfd, (struct sockaddr_in*)&sockaddr);
    Close(connfd);
    return NULL;
}

/*
 * connect_server - Connect to the tiny server. 
 * 
 * The inputs are the hostname, pathname, and 
 *port of the request
 */
int connect_server(char *hostname, char *pathname, int port)
{
    char buf[MAXLINE];
    int clientfd = Open_clientfd(hostname, port);
    sprintf(buf, "GET /%s HTTP/1.0\r\n", pathname);
    Rio_writen(clientfd, buf, strlen(buf));
    sprintf(buf, "Host: %s\r\n",hostname);
    Rio_writen(clientfd, buf, strlen(buf));
    Rio_writen(clientfd, "\r\n", strlen("\r\n"));
    return clientfd;
}

/* This function reports the request is valid */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

/* This function is used to help parsing */
void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
	Rio_readlineb(rp, buf, MAXLINE);
	printf("%s", buf);
    }
    return;
}