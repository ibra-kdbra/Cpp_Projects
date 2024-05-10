## Building

```sh
meson setup build/
meson compile -C build/
```

## Usage

```sh
abidump - Dump C++ Itanium ABI from ELF objects

USAGE:
    abidump <file> <subcommand> [options]

SUBCOMMANDS:
    symbols [filter] [--show-mangled]
        Dumps debug symbols. [filter] is an optional
        regex filter. If [--show-mangled] is set, the
        mangled symbol name will also be printed.
    vtables [filter]
       Dumps virtual function tables. [filter] is an
       optional regex filter
```

**Example - Dumping virtual function tables:**

```sh
$ abidump tf/bin/server_srv.so vtables CTFGameMovement
VTable for CTFGameMovement:
    [0] CTFGameMovement::~CTFGameMovement()
    [1] CTFGameMovement::ProcessMovement(CBasePlayer*, CMoveData*)
    [2] CGameMovement::StartTrackPredictionErrors(CBasePlayer*)
    [3] CGameMovement::FinishTrackPredictionErrors(CBasePlayer*)
    [4] CGameMovement::DiffPrint(char const*, ...)
    [5] CGameMovement::GetPlayerMins(bool) const
    [6] CGameMovement::GetPlayerMaxs(bool) const
    [7] CTFGameMovement::GetPlayerViewOffset(bool) const
    [...]
```

## License

[MIT](./LICENSE)
