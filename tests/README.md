# uProbeX Unit Tests

This directory contains unit tests for the uProbeX project.

## Dependencies

The tests require the following dependencies:
- **Google Test (gtest)**: C++ testing framework
- **Google Mock (gmock)**: C++ mocking framework
- **Qt6**: For Qt-specific functionality
- **ZeroMQ (cppzmq)**: For ZMQ socket testing

## Installation

### Installing Google Test on macOS:
```bash
brew install googletest
```

### Installing Google Test on Linux:
```bash
sudo apt-get install libgtest-dev libgmock-dev
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/*.a /usr/lib
```

## Building the Tests

From the project root directory:

```bash
# Create a build directory for tests
mkdir -p build/tests
cd build/tests

# Configure with CMake
cmake ../../tests

# Build the tests
make

# Run the tests
./BlueskyCommTest

# Or use CTest
ctest --verbose
```

## Test Coverage

### BlueskyCommTest.cpp

Tests for the `BlueskyComm` class which handles communication with Bluesky QServer via ZeroMQ.

#### Current Test Coverage:

1. **Constructor Tests**
   - Valid IP address connection
   - Empty IP address handling

2. **Message Generation Tests**
   - `gen_send_mesg()` without parameters
   - `gen_send_mesg()` with parameters
   - `gen_send_mesg2()` with JSON parameters

3. **Plan Conversion Tests**
   - `plan_to_json_item()` basic conversion
   - `plan_to_json_item()` with parameters (String, Double, Bool types)
   - `plan_to_json_item()` without UUID

4. **Parameter Parsing Tests**
   - `parse_args()` with mixed types (string, double, int, bool)
   - `parse_args()` with empty object

5. **Error Handling Tests**
   - Operations with null socket (no connection)

#### Integration Tests (Commented Out)

The following integration tests are outlined but commented out because they require:
- Actual ZMQ server/mock setup
- Network connectivity
- More sophisticated mocking infrastructure

These tests include:
- Environment operations (open, close)
- Queue operations (start, stop, queue, update, move, remove)
- Scan operations (get available scans, queued scans, history)
- History operations (clear, get)

To enable these tests, you would need to:
1. Set up a mock ZMQ server or use dependency injection to mock the socket
2. Create test fixtures that simulate server responses
3. Implement proper cleanup between tests

## Running Specific Tests

```bash
# Run all tests
./BlueskyCommTest

# Run tests with filtering
./BlueskyCommTest --gtest_filter=BlueskyCommTest.ConstructorWithValidIP

# Run tests with detailed output
./BlueskyCommTest --gtest_verbose
```

## Adding New Tests

To add new tests:

1. Create a new test file in this directory (e.g., `NewClassTest.cpp`)
2. Update `CMakeLists.txt` to include the new test executable
3. Follow the existing test patterns using Google Test macros:
   - `TEST_F()` for tests with fixtures
   - `TEST()` for standalone tests
   - `EXPECT_*()` for non-fatal assertions
   - `ASSERT_*()` for fatal assertions

## Best Practices

1. **Isolation**: Each test should be independent and not rely on other tests
2. **Clarity**: Test names should clearly describe what they test
3. **Coverage**: Aim to test both success and failure cases
4. **Mocking**: Use mocks for external dependencies (network, file system, etc.)
5. **Speed**: Keep tests fast; use mocks instead of real network calls

## Continuous Integration

These tests can be integrated into CI/CD pipelines:

```bash
# Example CI script
cmake -S tests -B build/tests
cmake --build build/tests
cd build/tests && ctest --output-on-failure
```

## Troubleshooting

### Qt MOC Issues
If you encounter MOC-related errors, ensure `CMAKE_AUTOMOC` is ON in CMakeLists.txt.

### ZeroMQ Linking Issues
Make sure `cppzmq` and `libzmq` are properly installed and findable by CMake.

### Google Test Not Found
Ensure Google Test is installed and `GTEST_ROOT` is set if installed in a non-standard location:
```bash
export GTEST_ROOT=/path/to/googletest
```

## Future Improvements

1. Add integration tests with a test ZMQ server
2. Implement proper ZMQ socket mocking
3. Add tests for BlueskyPlan and BlueskyParam classes
4. Add performance/benchmark tests
5. Increase code coverage to >90%
6. Add memory leak detection with Valgrind
