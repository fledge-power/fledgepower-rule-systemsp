#include <gtest/gtest.h>
#include <plugin_api.h>
#include <rapidjson/document.h>

#include <atomic>
#include <thread>

#include "version.h"


extern "C" {
    PLUGIN_INFORMATION* plugin_info();
};

TEST(TestPluginInfo, PluginInfo)
{
    PLUGIN_INFORMATION* info = plugin_info();
    ASSERT_STREQ(info->name, "systemspr");
    ASSERT_STREQ(info->version, VERSION);
    ASSERT_EQ(info->options, 0);
    ASSERT_STREQ(info->type, PLUGIN_TYPE_NOTIFICATION_RULE);
    ASSERT_STREQ(info->interface, "1.0.0");
}

TEST(TestPluginInfo, PluginInfoConfigParse)
{
    PLUGIN_INFORMATION* info = plugin_info();
    rapidjson::Document doc;
    doc.Parse(info->config);
    ASSERT_EQ(doc.HasParseError(), false);
    ASSERT_EQ(doc.IsObject(), true);
    ASSERT_EQ(doc.HasMember("plugin"), true);
    ASSERT_EQ(doc.HasMember("enable"), true);
    ASSERT_EQ(doc.HasMember("exchanged_data"), true);
}

TEST(TestDebugCI, JoinFromThread)
{
    // workerThread represent some worker thread performing its task
    std::shared_ptr<std::thread> workerThread = std::make_shared<std::thread>([]() {
        printf("workerThread started\n"); fflush(stdout);
        // Do the work
        std::this_thread::sleep_for(std::chrono::seconds(5));
        printf("workerThread completed\n"); fflush(stdout);
    });

    // Here we handle the case where workerThread may be stuck on some IO and never complete
    // Wait a certain time for worker thread to complete and join
    printf("Waiting for workerThread to join...\n"); fflush(stdout);
    std::atomic<bool> joinSuccess{ false };
    std::thread joinThread([&workerThread, &joinSuccess]() {
        printf("joinThread started\n"); fflush(stdout);
        workerThread->join();
        joinSuccess = true;
        printf("joinThread completed\n"); fflush(stdout);
        });
    joinThread.detach();
    int timeMs = 0;
    // Wait up to 10s for workerThread to join
    while (timeMs < 10000) {
        if (joinSuccess) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeMs += 100;
    }
    // Expected behavior:
    // - If the thread did join in time the assert passes and the test completes successdully
    // - If it did not join in time the tests fails then program crash because
    //   workerThread destructor was called before join() was completed
    ASSERT_TRUE(joinSuccess);
}