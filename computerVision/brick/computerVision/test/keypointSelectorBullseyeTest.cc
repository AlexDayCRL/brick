/**
***************************************************************************
* @file keypointSelectorBullseyeTest.cpp
*
* Source file defining tests for the KeypointSelectorBullseye class.
*
* Copyright (C) 2013 David LaRose, dlr@cs.cmu.edu
* See accompanying file, LICENSE.TXT, for details.
*
***************************************************************************
**/

#include <brick/computerVision/imageIO.hh>
#include <brick/computerVision/kernels.hh>
#include <brick/computerVision/keypointSelectorBullseye.hh>
#include <brick/computerVision/utilities.hh>
#include <brick/computerVision/test/testImages.hh>

#include <brick/random/pseudoRandom.hh>
#include <brick/test/functors.hh>
#include <brick/test/testFixture.hh>

#include <brick/utilities/path.hh>
#include <brick/utilities/stringManipulation.hh>
#include <brick/utilities/timeUtilities.hh>

namespace brick {

  namespace computerVision {

    
    // This test is copied 
    class KeypointSelectorBullseyeTest
      : public brick::test::TestFixture<KeypointSelectorBullseyeTest> {

    public:

      KeypointSelectorBullseyeTest();
      ~KeypointSelectorBullseyeTest() {}

      void setUp(const std::string& /* testName */) {}
      void tearDown(const std::string& /* testName */) {}

      // Tests.
      void testKeypointSelectorBullseye();

    private:

      double m_defaultTolerance;
      
    }; // class KeypointSelectorBullseyeTest


    /* ============== Member Function Definititions ============== */

    KeypointSelectorBullseyeTest::
    KeypointSelectorBullseyeTest()
      : brick::test::TestFixture<KeypointSelectorBullseyeTest>("KeypointSelectorBullseyeTest"),
        m_defaultTolerance(1.0E-8)
    {
      BRICK_TEST_REGISTER_MEMBER(testKeypointSelectorBullseye);
    }


    void
    KeypointSelectorBullseyeTest::
    testKeypointSelectorBullseye()
    {
      // Load an image with a moderately tricky bullseye in it.
      std::string inputFileName = getBullseyeFileNamePGM0();
      Image<GRAY8> inputImage = readPGM8(inputFileName);

      // Where we expect the keypoint detector to fire.
      // Note(xxx): must be a better way than hardcoding this.
      numeric::Index2D bullseyePosition(59, 54);
      
      // Make sure the detector finds the target.
      KeypointSelectorBullseye<double> selector(10, 15, 5);
      selector.setImage(inputImage);
      std::vector< KeypointBullseye<int> > keypoints = selector.getKeypoints();

      Image<GRAY8> flagImage(inputImage.rows(), inputImage.columns());
      flagImage = 0;
      for(unsigned int ii = 0; ii < keypoints.size(); ++ii) {
        flagImage(keypoints[ii].row, keypoints[ii].column) =
          (ii + 1) * (255 / keypoints.size());
      }
      writePGM8("flag.pgm", flagImage);

      BRICK_TEST_ASSERT(keypoints.size() == 1);
      BRICK_TEST_ASSERT(keypoints[0].row == bullseyePosition.getRow());
      BRICK_TEST_ASSERT(keypoints[0].column == bullseyePosition.getColumn());

#if 0
      // Make sure sub-pixel version is plausible.
      std::vector< KeypointBullseye<double> > keypointsGP =
        selector.getKeypointsGeneralPosition();
      BRICK_TEST_ASSERT(keypointsGP.size() == 1);
      BRICK_TEST_ASSERT(
        brick::test::approximatelyEqual(
          keypointsGP[ii].row, (double)bullseyePosition.getRow(), 0.5));
      BRICK_TEST_ASSERT(
        brick::test::approximatelyEqual(
          keypointsGP[ii].column, (double)bullseyePosition.getColumn(), 0.5));
#endif

    }

  } // namespace computerVision

} // namespace brick


#if 1

int main()
{
  brick::computerVision::KeypointSelectorBullseyeTest currentTest;
  currentTest.testKeypointSelectorBullseye();
  bool result = currentTest.run();

  return (result ? 0 : 1);
}

#else

namespace {

  brick::computerVision::KeypointSelectorBullseyeTest currentTest;

}

#endif