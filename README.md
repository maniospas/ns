# About
NS is an interpreted self-modifying programming framework in which variable scopes are objects. The framework is interpreted, but can call precompiled C++ code.

NSLang is a programming language that implements traditional programming expressions in NS, like conditions, loops, and input/output. Enable it with `run('nslang/all.ns')`.

**Author:** Emmanouil (Manios) Krasanakis<br>
**License:** Apache 2<br>
**Status:** The core language interpreter is still being developed.

Documentation: https://maniospas.github.io/ns


# Download (windows)
Download the interpreter executable `ns.exe` and add it to your path. 
Run a source code file `file.ns` with the following command line instruction.

```
ns file.ns
```

If you do not provide a file or run the interpreter by double-clicking it, 
`cli.ns` will be interpreted (place this and the required `nslang` folder alongside the executable). 
This is a command line interface for NSLang.