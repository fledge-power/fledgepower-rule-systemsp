#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <plugin_api.h>
#include <rapidjson/document.h>

#include "ruleSystemSp.h"
#include "constantsSystem.h"

using namespace DatapointUtility;
using namespace systemspr;


static std::string configure = QUOTE({
    "enable": {
        "value": "true"
    },
    "asset": {
        "value": "CONNECTION-1"
    },
    "exchanged_data": {
        "value": {
            "exchanged_data": {
                "datapoints": [          
                    {
                        "label":"TS-1",
                        "pivot_id":"M_2367_3_15_4",
                        "pivot_type":"SpsTyp",
                        "pivot_subtypes": [
                            "prt.inf"
                        ],
                        "protocols":[
                            {
                                "name":"IEC104",
                                "typeid":"M_ME_NC_1",
                                "address":"3271612"
                            }
                        ]
                    },
                    {
                        "label":"TS-2",
                        "pivot_id":"M_2367_3_15_5",
                        "pivot_type":"DpsTyp",
                        "pivot_subtypes": [
                            "prt.inf"
                        ],
                        "protocols":[
                            {
                                "name":"IEC104",
                                "typeid":"M_ME_NC_2",
                                "address":"3271613"
                            }
                        ]
                    } 
                ]
            }
        }
    }
});

std::string emptyConfig = QUOTE({
    "enable": {
        "value": "true"
    },
    "exchanged_data": {
        "value": {
            "exchanged_data": {
                "datapoints": []
            }
        }
    }
});

std::string defaultTrigger = QUOTE({
    "triggers": [
        {
            "asset": "CONNECTION-1"
        }
    ]
});
 
extern "C" {
	PLUGIN_INFORMATION *plugin_info();
	PLUGIN_HANDLE plugin_init(ConfigCategory *config);
    void plugin_reconfigure(PLUGIN_HANDLE *handle, const std::string& newConfig);
    std::string plugin_triggers(PLUGIN_HANDLE handle);
    bool plugin_eval(PLUGIN_HANDLE handle, const std::string& assetValues);
    std::string plugin_reason(PLUGIN_HANDLE handle);
    void plugin_shutdown(PLUGIN_HANDLE *handle);
};

class TestSystemSp : public testing::Test
{
protected:
    RuleSystemSp *filter = nullptr;  // Object on which we call for tests

    // Setup is ran for every tests, so each variable are reinitialised
    void SetUp() override
    {
        PLUGIN_INFORMATION *info = plugin_info();
		ConfigCategory *config =  new ConfigCategory("systemsp", info->config);
		ASSERT_NE(config, nullptr);		
		config->setItemsValueFromDefault();
		config->setValue("enable", "true");
		
		PLUGIN_HANDLE handle = nullptr;
        ASSERT_NO_THROW(handle = plugin_init(config));
		filter = static_cast<RuleSystemSp *>(handle);

        ASSERT_NO_THROW(plugin_reconfigure(reinterpret_cast<PLUGIN_HANDLE*>(filter), configure));
        ASSERT_TRUE(filter->isEnabled());
    }

    // TearDown is ran for every tests, so each variable are destroyed again
    void TearDown() override
    {
        if (filter) {
            ASSERT_NO_THROW(plugin_shutdown(reinterpret_cast<PLUGIN_HANDLE*>(filter)));
        }
    }

    template<class... Args>
    static void debug_print(std::string format, Args&&... args) {    
        printf(format.append("\n").c_str(), std::forward<Args>(args)...);
        fflush(stdout);
    }

    static void validateNotification(const std::string& jsonNotification, std::map<std::string, std::string> attributes) {
        // Validate existance of the required keys and non-existance of the others
        for (const auto &kvp : attributes) {
            const std::string& name(kvp.first);
            ASSERT_TRUE(std::find(allNotificationAttributeNames.begin(), allNotificationAttributeNames.end(), name)
                != allNotificationAttributeNames.end()) << ": Attribute not listed in full list: " << name;
        }

        rapidjson::Document d;
        d.Parse(jsonNotification.c_str());
        ASSERT_FALSE(d.HasParseError()) << "JSON parse error in: " << jsonNotification;
        ASSERT_TRUE(d.IsObject());
        for(const std::string& name: allNotificationAttributeNames) {
            ASSERT_EQ(d.HasMember(name.c_str()), static_cast<bool>(attributes.count(name)))
                << ": Attribute not found: " << name;
        }
        // Validate value and type of each key
        for(auto const& kvp: attributes) {
            const std::string& name = kvp.first;
            const std::string& expectedValue = kvp.second;
            ASSERT_TRUE(d[name.c_str()].IsString()) << "Attribute is not a string: " << name;
            ASSERT_STREQ(expectedValue.c_str(), d[name.c_str()].GetString()) << "Attribute has unexpected value: " << name;
        }
    }

