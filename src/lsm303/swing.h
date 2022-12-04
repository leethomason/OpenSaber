/*
  Copyright (c) Lee Thomason, Grinning Lizard Software

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <stdint.h>
#include "./src/util/Grinliz_Util.h"

#define SWING_SAMPLES	12
#define FILTER_MAG_X	10
#define FILTER_MAG_Y	10
#define FILTER_MAG_Z	10
static const int SWING_MAX = 14;

/*
// The FILTER constants run an average over the accelerometer
// data. They are turned first, to get stable accelerometer
// readings when the saber is on or off.
#	define FILTER_MAG_X	10
#	define FILTER_MAG_Y	10
#	define FILTER_MAG_Z	10
// SWING_SAMPLES further filters the actual swing. It is 
// frustrating that the FILTER_ isn't enough. If this is
// too large, the swing sound will trail the motion. If too
// small, dead spots become annoying.
#	define SWING_SAMPLES 8
// The maximum rad/sec of the swing. Used for the sound envelope.
#	define SWING_MAX 16
*/
/*
The Swing algorithm translation rotation of the blade into sound level.
It is currently based on an LSM303 accel + mag chip. Some notes:

- An accel + gyro would be better; the output of a gyro is, after all,
  the number we are trying to get. But that takes another chip on the board
  and has it's own challenges, in terms of board layout and yield.
- The mag is *very* noisy, tuned above.
- The swing is still noisier than expected.
- There are dead spots in the mag reading - from local fields or sensor orientation?
*/

class Swing
{
public:
	Swing();

	void push(const Vec3<int32_t>& x, const Vec3<int32_t>& xMin, const Vec3<int32_t>& xMax);
	// speed in radians / second
	float speed() const { return m_speed; }

	// Where the swing "starts", probably any starting location is fine.
	// Maybe even idle times?
	void setOrigin() { m_init = false; }
	static bool test();

private:
	//Vec3<float> normalize(const Vec3<int32_t> x, const Vec3<int32_t>& x0, const Vec3<int32_t>& x1);

	bool m_init = false;
	float m_speed = 0;
	AverageSample<int32_t, SWING_SAMPLES> swingAve;
	AverageSample<Vec3<int32_t>, 4> inputAve;
};
