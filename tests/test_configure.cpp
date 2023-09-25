#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <plugin_api.h>

#include "configPlugin.h"

using namespace systemspr;

static std::string configureErrorParseJSON = QUOTE({
    "exchanged_data" : {
        "eee"
    }
});

static std::string configureErrorRootNotObject = QUOTE(42);

static std::string configureErrorExchanged_data = QUOTE({
    "configureErrorExchanged_data": {         
        "datapoints" : [          
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
});

static std::string configureErrorExchangedDataNotObject = QUOTE({
    "exchanged_data" : [
        42
    ]
});

static std::string configureErrorDatapoint = QUOTE({
    "exchanged_data": {         
        "configureErrorDatapoint" : [          
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
});

static std::string configureErrorDatapointNotObject = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            42
        ]
    }
});

static std::string configureErrorType = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TS-1",
                "pivot_id":"M_2367_3_15_4",
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
});

static std::string configureErrorTypeMv = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TS-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"MvTyp",
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
});

static std::string configureErrorPivotID = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TS-1",
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
});

static std::string configureErrorLabel = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
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
});

static std::string configureErrorSubtypes = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TS-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"SpsTyp",
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
});

static std::string configureErrorSubtypesWithUnknownSubtype = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TS-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"SpsTyp",
                "pivot_subtypes": [
                    "test"
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
});

static std::string configureOKSps = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
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
});

static std::string configureOKDps = QUOTE({
    "exchanged_data": {         
        "datapoints" : [          
            {
                "label":"TS-1",
                "pivot_id":"M_2367_3_15_4",
                "pivot_type":"DpsTyp",
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
});

class TestPluginConfigure : public testing::Test
{
protected:
    std::shared_ptr<ConfigPlugin> configPlugin;  // Object on which we call for tests

    // Setup is ran for every tests, so each variable are reinitialised
    void SetUp() override
    {
        configPlugin = std::make_shared<ConfigPlugin>();
    }
};

TEST_F(TestPluginConfigure, ConfigureErrorParsingJSON) 
{
	configPlugin->importExchangedData(configureErrorParseJSON);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorRootNotObject) 
{
	configPlugin->importExchangedData(configureErrorRootNotObject);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorExchanged_data) 
{
	configPlugin->importExchangedData(configureErrorExchanged_data);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorExchangedDataNotObject) 
{
	configPlugin->importExchangedData(configureErrorExchangedDataNotObject);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorDatapoint) 
{
	configPlugin->importExchangedData(configureErrorDatapoint);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorDatapointNotObject) 
{
	configPlugin->importExchangedData(configureErrorDatapointNotObject);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorType) 
{
	configPlugin->importExchangedData(configureErrorType);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorTypeMv) 
{
	configPlugin->importExchangedData(configureErrorTypeMv);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorPivotID) 
{
	configPlugin->importExchangedData(configureErrorPivotID);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorLabel) 
{
	configPlugin->importExchangedData(configureErrorLabel);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorSubtypes) 
{
	configPlugin->importExchangedData(configureErrorSubtypes);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorSubtypesWithUnknownSubtype) 
{
	configPlugin->importExchangedData(configureErrorSubtypesWithUnknownSubtype);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureOKSps) 
{
	configPlugin->importExchangedData(configureOKSps);
    ASSERT_TRUE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureOKDps) 
{
	configPlugin->importExchangedData(configureOKDps);
    ASSERT_TRUE(configPlugin->hasConnectionLossTracking());
}