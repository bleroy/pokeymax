package builder.resid.resid;

/**
 * Filter for 8580 chip based on simple linear approximation of the FC control.
 *
 * This is like the original reSID filter except the phase of BP output has been
 * inverted. I saw samplings on the internet that indicated it would genuinely
 * happen like this.
 *
 * @author Ken Händel
 * @author Dag Lem
 * @author Antti Lankila
 */
public class Filter8580 extends Filter {
	float Vlp, Vbp, Vhp;
	float ve, w0, _1_div_Q;
	private double highFreq = 12500;

	protected Filter8580() {
		super();
	}

	@Override
	protected final int clock(int voice1, int voice2, int voice3) {
		voice1 >>= 7;
		voice2 >>= 7;
		voice3 >>= 7;

		int Vi = 0;
		float Vo = 0;
		if (filt1) {
			Vi += voice1;
		} else {
			Vo += voice1;
		}
		if (filt2) {
			Vi += voice2;
		} else {
			Vo += voice2;
		}
		// NB! Voice 3 is not silenced by voice3off if it is routed
		// through the filter.
		if (filt3) {
			Vi += voice3;
		} else if (!voice3off) {
			Vo += voice3;
		}
		if (filtE) {
			Vi += ve;
		} else {
			Vo += ve;
		}

		float dVbp = w0 * Vhp;
		float dVlp = w0 * Vbp;
		Vbp -= dVbp;
		Vlp -= dVlp;
		Vhp = Vbp * _1_div_Q - Vlp - Vi + (float) Math.random();

		if (lp) {
			Vo += Vlp;
		}
		if (bp) {
			Vo += Vbp;
		}
		if (hp) {
			Vo += Vhp;
		}

		return (int) (Vo * vol) >> 4;
	}

	@Override
	protected final void zeroDenormals() {
		/* We only need this for systems that don't do -msse and -mfpmath=sse */
		if (Vbp > -1e-12f && Vbp < 1e-12f) {
			Vbp = 0;
		}
		if (Vlp > -1e-12f && Vlp < 1e-12f) {
			Vlp = 0;
		}
		if (Vhp > -1e-12f && Vhp < 1e-12f) {
			Vhp = 0;
		}
	}

	@Override
	protected void updatedCenterFrequency() {
		w0 = (float) (2 * Math.PI * highFreq * fc / 2047 / 1e6);
	}

	@Override
	protected void updatedResonance() {
		_1_div_Q = 1f / (0.707f + res / 15f);
	}

	@Override
	protected void input(int input) {
		ve = input << 4;
	}

	@Override
	protected void updatedMixing() {
	}

	public void setFilterCurve(double filter8580CurvePosition) {
		highFreq = filter8580CurvePosition;
	}
}