    static const std::vector<std::string> allNotificationAttributeNames;
};

const std::vector<std::string> TestSystemSp::allNotificationAttributeNames = {"asset", "reason"};

TEST_F(TestSystemSp, ConnectionLossMessages)
{
	std::string customConfig = QUOTE({
        "enable": {
            "value": "true"
        },
        "asset": {
            "value": "CONNECTION-1"
        },
        "exchanged_data": {
            "value": {
                "exchanged_data": {
                    "datapoints": [          
                        {
                            "label":"TS-1",
                            "pivot_id":"M_2367_3_15_4",
                            "pivot_type":"SpsTyp",
                            "pivot_subtypes": [
                                "prt.inf"
                            ],
                            "protocols":[
                                {
                                    "name":"IEC104",
                                    "typeid":"M_ME_NC_1",
                                    "address":"3271612"
                                }
                            ]
                        },
                        {
                            "label":"TS-2",
                            "pivot_id":"M_2367_3_15_5",
                            "pivot_type":"DpsTyp",
                            "pivot_subtypes": [
                                "prt.inf"
                            ],
                            "protocols":[
                                {
                                    "name":"IEC104",
                                    "typeid":"M_ME_NC_2",
                                    "address":"3271613"
                                }
                            ]
                        } 
                    ]
                }
            }
        }
    });

    debug_print("Reconfigure plugin");
    ASSERT_NO_THROW(plugin_reconfigure(reinterpret_cast<PLUGIN_HANDLE*>(filter), customConfig));
    ASSERT_TRUE(filter->isEnabled());
    ASSERT_TRUE(filter->getConfigPlugin().hasConnectionLossTracking());
    ASSERT_STREQ(plugin_triggers(filter).c_str(), defaultTrigger.c_str());

    // Send invalid json
    std::string assetInvalidJson = QUOTE({42});
    ASSERT_FALSE(plugin_eval(filter, assetInvalidJson));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Send asset that is not an object
    std::string assetNotAnObject = QUOTE([42]);
    ASSERT_FALSE(plugin_eval(filter, assetNotAnObject));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Send asset with no asset name attribute
    std::string assetNoReading = QUOTE({
        "something": "something"
    });
    ASSERT_FALSE(plugin_eval(filter, assetNoReading));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Send asset with asset name that is not an object
    std::string assetReadingNotAnObject = QUOTE({
        "CONNECTION-1": 42
    });
    ASSERT_FALSE(plugin_eval(filter, assetReadingNotAnObject));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Send asset with no south_event
    std::string assetNoSouthEvent = QUOTE({
        "CONNECTION-1": {
            "something": "something"
        }
    });
    ASSERT_FALSE(plugin_eval(filter, assetNoSouthEvent));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Send asset with south_event that is not an object
    std::string assetSouthEventNotAnObject = QUOTE({
        "CONNECTION-1": {
            "south_event": 42
        }
    });
    ASSERT_FALSE(plugin_eval(filter, assetSouthEventNotAnObject));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Send asset with empty south_event
    std::string assetEmptySouthEvent = QUOTE({
        "CONNECTION-1": {
            "south_event": {}
        }
    });
    ASSERT_FALSE(plugin_eval(filter, assetEmptySouthEvent));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Send asset with non relevant status change in south_event
    std::string assetIrrelevantSouthEvent = QUOTE({
        "CONNECTION-1": {
            "south_event": {
                "connx_status": "started",
                "gi_status": "failed"
            }
        }
    });
    ASSERT_FALSE(plugin_eval(filter, assetIrrelevantSouthEvent));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Send asset with connection loss in south_event
    std::string assetConnectionLoss = QUOTE({
        "CONNECTION-1": {
            "south_event": {
                "connx_status": "not connected"
            }
        }
    });
    ASSERT_TRUE(plugin_eval(filter, assetConnectionLoss));
    std::string jsonNotification = plugin_reason(filter);
    validateNotification(jsonNotification, {
        {"asset", "prt.inf"},
        {"reason", "connection lost"}
    });
    if(HasFatalFailure()) return;

    // Send asset with GI completed in south_event
    std::string assetGICompleted = QUOTE({
        "CONNECTION-1": {
            "south_event": {
                "gi_status": "finished"
            }
        }
    });
    ASSERT_TRUE(plugin_eval(filter, assetGICompleted));
    jsonNotification = plugin_reason(filter);
    validateNotification(jsonNotification, {
        {"asset", "prt.inf"},
        {"reason", "connected"}
    });
    if(HasFatalFailure()) return;


    // Send asset with connection loss and GI completed in south_event (priority to connection loss)
    std::string assetConnectionLossAndGICompleted = QUOTE({
        "CONNECTION-1": {
            "south_event": {
                "connx_status": "not connected",
                "gi_status": "finished"
            }
        }
    });
    ASSERT_TRUE(plugin_eval(filter, assetConnectionLossAndGICompleted));
    jsonNotification = plugin_reason(filter);
    validateNotification(jsonNotification, {
        {"asset", "prt.inf"},
        {"reason", "connection lost"}
    });
    if(HasFatalFailure()) return;
}

