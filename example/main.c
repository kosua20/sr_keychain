#include <sr_keychain.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_SIZE 1023

void trim_end_newline(char* str) {
	for(int i = 0; i < MAX_STR_SIZE; ++i) {
		if(str[i] == '\r' || str[i] == '\n') {
			str[i] = '\0';
			break;
		}
	}
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	char domain[MAX_STR_SIZE + 1];
	domain[MAX_STR_SIZE] = '\0';
	char user[MAX_STR_SIZE + 1];
	user[MAX_STR_SIZE] = '\0';
	char passwd[MAX_STR_SIZE + 1];
	passwd[MAX_STR_SIZE] = '\0';

	printf("Input domain:\n");
	char* res = fgets(domain, MAX_STR_SIZE, stdin);
	if(res == NULL) {
		return 1;
	}
	trim_end_newline(domain);

	printf("Input user:\n");
	res = fgets(user, MAX_STR_SIZE, stdin);
	if(res == NULL) {
		return 1;
	}
	trim_end_newline(user);

	printf("Input password:\n");
	sr_keychain_set_stdin_printback(0);
	res = fgets(passwd, MAX_STR_SIZE, stdin);
	if(res == NULL) {
		return 1;
	}
	trim_end_newline(passwd);
	sr_keychain_set_stdin_printback(1);

	printf("--------------------------------------\n");
	printf("Input summary:\n");
	printf("* Domain: \"%s\"\n", domain);
	printf("* User: \"%s\"\n", user);
	printf("* Password: \"%s\"\n", passwd);
	printf("--------------------------------------\n");

	printf("Writing to keychain...");
	const int res0 = sr_keychain_set_password(domain, user, passwd);
	if(res0 != 0) {
		printf("failure, exiting.\n");
		return 1;
	}
	printf("success.\n");

	printf("--------------------------------------\n");

	printf("Retrieving from keychain...");
	char* passwordResult = NULL;
	const int res1		 = sr_keychain_get_password(domain, user, &passwordResult);
	if(res1 != 0) {
		printf("failure, exiting.\n");
		return 1;
	}

	printf("success.\n");
	printf("Password is: \"%s\"\n", passwordResult);
	free(passwordResult);

	return 0;
}
