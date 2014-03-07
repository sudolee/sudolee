#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>

int main()
{
	struct hostent *h;
	char hostname[40];

	printf("enter URL: ");
	scanf("%s", hostname);
	getchar();
	if ((h = gethostbyname(hostname)) == NULL) {
		fprintf(stderr, "Can not match this IP.\n");
		exit(1);
	}
	printf("hostname: %s\n", h->h_name);
	printf("ip: %s\n", (char *)inet_ntoa(*((struct in_addr *)h->h_addr)));

	return EXIT_SUCCESS;
}
