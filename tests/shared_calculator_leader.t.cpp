#include <gtest/gtest.h>
#include <shared_calculator_leader.h>

#include <thread>

TEST(SharedCalculatorLeaderTest, RunSuccessfullyCreatesAndSubmitsEvents) {
  Calculator::Leader leaderUnderTest;

  // Spawn Run() in a thread
  std::thread runThread(
      [&leaderUnderTest]() { EXPECT_NO_THROW(leaderUnderTest.Run()); });

  // Let it run for a bit
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Check that events were created
  const auto [value1, index1] = leaderUnderTest.GetCurrentValueAndIndex();
  EXPECT_GT(index1, 0);  // Should have at least one event
  EXPECT_NE(value1, 0);  // Value should have changed

  // Can't join because Run() is infinite, so detach
  runThread.detach();
}

TEST(SharedCalculatorLeaderTest,
     GetCurrentValueAndIndexReturnsDifferentValuesAsEventsAreApplied) {
  Calculator::Leader leaderUnderTest;

  // Initially, the value should be 0 and index should be 0
  auto [initialValue, initialIndex] = leaderUnderTest.GetCurrentValueAndIndex();
  EXPECT_EQ(initialValue, 0);
  EXPECT_EQ(initialIndex, 0);

  // After running the leader for a bit, the value and index should have updated
  std::thread leaderThread(
      [&leaderUnderTest]() { EXPECT_NO_THROW(leaderUnderTest.Run()); });
  std::this_thread::sleep_for(
      std::chrono::seconds(1));  // Let the leader generate some events

  auto [updatedValue, updatedIndex] = leaderUnderTest.GetCurrentValueAndIndex();
  // Sometimes the values will be the same, for the purposes of this take home
  // we can ignore that possibility
  EXPECT_NE(updatedValue, initialValue)
      << "Could possible be false positive since values might coincidentally "
         "be the same";
  EXPECT_GT(updatedIndex,
            initialIndex);  // Index should have incremented from initial

  leaderThread.detach();  // Detach the thread since Run() is an infinite loop
}

TEST(SharedCalculatorLeaderTest,
     WaitForUpdatesFromIndexReturnsEmptyIfTimeoutReached) {
  Calculator::Leader leaderUnderTest;
  std::thread leaderThread(
      [&leaderUnderTest]() { EXPECT_NO_THROW(leaderUnderTest.Run()); });

  // Wait for updates from index 0 with a short timeout, should return nullopt
  // since no events have been generated yet
  auto updates =
      leaderUnderTest.WaitForUpdatesFromIndex(0, std::chrono::milliseconds(0));
  EXPECT_FALSE(updates.has_value());

  leaderThread.detach();  // Detach the thread since Run() is an infinite loop
}

TEST(SharedCalculatorLeaderTest,
     WaitForUpdatesFromIndexReturnsEmptyIfLeaderNeverRuns) {
  Calculator::Leader leaderUnderTest;

  // Will not matter how long we wait, should return nullopt since leader is not
  // running and thus will never generate events
  auto updates = leaderUnderTest.WaitForUpdatesFromIndex(
      0, std::chrono::milliseconds(100));
  EXPECT_FALSE(updates.has_value());
}

TEST(SharedCalculatorLeaderTest,
     WaitForUpdatesFromIndexReturnsEventsGeneratedAfterGivenIndex) {
  Calculator::Leader leaderUnderTest;
  std::thread leaderThread(
      [&leaderUnderTest]() { EXPECT_NO_THROW(leaderUnderTest.Run()); });

  // Wait for a bit to let some events be generated
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Get the current index
  auto [currentValue, currentIndex] = leaderUnderTest.GetCurrentValueAndIndex();

  // Wait for updates from the current index with a reasonable timeout
  auto updates = leaderUnderTest.WaitForUpdatesFromIndex(
      currentIndex, std::chrono::milliseconds(500));
  EXPECT_TRUE(updates.has_value());
  EXPECT_GT(updates->size(), 0);  // Should have at least one new event

  leaderThread.detach();  // Detach the thread since Run() is an infinite loop
}