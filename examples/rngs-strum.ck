Rings rings => dac;

rings.internalExciter(true);
rings.note(12);
rings.model(1);

while (150::ms => now) {
	rings.structure(Math.random2f(0.0, 0.2));
	rings.brightness(Math.random2f(0.1, 0.6));
	rings.damping(Math.random2f(0.1, 0.6));
	rings.position(Math.random2f(0.1, 0.6));
	rings.strum();
}
