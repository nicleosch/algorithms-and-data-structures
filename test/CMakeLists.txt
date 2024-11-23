################
# ARCHARIOS-DB #
################


FILE (GLOB_RECURSE PROJECT_TESTS *.cc *.h)


# EXECUTABLE

add_executable(tester main.cc ${PROJECT_TESTS})
target_link_libraries(tester ${PROJECT_NAME} GTest::gtest_main)


# TESTING

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/ff233bdd4cac0a0bf6e5cd45bda3406814cb2796.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)
gtest_discover_tests(tester)