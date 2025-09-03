@import "../src/rngs/Rngs.chug";

adc => Rngs rngs => dac;

// TODO: replace adc with wav file

rngs.internalExciter(0);
rngs.polyphony(1);
rngs.model(3);
rngs.note(16);
rngs.structure(0.4);
rngs.brightness(0.5);
rngs.damping(0.0);
rngs.position(0.9);

// safety first
rngs.gain(0.5);

1::eon => now;
