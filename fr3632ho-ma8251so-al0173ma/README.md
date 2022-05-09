# # EDAF50 Project

## Contributors
- Max Soller
- Fredrik Horn Af Åminne Dannert
- Alexander Magnusson

## Makefiles and file structure

- `make` makes the library libclientserver.a and recursively builds the programs in `src`.
- The subdirectory `src/executables` contains the programs `myclient` and `myserver`.

## Building with make

- `make` in the directory builds the library (in
lib/libclientserver.a) and the programs
myclient and myserver (in the src/executables directory).
- `make install` makes the files and copies the executables to `bin`.
- For cleaning, use `make clean` and `make distclean`.

## Running the programs

To run the programs, open two terminal windows.

In the first one, start the server with `myserver <port>`, e.g.,

```
bin/myserver 7777
```

In the other one, start the client with `myclient <server> <port>`, e.g.,

```
bin/myclient localhost 7777
```