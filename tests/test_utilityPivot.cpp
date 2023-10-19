#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>

#include "utilityPivot.h"

using namespace systemspr;

TEST(TestUtilityPivot, Logs)
{
    std::string text("This message is at level %s");
    ASSERT_NO_THROW(UtilityPivot::log_debug(text.c_str(), "debug"));
    ASSERT_NO_THROW(UtilityPivot::log_info(text.c_str(), "info"));
    ASSERT_NO_THROW(UtilityPivot::log_warn(text.c_str(), "warning"));
    ASSERT_NO_THROW(UtilityPivot::log_error(text.c_str(), "error"));
    ASSERT_NO_THROW(UtilityPivot::log_fatal(text.c_str(), "fatal"));
}