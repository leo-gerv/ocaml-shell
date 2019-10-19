<div align="center">
    <br>
    <a href="https://github.com/leo-gerv/ocaml-shell"><img src="https://raw.githubusercontent.com/leo-gerv/ocaml-shell/master/res/camel.png" alt="ocaml-shell" width="250"></a>
    <br>
    <h1>ocaml-shell</h1>
    <p align="center">Interactive shell for <b>OCAML</b></p>
    <br>
</div>

## How to build
```shell
$ mkdir build
$ qmake -o build/Makefile src/
$ cd build && make
```

## How it looks

![Ocaml Shell](screenshots/ocaml.jpg "Ocaml Shell")


## Project Architecture
```
. 
├── src             # C++ / Qt Source files
├── res             # Resources
├── screenshots     # Images for the README
└── bin             # Binaries of the project
```