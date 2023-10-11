#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <plugin_api.h>

#include "configPlugin.h"

using namespace systemspr;


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
	std::string configureErrorParseJSON = QUOTE({42});
    
    configPlugin->importExchangedData(configureErrorParseJSON);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorRootNotObject) 
{
	std::string configureErrorRootNotObject = QUOTE(42);

    configPlugin->importExchangedData(configureErrorRootNotObject);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorNoExchangedData) 
{
	std::string configureErrorNoExchangedData = QUOTE({});
    
    configPlugin->importExchangedData(configureErrorNoExchangedData);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorExchangedDataNotObject) 
{
	std::string configureErrorExchangedDataNotObject = QUOTE({
        "exchanged_data" : 42
    });
    
    configPlugin->importExchangedData(configureErrorExchangedDataNotObject);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorNoDatapoints) 
{
	std::string configureErrorNoDatapoints = QUOTE({
        "exchanged_data": {}
    });
    
    configPlugin->importExchangedData(configureErrorNoDatapoints);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorDatapointNotArray) 
{
    std::string configureErrorDatapointNotArray = QUOTE({
        "exchanged_data": {         
            "datapoints" : 42
        }
    });

    configPlugin->importExchangedData(configureErrorDatapointNotArray);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorDatapointsContainsNotObject) 
{
    std::string configureErrorDatapointsContainsNotObject = QUOTE({
        "exchanged_data": {         
            "datapoints" : [          
                42
            ]
        }
    });

    configPlugin->importExchangedData(configureErrorDatapointsContainsNotObject);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorNoType) 
{
	std::string configureErrorNoType = QUOTE({
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
    
    configPlugin->importExchangedData(configureErrorNoType);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorTypeNotString) 
{
	std::string configureErrorTypeNotString = QUOTE({
        "exchanged_data": {         
            "datapoints" : [          
                {
                    "label":"TS-1",
                    "pivot_id":"M_2367_3_15_4",
                    "pivot_type":42,
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
    
    configPlugin->importExchangedData(configureErrorTypeNotString);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorTypeInvalid) 
{
    std::string configureErrorTypeInvalid = QUOTE({
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
    
    configPlugin->importExchangedData(configureErrorTypeInvalid);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorNoPivotID) 
{
	std::string configureErrorNoPivotID = QUOTE({
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

    configPlugin->importExchangedData(configureErrorNoPivotID);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorPivotIDNotString) 
{
	std::string configureErrorPivotIDNotString = QUOTE({
        "exchanged_data": {         
            "datapoints" : [          
                {
                    "label":"TS-1",
                    "pivot_id":42,
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

    configPlugin->importExchangedData(configureErrorPivotIDNotString);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorNoLabel) 
{
	std::string configureErrorNoLabel = QUOTE({
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

    configPlugin->importExchangedData(configureErrorNoLabel);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}


TEST_F(TestPluginConfigure, ConfigureErrorLabelNotString) 
{
	std::string configureErrorLabelNotString = QUOTE({
        "exchanged_data": {         
            "datapoints" : [          
                {
                    "label":42,
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

    configPlugin->importExchangedData(configureErrorLabelNotString);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorNoSubtypes) 
{
	std::string configureErrorNoSubtypes = QUOTE({
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
    
    configPlugin->importExchangedData(configureErrorNoSubtypes);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorSubtypesNotArray) 
{
	std::string configureErrorSubtypesNotArray = QUOTE({
        "exchanged_data": {         
            "datapoints" : [          
                {
                    "label":"TS-1",
                    "pivot_id":"M_2367_3_15_4",
                    "pivot_type":"SpsTyp",
                    "pivot_subtypes": 42,
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
    
    configPlugin->importExchangedData(configureErrorSubtypesNotArray);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorSubtypesNotContainString) 
{
	std::string configureErrorSubtypesNotContainString = QUOTE({
        "exchanged_data": {         
            "datapoints" : [          
                {
                    "label":"TS-1",
                    "pivot_id":"M_2367_3_15_4",
                    "pivot_type":"SpsTyp",
                    "pivot_subtypes": [
                        42
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
    
    configPlugin->importExchangedData(configureErrorSubtypesNotContainString);
    ASSERT_FALSE(configPlugin->hasConnectionLossTracking());
}

TEST_F(TestPluginConfigure, ConfigureErrorSubtypesWithUnknownSubtype) 
{
	std::string configureErrorSubtypesWithUnknownSubtype = QUOTE({
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