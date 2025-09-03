@import "../src/rngs/Rngs.chug";

Rngs rngs => dac;

rngs.internalExciter(true);
rngs.note(12);
rngs.model(1);

while (150::ms => now) {
	rngs.structure(Math.random2f(0.0, 0.2));
	rngs.brightness(Math.random2f(0.1, 0.6));
	rngs.damping(Math.random2f(0.1, 0.6));
	rngs.position(Math.random2f(0.1, 0.6));
	rngs.strum();
}
