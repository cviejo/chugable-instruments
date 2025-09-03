@import "../src/onset-detector/OnsetDetector.chug";

SndBuf buffer("./examples/1.wav");

buffer => OnsetDetector onset => blackhole;

fun void loop() {
	while (buffer.samples()::samp => now) {
		buffer.pos(0);
		buffer.play(1);
	}
}
spork ~loop();

// detect using the event
fun void event() {
	while (onset.event => now) {
		<<< "onset event" >>>;
	}
}
spork ~event();

// detect using the sample
while (1::samp => now) {
	if (onset.last() != 0.0) {
		<<< "onset sample" >>>;
	}
}
