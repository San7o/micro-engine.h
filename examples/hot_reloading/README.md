# hot reloading

You can use `micro-module.h` for hot reloading, read its documentation
for more information.

## Usage

Build shared library and main loop, then run the application:

```
make
./hot_reloading
```

Then you can change anything in  `app_module.c`, then recompile
and hit `R` to reload the shared library:

```
make
# Hit `R`
```

You will see that the application updates itself without closing.
