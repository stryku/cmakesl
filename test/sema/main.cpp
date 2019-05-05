#include <gmock/gmock.h>

// Todo: move to common file
int main(int argc, char** argv)
{
  ::testing::InitGoogleMock(&argc, argv);
  return ::RUN_ALL_TESTS();
}
