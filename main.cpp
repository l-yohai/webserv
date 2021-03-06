#include <iostream>
#include <vector>
#include <algorithm>

#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define PORTNUM 80
#define MAXLINE 1024
#define HEADERSIZE 1024

#define SERVER "myserver/1.0 (Linux)"

struct user_request
{
	char method[20];   // 요청 방법
	char page[255];    // 페이지 이름
	char http_ver[80]; // HTTP 프로토콜 버전 
};

char root[MAXLINE];

int webserv(int sockfd);
int protocol_parser(char *str, struct user_request *request);
int sendpage(int sockfd, char *filename, char *http_ver, char *codemsg);

int main(int argc, char **argv)
{

	int server_socket;
	int client_socket;
	socklen_t client_len;
	int optval = 1;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	fd_set read_fds;
	fd_set write_fds;
	fd_set exception_fds;
	int fd_max;

	struct timeval timeout;

	if (argc !=2 )
	{
		std::cerr << "Argument Error" << std::endl;
		return (1);
	}

	if ((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		std::cerr << "Socket Error" << std::endl;
		return 1;
	}

	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	memset(&server_address, 0x00, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	// server_address.sin_port = htons(PORTNUM);
	server_address.sin_port = htons(stoi(std::string(argv[1])));

	if (bind(server_socket, reinterpret_cast<struct sockaddr *>(&server_address), static_cast<socklen_t>(sizeof(server_address))))
	{
		std::cerr<<"Bind error"<<std::endl;
		return 1;
	}

	if (listen(server_socket, 15) == -1)
	{
		//error처리
		std::cerr<<"Listen error"<<std::endl;
	}

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&exception_fds);

	FD_SET(server_socket, &read_fds);
	FD_SET(server_socket, &write_fds);
	FD_SET(server_socket, &exception_fds);

	fd_max = server_socket;

	while(1)
	{

		int fd;

		fd_set tmp;

		timeout.tv_sec = 5;
		timeout.tv_usec = 5;

		if ((select(fd_max + 1), &tmp, 0, 0, &timeout) == -1)
			std::cerr << "Select Error" << std::endl;

		for (fd = 0; fd < fd_max + 1; fd++)
		{
			if (FD_ISSET(fd, &tmp))
			{
				if (fd == server_socket)
				{
					client_len = sizeof(client_address);
					if ((client_socket = accept(server_socket, reinterpret_cast<struct sockaddr *>(&client_address), reinterpret_cast<socklen_t *>(&client_len))) == -1)
						std::cerr << "Accept Error" << std::endl;
					FD_SET(client_socket, &read_fds);
					if (fd_max < client_socket)
						fd_max = client_socket;
					std::cout << "Client Connect FD = " << client_socket << std::endl;
				}
				else
				{
					if ((len = read(fd, buf, BUFFER_SIZE)) < 0)
						std::cerr << "Read Error" << std::endl;
					if (len == 0)
					{
						FD_CLR(fd, &read_fds);
						close(fd);
						std::cout << "Client Disconnect: " << fd << std::endl;
					}
					else
						write(fd, buf, len);
				}
			}
		}




		client_len = sizeof(client_len);
		client_socket = accept(server_socket , (struct sockaddr *)&client_address, &client_len);
		if (client_socket == -1)
		{
			return 1;
		}
		// pid = fork();
		// if(pid == 0)
		// {
		// 	webserv(client_socket);
		// 	close(client_socket);
		// 	exit(0);
		// }
		// if(pid == -1)
		// {
		// 	return 1;
		// }
		// close(client_socket);
	}
}

int webserv(int sockfd)
{
	char buf[MAXLINE];
	char page[MAXLINE];
	struct user_request request;

	memset(&request, 0x00, sizeof(request));
	memset(buf, 0x00, MAXLINE);

	if(read(sockfd, buf, MAXLINE) <= 0)
	{
		return -1;
	}
	protocol_parser(buf, &request);
	if(strcmp(request.method, "GET") == 0)
	{
		sprintf(page, "%s%s", root, request.page);
		if(access(page, R_OK) != 0)
		{
			sendpage(sockfd, NULL, request.http_ver, "404 Not Found");
		}
		else
		{
			sendpage(sockfd, page, request.http_ver, "200 OK");
		}
	}
	else
	{
		sendpage(sockfd, NULL, request.http_ver, "500 Internal Server Error");
	}
	return 1;
}

int sendpage(int sockfd, char *filename, char *http_ver, char *codemsg)
{
	struct tm *tm_ptr;
	time_t the_time;
	struct stat fstat;
	char header[HEADERSIZE];
	int fd; 
	int readn;
	int content_length=0;
	char buf[MAXLINE];
	char date_str[80];
	char *daytable = "Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0";
	char *montable = "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec\0";

	memset(header, 0x00, HEADERSIZE);

	time(&the_time);
	tm_ptr = localtime(&the_time);

	sprintf(date_str, "%s, %d %s %d %02d:%02d:%02d GMT",
			daytable+(tm_ptr->tm_wday*4),
			tm_ptr->tm_mday,
			montable+((tm_ptr->tm_mon)*4),
			tm_ptr->tm_year+1900,
			tm_ptr->tm_hour,
			tm_ptr->tm_min,
			tm_ptr->tm_sec
			);

	if(filename != NULL)
	{   
		stat(filename, &fstat);
		content_length = (int)fstat.st_size;
	}   
	else	
	{   
		content_length = strlen(codemsg);
	}
	sprintf(header, "%s %s\nDate: %s\nServer: %s\nContent-Length: %d\nConnection: close\nContent-Type: text/html; charset=UTF8\n\n",
		http_ver, date_str, codemsg, SERVER, content_length);
	write(sockfd, header, strlen(header));

	if(filename != NULL)
	{
		fd = open(filename, O_RDONLY);
		memset(buf, 0x00, MAXLINE);
		while((readn = read(fd,buf,MAXLINE)) > 0)
		{
			write(sockfd, buf, readn);
		}
		close(fd);
	}
	else
	{
		write(sockfd, codemsg, strlen(codemsg));
	}

	return 1;
}

int protocol_parser(char *str, struct user_request *request)
{
	char *tr;
	char token[] = " \r\n";
	int i;
	tr = strtok(str, token);
	for (i = 0; i < 3; i++)
	{
		if(tr == NULL) break;
		if(i == 0)
			strcpy(request->method, tr);
		else if(i == 1)
			strcpy(request->page, tr);
		else if(i == 2)
			strcpy(request->http_ver, tr);
		tr = strtok(NULL, token);
	}
	return 1;
}
