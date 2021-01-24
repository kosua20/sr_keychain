#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int sr_keychain_get_password(const char * domain, const char * user, char ** password);
	
int sr_keychain_set_password(const char * domain, char * user, const char * password);

void sr_keychain_set_stdin_printback(int enable);

#ifdef __cplusplus
}
#endif
