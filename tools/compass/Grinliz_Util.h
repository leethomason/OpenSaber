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

#ifndef GRINLIZ_UTIL_INCLUDED
#define GRINLIZ_UTIL_INCLUDED

#include <string.h>
#include <stdint.h>

/**
 * Power changes over time, and higher
 * draw changes the power. A small class
 * to average out power changes.
 */
template<typename TYPE, typename SUMTYPE, int N>
class AverageSample
{
public:
    AverageSample(TYPE initialValue) {
        for (int i = 0; i < N; ++i) {
			m_sample[i] = initialValue;
			m_total += initialValue;
		}
        m_average = initialValue;
    }

    AverageSample() {
        for (int i = 0; i < N; ++i) {
			m_sample[i] = 0;
		}
		m_total = 0;
		m_average = 0;
	}

	void recount() {
		m_total = 0;
		for (int i = 0; i < N; ++i)
			m_total += m_sample[i];
	}

    void push(TYPE value) {
		m_total -= m_sample[m_pos];
        m_sample[m_pos] = value;
		m_total += value;

        m_pos++;
        if (m_pos == N) {
			m_pos = 0;
			recount();
		}
		m_average = m_total / N;
    }

	void fill(TYPE value) {
		for(int i=0; i<N; ++i)
			push(value);
	}

    TYPE average() const { return m_average; }
    int numSamples() const { return N; }
	bool origin() const { return m_pos == 0; }

private:
    SUMTYPE m_average;
	SUMTYPE m_total = 0;
    int m_pos = 0;
    TYPE m_sample[N];
};


#endif // GRINLIZ_UTIL_INCLUDED