TEST_F(TestSystemSp, ConnectionLossInactive) 
{
    ASSERT_NO_THROW(plugin_reconfigure(reinterpret_cast<PLUGIN_HANDLE*>(filter), emptyConfig));
    ASSERT_TRUE(filter->isEnabled());
    ASSERT_FALSE(filter->getConfigPlugin().hasConnectionLossTracking());
    ASSERT_STREQ(plugin_triggers(filter).c_str(), defaultTrigger.c_str());
   
    std::string assetValid = QUOTE({
        "CONNECTION-1": {
            "south_event": {
                "connx_status": "not connected",
                "gi_status": "finished"
            }
        }
    });
    ASSERT_FALSE(plugin_eval(filter, assetValid));
    ASSERT_STREQ(plugin_triggers(filter).c_str(), defaultTrigger.c_str());
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");
}

TEST_F(TestSystemSp, PluginDisabled) 
{
	static std::string customConfig = QUOTE({
        "enable": {
            "value": "false"
        },
        "asset": {
            "value": "CONNECTION-1"
        },
        "exchanged_data": {
            "value": {
                "exchanged_data": {
                    "datapoints": [          
                        {
                            "label":"TS-1",
                            "pivot_id":"M_2367_3_15_4",
                            "pivot_type":"SpsTyp",
                            "pivot_subtypes": [
                                "prt.inf"
                            ],
                            "protocols":[
                                {
                                    "name":"IEC104",
                                    "typeid":"M_ME_NC_1",
                                    "address":"3271612"
                                }
                            ]
                        }
                    ]
                }
            }
        }
    });

    ASSERT_NO_THROW(plugin_reconfigure(reinterpret_cast<PLUGIN_HANDLE*>(filter), customConfig));
    ASSERT_FALSE(filter->isEnabled());
    ASSERT_TRUE(filter->getConfigPlugin().hasConnectionLossTracking());
    ASSERT_STREQ(plugin_triggers(filter).c_str(), defaultTrigger.c_str());

    std::string assetValid = QUOTE({
        "CONNECTION-1": {
            "south_event": {
                "connx_status": "not connected",
                "gi_status": "finished"
            }
        }
    });
    ASSERT_FALSE(plugin_eval(filter, assetValid));
    ASSERT_STREQ(plugin_triggers(filter).c_str(), defaultTrigger.c_str());
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");
}

TEST_F(TestSystemSp, AssetTrigger) 
{
    // By default asset name use in trigger is the default tracked asset
    ASSERT_STREQ(plugin_triggers(filter).c_str(), defaultTrigger.c_str());

    // Use a different tracked asset name
    std::string customConfig = QUOTE({
        "asset": {
            "value": "TEST_ASSET"
        }
    });
    ASSERT_NO_THROW(plugin_reconfigure(reinterpret_cast<PLUGIN_HANDLE*>(filter), customConfig));

    std::string expectedTrigger = QUOTE({
        "triggers": [
            {
                "asset": "TEST_ASSET"
            }
        ]
    });
    ASSERT_STREQ(plugin_triggers(filter).c_str(), expectedTrigger.c_str());

    // Message with old asset name is rejected
    std::string assetOldName = QUOTE({
        "CONNECTION-1": {
            "south_event": {
                "connx_status": "not connected",
                "gi_status": "finished"
            }
        }
    });
    ASSERT_FALSE(plugin_eval(filter, assetOldName));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");

    // Message with new asset name is accepted
    std::string assetNewName = QUOTE({
        "TEST_ASSET": {
            "south_event": {
                "connx_status": "not connected",
                "gi_status": "finished"
            }
        }
    });
    ASSERT_TRUE(plugin_eval(filter, assetNewName));
    std::string jsonNotification = plugin_reason(filter);
    validateNotification(jsonNotification, {
        {"asset", "prt.inf"},
        {"reason", "connection lost"}
    });
    if(HasFatalFailure()) return;

    // Remove trigger
    std::string noAssetConfig = QUOTE({
        "asset": {
            "value": ""
        }
    });
    ASSERT_NO_THROW(plugin_reconfigure(reinterpret_cast<PLUGIN_HANDLE*>(filter), noAssetConfig));

    expectedTrigger = QUOTE({"triggers": []});
    ASSERT_STREQ(plugin_triggers(filter).c_str(), expectedTrigger.c_str());

    // All messages are filtered out
    ASSERT_FALSE(plugin_eval(filter, assetOldName));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");
    ASSERT_FALSE(plugin_eval(filter, assetNewName));
    ASSERT_STREQ(plugin_reason(filter).c_str(), "");
}
