
// Install a set of trap-patches for the struct-of-function pointers off
// of `pd`.  Handy for breakpointing, or adding logging to see when the
// system (or game) calls in to them.
//
void installSpies(void);

// Restore the trap-patches that got made.  You'll need to do this on
// a kEventTerminate, which gets called when `open`ing a new `.pdx` file
// in the simulator.
//
void uninstallSpies(void);
