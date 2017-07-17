/**
***************************************************************************
* @file fftTest.cc
* 
* Source file defining FFTTest class.
*
* Copyright (C) 2017 David LaRose, dlr@cs.cmu.edu
* See accompanying file, LICENSE.TXT, for details.
*
***************************************************************************
**/

#include <complex>

#include <brick/common/functional.hh>
#include <brick/numeric/fft.hh>
#include <brick/test/testFixture.hh>

namespace brick {

  namespace numeric {
    
    class FFTTest
      : public brick::test::TestFixture<FFTTest> {

    public:

      FFTTest();
      ~FFTTest() {}

      void setUp(const std::string& /* testName */) {}
      void tearDown(const std::string& /* testName */) {}

      // Tests.
      void testComputeFFT();
    
    private:

      

      double m_defaultTolerance;
      double m_relaxedTolerance;

    }; // class FFTTest


    /* ============== Member Function Definititions ============== */

    FFTTest::
    FFTTest()
      : brick::test::TestFixture<FFTTest>("FFTTest"),
        m_defaultTolerance(1.0E-11),
        m_relaxedTolerance(1.0E-5)
    {
      // Register all tests.
      BRICK_TEST_REGISTER_MEMBER(testComputeFFT);
    }


    void
    FFTTest::
    testComputeFFT()
    {
      double constexpr twoPi = brick::common::constants::twoPi;
      std::size_t constexpr signalLength = 4;

      // Define the fourier representation of a signal.  For now all
      // phases are zero.
      Array1D<double> referenceAmplitudes(signalLength);
      Array1D<double> referencePhases(signalLength);

      // for(std::size_t ii = 0; ii < signalLength; ++ii) {
      // referenceAmplitudes[ii] = (1.0 - double(ii) / signalLength);
      // }
      // referenceAmplitudes[signalLength / 2] = 1.0;
      referenceAmplitudes = 0.0;
      referenceAmplitudes[0] = 0.0;
      referenceAmplitudes[1] = 1.0;
      referencePhases = 0.0;

      // Compute a signal that has the fourier representation we just
      // defined.
      Array1D< std::complex<double> > inputSignal(signalLength);

      // For each element of the sequence.
      for(std::size_t ii = 0; ii < signalLength; ++ii) {
        inputSignal[ii] = std::complex<double>(0.0, 0.0);

        // For each component frequency.
        for(std::size_t jj = 0; jj < signalLength; ++jj) {
          // What's the frequency in radians per sample.
          double frequency = twoPi * double(jj) / signalLength;

          // And what's the amplitude of the sine wave.  Notice that
          // we're just Naively doing an inverse DFT here, so we need
          // a factor of 1/N to make the amplitudes work out.
          double amplitude = (referenceAmplitudes[jj] *
                              brick::common::cosine(ii * frequency));
          amplitude /= static_cast<double>(signalLength);

          // Fill in the relevant signal element.
          inputSignal[ii] += std::complex<double>(amplitude, 0.0);
        }
      }

      std::cout << "\nSignal: " << inputSignal << std::endl;
      
      // Now do the FFT.
      Array1D< std::complex<double> > fft = computeFFT(inputSignal);

      std::cout << "\nFFT: " << fft << std::endl;
      // Check that the result is correct.
      for(std::size_t ii = 0; ii < signalLength; ++ii) {
        BRICK_TEST_ASSERT(
          approximatelyEqual(fft[ii].real(), referenceAmplitudes[ii],
                             this->m_defaultTolerance));
        BRICK_TEST_ASSERT(
          approximatelyEqual(fft[ii].imag(), 0.0, 
                             this->m_defaultTolerance));
      }
    }
  
  } //  namespace numeric

} // namespace brick


#if 1

int main(int /* argc */, char** /* argv */)
{
  brick::numeric::FFTTest currentTest;
  bool result = currentTest.run();
  return (result ? 0 : 1);
}

#else

namespace {

  brick::numeric::FFTTest currentTest;

}

#endif