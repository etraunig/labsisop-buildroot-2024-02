#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFLEN	1024	//Max length of buffer
#define PORT	8000	//The port on which to listen for incoming data


char http_ok[] = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\nServer: Test\r\n\r\n";
char http_error[] = "HTTP/1.0 400 Bad Request\r\nContent-type: text/html\r\nServer: Test\r\n\r\n";
char page[] = "<html>\n<head>\n<title>\nTest page\n</title>\n</head>\n<body>\n<p>%s</p>\n</body>\n</html>\n";

void die(char *s)
{
	perror(s);
	exit(1);
}

int main(void)
{
	struct sockaddr_in si_me, si_other;

	int s, i, slen = sizeof(si_other) , recv_len, conn;
	char buf[BUFLEN];
	char line[256];
	FILE *searched_file;
	char html_buffer[8192];
	char page_buffer[1];

	/* create a TCP socket */
	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		die("socket");

	/* zero out the structure */
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind socket to port */
	if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
		die("bind");
	
	/* allow 10 requests to queue up */
	if (listen(s, 10) == -1)
		die("listen");

	/* keep listening for data */
	while (1) {
		memset(buf, 0, sizeof(buf));
		printf("Waiting a connection...");
		fflush(stdout);
		
		conn = accept(s, (struct sockaddr *) &si_other, &slen);
		if (conn < 0)
			die("accept");

		printf("Client connected: %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

		memset(html_buffer, 0, sizeof(html_buffer));

		printf("p1\n");
		// processa as informacoes da maquina target

		//model name e frequencia do processador
		searched_file = fopen("/proc/cpuinfo", "r");
		if (searched_file != 0) {
		  while (fgets(line, sizeof(line), searched_file)){
			if (strstr(line, "model name")){
			   strcat(html_buffer, line);
			   strcat(html_buffer, "<br>");		
			}
			else if (strstr(line, "cpu MHz")){
				strcat(html_buffer, line);
				break;
			}
				
		   }
		}

		fclose(searched_file);

		printf("p2\n");
		
		//uptime
		searched_file = fopen("/proc/uptime", "r");
		printf("p2.1\n");
		if (searched_file != 0){
			fgets(line, sizeof(line), searched_file);
			printf("p2.2\n");
			strtok(line, " ");
			strcat(html_buffer, "<br>");
			strcat(html_buffer, "Uptime em segundos: ");
			strcat(html_buffer, line);
		}
		fclose(searched_file);

		//versao do sistema
		searched_file = fopen("/proc/version", "r");
		if (searched_file != 0){
			while(fgets(line, sizeof(line), searched_file)){
				if (strstr(line, "version")){
				  strcat(html_buffer, "<br>");
				  strcat(html_buffer, "Versao do sistema: ");
			      strcat(html_buffer, line);
				  strcat(html_buffer, "<br>");
				  break;	
				}
			}
		}

		fclose(searched_file);

		// printf("p4\n");
		// //data e hora do sistema
		// searched_file = fopen("/proc/driver/rtc", "r");
		// if (searched_file != 0){
		// 	strcat(html_buffer, "<br>");
		// 	fgets(line, sizeof(line), searched_file);
		// 	strcat(html_buffer, line);
		// 	strcat(html_buffer, "<br>");
		// 	fgets(line, sizeof(line), searched_file);
		// 	strcat(html_buffer, line);
		// 	strcat(html_buffer, "<br>");
		// }

		// fclose(searched_file);

		//tabela de roteamento
		searched_file = fopen("/proc/net/route", "r");
		strcat(html_buffer, "Tabela de roteamento:");	
		strcat(html_buffer, "<br>");
		if (searched_file != 0){
			while(fgets(line, sizeof(line), searched_file)){
				strcat(html_buffer, line);
				strcat(html_buffer, "<br>");
			}
		}

		fclose(searched_file);

		//discos
		searched_file = fopen("/proc/partitions", "r");
		strcat(html_buffer, "Lista de unidades de disco:");
		strcat(html_buffer, "<br>");
		if (searched_file != 0){
			while(fgets(line, sizeof(line), searched_file)){
				strcat(html_buffer, line);
				strcat(html_buffer, "<br>");
			}
		}

		fclose(searched_file);

		//lista de dispostivos reconhecidos
		searched_file = fopen("/proc/bus/input/devices", "r");
		strcat(html_buffer, "Lista de dispositivos reconhecidos:");
		strcat(html_buffer, "<br>");
		if (searched_file != 0){
			while(fgets(line, sizeof(line), searched_file)){
				if (line[0] == 'I'){
				  strcat(html_buffer, line);
				  strcat(html_buffer, "<br>");
				}
				else if (line[0] == 'N'){
				  strcat(html_buffer, line);
				  strcat(html_buffer, "<br>");
				}
			}
		}

		fclose(searched_file);

		/* try to receive some data, this is a blocking call */
		recv_len = read(conn, buf, BUFLEN);
		if (recv_len < 0)
			die("read");

		/* print details of the client/peer and the data received */
		printf("Data: %s\n" , buf);

		if (strstr(buf, "GET")) {
			/* now reply the client with the same data */
			if (write(conn, http_ok, strlen(http_ok)) < 0)
	 			die("write");
			if (write(conn, html_buffer, strlen(html_buffer)) < 0)
				die("write");
		} else {
			if (write(conn, http_error, strlen(http_error)) < 0)
				die("write");
		}

		/* close the connection */
		close(conn);
	}
	close(s);
	
	return 0;
}