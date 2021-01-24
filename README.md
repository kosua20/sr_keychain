# sr_keychain

This is a simple wrapper around credential manager utilities on Windows, macOS and Linux. It respectively interacts with:

* the system credential manager on Windows
* the keychain on macOS
* the "Secret service" of your distribution on Linux (such as `gnome-keyring` or `ksecretservice`)

Only `sr_keychain.h` and `sr_keychain.c` are required. A basic example in C can be found in the `example` directory.

## Dependencies

* On Windows, this should link against `Advapi32`.
* On macOS, this should link against `Security.framework`.
* On Linux, this should link against the `libsecret-1` library.

## Usage

All strings passed as arguments below should be null-terminated, including the passwords.

### Storing a password

    int sr_keychain_set_password(const char * domain, char * user, const char * password);
Store a password in the keychain, for a given domain and user name. This will return 0 on success.

### Querying a password

    int sr_keychain_get_password(const char * domain, const char * user, char ** password);
Retrieve the password previously stored for a given domain and username. The string will be internally allocated and its ownership is transferred to the caller. This will return 0 on success.

### Helpers

    void sr_keychain_set_stdin_printback(int enable)
Disable printback when querying for user input on stdin, if the terminal honors it.

## Customization

`sr_keychain` internally allocate a few strings (including the retrieved password). By default, it uses the standard `malloc` and `free` but these can be overriden by defining `SR_KEYCHAIN_MALLOC(S)` and `SR_KEYCHAIN_FREE(S)`.

On Linux, a URL identifier is associated to all elements stored by `sr_keychain`. It defaults to `"com.sr.sr_keychain"`, but can be overriden by defining `SR_KEYCHAIN_LINUX_SCHEME_NAME` to the desired string.

## Limitations

This library currently does not allow for the deletion of existing passwords from the system keychain. It exposes no additional options such as the lifespan of a password, the associated protocol or application, etc. Finally, it doesn't protect the password once it is stored in memory in your application.