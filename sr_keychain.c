//#include <stdlib.h>
//#include <assert.h>
//#include <string.h>
//#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#endif

#ifdef __APPLE__
#include <Security/Security.h>
#endif

#ifdef _WIN32
#include <wincred.h>

wchar_t * get_complete_url(const char * domain, const char * user){
	const int length = strlen(user) + 1 + strlen(domain);
	char* domainAndUser = (char*)malloc(sizeof(char) * (length + 1));
	snprintf(domainAndUser, length, "%s@%s", user, domain);

	const int sizeWide = MultiByteToWideChar(CP_UTF8, 0, domainAndUser, -1, NULL, 0);
	wchar_t* targetName = (wchar_t*)malloc(sizeWide * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, domainAndUser, -1, targetName, sizeWide);
	free(domainAndUser);
	return targetName;
}
#endif

#ifdef __linux__
#include <libsecret/secret.h>

// Declare secret scheme.
const SecretSchema * get_secret_schema(){
	static SecretSchema schema;
	schema.name = "com.sr.sr_keychain";
	schema.flags = SECRET_SCHEMA_NONE;
	schema.attributes[0].name = "domain";
	schema.attributes[0].type = SECRET_SCHEMA_ATTRIBUTE_STRING;
	schema.attributes[1].name = "user";
	schema.attributes[1].type = SECRET_SCHEMA_ATTRIBUTE_STRING;
	schema.attributes[2].name = "NULL";
	schema.attributes[2].type = (SecretSchemaAttributeType)0;
	return &schema;
}
#endif

int sr_keychain_get_password(const char * domain, const char * user, char ** password){
#if defined(__APPLE__)
	UInt32 passLength;
	char * passBuffer;
	OSStatus stat = SecKeychainFindInternetPassword(NULL, strlen(domain), domain, 0, NULL, strlen(user), user, 0, NULL, 0, kSecProtocolTypeAny, kSecAuthenticationTypeDefault, &passLength, (void**)&passBuffer, NULL);
	if(stat == 0){
		*password = (char*)malloc(sizeof(char) * (passLength + 1));
		memcpy(*password, passBuffer, passLength);
		(*password)[passLength] = '\0';
		return 0;
	}
#elif defined(_WIN32)

	wchar_t* targetName = get_complete_url(domain, user);

	PCREDENTIALW credential;
	BOOL stat = CredReadW(targetName, CRED_TYPE_GENERIC, 0, &credential);
	free(targetName);

	if(stat){
		const int passLength = credential->CredentialBlobSize;
		*password = (char*)malloc(sizeof(char) * (passLength + 1));
		memcpy(*password, (const char *)credential->CredentialBlob, passLength);
		(*password)[passLength] = '\0';
		CredFree(credential);
		return 0;
	}
#elif defined(__linux__)
	GError* stat = NULL;
	gchar *passBuffer = secret_password_lookup_sync(get_secret_schema(), NULL, &stat,
		"domain", domain, "user", user, NULL);
	if(stat != NULL){
		g_error_free(stat);
		return 1;
	}
	if(passBuffer == NULL){
		return 1;
	}
	const int passLength = strlen(passBuffer);
	*password = (char*)malloc(sizeof(char) * (passLength + 1));
	memcpy(*password, passBuffer, passLength);
	(*password)[passLength] = '\0';
	secret_password_free(passBuffer);
	return 0;
#else
	#pragma message("Unsupported platform for sr_keychain.")
#endif
	return 1;
}

int sr_keychain_set_password(const char * domain, char * user, const char * password){
#if defined(__APPLE__)
	SecKeychainItemRef item;
	OSStatus stat = SecKeychainFindInternetPassword(NULL, strlen(domain), domain, 0, NULL, strlen(user), user, 0, NULL, 0, kSecProtocolTypeAny, kSecAuthenticationTypeDefault, 0, NULL, &item);
	if(stat == 0){
		// If the item already exists, modify it.
		stat = SecKeychainItemModifyAttributesAndData(item, NULL, strlen(password), password);
	} else {
		// Else create it.
		stat = SecKeychainAddInternetPassword(NULL, strlen(domain), domain, 0, NULL, strlen(user), user, 0, NULL, 0, kSecProtocolTypeAny, kSecAuthenticationTypeDefault, strlen(password), password, NULL);
	}
	return stat == 0 ? 0 : 1;
#elif defined(_WIN32)
	wchar_t* targetName = get_complete_url(domain, user);

	CREDENTIALW credsToAdd = {};
	credsToAdd.Flags = 0;
	credsToAdd.Type = CRED_TYPE_GENERIC;
	credsToAdd.TargetName = (LPWSTR)targetName;
	credsToAdd.CredentialBlob = (LPBYTE)password;
	credsToAdd.CredentialBlobSize = strlen(password);
	credsToAdd.Persist = CRED_PERSIST_LOCAL_MACHINE;
	// This will overwrite the credential if it already exists.
	BOOL stat = CredWrite(&credsToAdd, 0);
	free(targetName);
	return stat ? 0 : 1;
#elif defined(__linux__)
	GError* stat = NULL;
	secret_password_store_sync(get_secret_schema(), SECRET_COLLECTION_DEFAULT, "", password, NULL, &stat, "domain", domain, "user", user, NULL);
	if(stat != NULL){
		g_error_free(stat);
		return 1;
	}
	return 0;
#else
	#pragma message("Unsupported platform for sr_keychain.")
#endif
	return 1;
}

void sr_keychain_set_stdin_printback(int enable){
#ifdef _WIN32
	HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hstdin, &mode);
	if(enable != 0){
		mode |= ENABLE_ECHO_INPUT;
	} else {
		mode &= ~ENABLE_ECHO_INPUT;
	}
	SetConsoleMode(hstdin, mode);
#else
	struct termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	if(enable != 0){
		tty.c_lflag |= ECHO;
	} else {
		tty.c_lflag &= ~ECHO;
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}
