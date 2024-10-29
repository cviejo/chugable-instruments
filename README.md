# Chugable Instruments

mutable-instruments for chuck. Currently includes:

-  Clouds (Clds)
-  Clouds Parasites (CldsP)
-  OnsetDetector (OnsetDetector)
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
adc => Rngs rngs => dac;

rngs.internalExciter(0);
rngs.polyphony(1);
rngs.model(3);
rngs.note(16);
rngs.structure(0.4);
rngs.brightness(0.5);
rngs.damping(0.0);
rngs.position(0.9);

rngs.gain(0.5);  // safety first

1::eon => now;
```

See also the `examples` directory.

## Contributing

Yes please! I'm mostly adding the modules and platforms I use, feel free to open a PR to add more. See the current modules for reference.
