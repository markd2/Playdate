
// Install a set of trap-patches for the struct-of-function pointers off
// of `pd`.  Handy for breakpointing, or adding logging to see when the
// system (or game) calls in to them.
//
void installSpies(void);
void uninstallSpies(void);
