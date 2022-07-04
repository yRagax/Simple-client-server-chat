#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
int main()
{
	int yes = 1;
	int sockfd;

	struct sockaddr_in my_addr;

	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	memset(my_addr.sin_zero, '0', sizeof my_addr.sin_zero);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(9034);     // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY;


	bind(sockfd, (struct sockaddr*) & my_addr, sizeof my_addr);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt");
		//exit(1);
	}
	listen(sockfd, 5);
	struct sockaddr_in klientAdr;
	socklen_t addr_size = sizeof klientAdr;
	char bufor[1024];
	int ileZnak;
	int klient;
	fd_set master;
	fd_set read_fds;
	int fdmax;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	fdmax = sockfd;
	FD_SET(sockfd, &master);
	while (1)
	{
		read_fds = master;
		select(fdmax + 1, &read_fds, NULL, NULL, NULL);
		for (int i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds))
			{
				//nowosc
				if (i == sockfd)
				{
					//nowy uzytkownik polaczyl sie
					klient = accept(sockfd, (struct sockaddr*) & klientAdr, &addr_size);
					FD_SET(klient, &master); //wpisanie uzytkownika do listy
					if (klient > fdmax)
						fdmax = klient;
				}
				else
				{
					//nowa wiadomosc
					memset(bufor, '\0', sizeof(bufor));
					ileZnak = recv(i, bufor, sizeof(bufor), 0);
					if(!ileZnak)
					{
						//koniec polaczenia
						FD_CLR(i,&master);
						close(i);
					}
					for (int j = 0; j <= fdmax; j++)
					{
						//wysylam kazdemu oprocz socketa do sluchania i siebie samego
						if (j != sockfd && j != i)
						{
							send(j, bufor, sizeof(bufor), 0);
						}
					}
				}
			}
		}
	}
}
