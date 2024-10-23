adc => Rings rings => dac;

rings.internalExciter(0);
rings.polyphony(1);
rings.model(3);
rings.note(16);
rings.structure(0.4);
rings.brightness(0.5);
rings.damping(0.0);
rings.position(0.9);

rings.gain(0.5);  // safety first

1::eon => now;
