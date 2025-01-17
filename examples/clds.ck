// @import "CldsP.chug";
@import "../src/cldsp/CldsP.chug";
// @import "Clds.chug";
@import "../src/clds/Clds.chug";
@import "../../../chuck/shared/touchpad.ck";
@import "../../../chuck/shared/console.ck";
@import "../../../chuck/shared/recorder.ck";
@import "../../../chuck/mutable-instruments/simple-loop.ck";

SimpleLoop buf;
// SimpleLoop buf => dac.right;

// buf => Clds c1 => Gain g(8.0) => dac;
// buf => Clds c1 => Gain g(8.0) => dac;
buf => CldsP c1 => Gain g(1) => dac.left;
buf => CldsP c2 => Gain g2(2.8) => dac.left;

// buf => dac.right;

// dyno.attackTime(0::ms);
// dyno.thresh(0.8);
// dyno.compress();
// // dyno.slopeAbove(0.5);
// // dyno.slopeBelow(1.0);
// // dyno.thresh(0.1);
// // dyno.attackTime(0::ms);

// Recorder rec;
// rec.init("temp", g);
// rec.start();

// buf => Gain g(0.1) => dac.left;
// buf => Clds c2;

(12.0) => c1.pitch;
(-12.0) => c2.pitch;

// 0.5 => c1.gain;

// (-0.3) => c1.density;
0.76 => c1.density => c2.density;
0.80 => c1.texture => c2.texture;

1.0 => c1.size => c2.size;
0.0 => c1.position => c2.position;

1 => c1.reverse;

"stretch" => c1.mode;

fun void toggleReverse() {
	while (true) {
		4::second => now;
		!c1.reverse() => c1.reverse;
		<<< "c1.reverse:", c1.reverse() >>>;
	}
}
spork ~toggleReverse();

// fun void setTimeout(dur timeout) {
// 	while (true) {
// 		true => c1.freeze;
// 		<<< "freeze:", c1.freeze() >>>;
// 		timeout* 3 => now;
// 		false => c1.freeze;
// 		150::ms => now;
// 	}
// }
// spork ~setTimeout(2::second);

<<< "c1.mode:", c1.mode() >>>;

// 1.0 => c1.position;
// 1.0 => c1.size;
// 1.0 => c1.density;
// 1.0 => c1.texture;
// (.04) => c1.pitch;
// 1.5 => c1.gain;
0.0 => c1.spread;

// while (true)

// position: 0.000106
// size: 0.320982
// density: 0.839403
// texture: 0.854542
// gain: 0.500000
// pitch: 12.195640
// 0.106 => c1.position;
// 0.320982 => c1.size;
// 0.839403 => c1.density;
// 0.854542 => c1.texture;
// // 0.500000 => c1.gain;
// 12.195640 => c1.pitch;
//
// 0.106 => c2.position;
// 0.320982 => c2.size;
// 0.839403 => c2.density;
// 0.854542 => c2.texture;
// 1.500000 => c1.gain;
// (-12.195640) => c2.pitch;
//
// while (2::second => now) {
// 	!c1.freeze() => c1.freeze;
// }

Touchpad.listen("MultiTouch Device") @=> TouchpadEvent event;

5 => int sliderCount;

1.0 / sliderCount => float sliderWidth;
<<< "sliderWidth:", sliderWidth >>>;

// fun void stop() {
// 	4::second => now;
// 	rec.stop();
// }
// spork ~stop();

// 0.5 => c1.pitch;
// 4::second => now;
// 0.25 => c1.pitch;
// 4::second => now;

// fun void iterateMode() {
// 	while (true) {
// 		4::second => now;
// 		<<< "c1.mode:", c1.mode() >>>;
// 		(c1.mode() + 1) % 6 => c1.mode;
// 	}
// }
// spork ~iterateMode();

while (event => now) {
	(event.x / sliderWidth) $ int => int sliderId;

	// event.y => c1.density;
	// event.y => c2.density;

	if (sliderId == 0) {
		event.y => c1.position => c2.position;
	} else if (sliderId == 1) {
		event.y => c1.size => c2.size;
	} else if (sliderId == 2) {
		event.y => c1.density => c2.density;
	} else if (sliderId == 3) {
		event.y => c1.texture => c2.texture;
	} else if (sliderId == 4) {
		(event.y * 48.0) - 24 => c1.pitch;
		c1.pitch() * (-1.0) => c2.pitch;
		// (event.y * -24.0) => c2.pitch;
	}

	<<< "-------------------------------------:" >>>;
	<<< "position:", c1.position() >>>;
	<<< "size:", c1.size() >>>;
	<<< "density:", c1.density() >>>;
	<<< "texture:", c1.texture() >>>;
	<<< "gain:", c1.gain() >>>;
	<<< "pitch:", c1.pitch() >>>;
	// // <<< event.which, sliderId, event.y >>>;
}
