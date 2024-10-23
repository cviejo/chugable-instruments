# Chugable Instruments

mutable-instruments for chuck. Currently includes:

-  Clouds (Clds)
-  Clouds Parasites (Cldsp)
-  Rings (Rngs)

## Installation

```bash
git clone https://github.com/cviejo/chugable-instruments.git
cd chugable-instruments
git submodule update --init --recursive
make mac
make install
```

By default, this will install the Chugins to `/usr/local/lib/chuck`. You can specify a different destination by setting the `DEST` variable:

```bash
DEST=/path/to/chuck/chugins make install
```

Note: You may need to use `sudo` for the installation step if you don't have write permissions to the destination directory.

## Usage

```chuck
TODO
```

See examples in the `examples` directory for more detailed usage.

## Contributing

Yes please! I'm mostly adding the modules and platforms I use, feel free to open a PR to add more. See the current modules for reference.
