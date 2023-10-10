#include <logger.h>
#include <cctype>
#include <algorithm>

#include "configPlugin.h"
#include "constantsSystem.h"
#include "utilityPivot.h"

using namespace systemspr;

/**
 * Import data in the form of Exchanged_data
 * The data is saved in a map m_exchangeDefinitions
 * 
 * @param exchangeConfig : configuration Exchanged_data as a string 
*/
void ConfigPlugin::importExchangedData(const std::string & exchangeConfig) {
    
    std::string beforeLog = ConstantsSystem::NamePlugin + " - ConfigPlugin::importExchangedData :";
    rapidjson::Document document;

    m_connectionLossTracking = false;

    if (document.Parse(exchangeConfig.c_str()).HasParseError()) {
        UtilityPivot::log_fatal("%s Parsing error in data exchange configuration", beforeLog.c_str());
        return;
    }

    if (!document.IsObject()) {
        UtilityPivot::log_fatal("%s Root element is not an object", beforeLog.c_str());
        return;
    }

    if (!document.HasMember(ConstantsSystem::JsonExchangedData) || !document[ConstantsSystem::JsonExchangedData].IsObject()) {
        UtilityPivot::log_fatal("%s exchanged_data not found in root object or is not an object", beforeLog.c_str());
        return;
    }
    const rapidjson::Value& exchangeData = document[ConstantsSystem::JsonExchangedData];

    if (!exchangeData.HasMember(ConstantsSystem::JsonDatapoints) || !exchangeData[ConstantsSystem::JsonDatapoints].IsArray()) {
        UtilityPivot::log_fatal("%s datapoints not found in exchanged_data or is not an array", beforeLog.c_str());
        return;
    }
    const rapidjson::Value& datapoints = exchangeData[ConstantsSystem::JsonDatapoints];

    bool foundPrtInf = false;
    for (const rapidjson::Value& datapoint : datapoints.GetArray()) {
        foundPrtInf = m_importDatapoint(datapoint);
        if (foundPrtInf) {
            break;
        }
        
    }
    UtilityPivot::log_debug("%s Connection loss tracking is %s", beforeLog.c_str(), foundPrtInf?"active":"inactive");
    m_connectionLossTracking = foundPrtInf;
}

/**
 * Import data from a single datapoint of exchanged data
 * 
 * @param datapoint : datapoint to parse and import
*/
bool ConfigPlugin::m_importDatapoint(const rapidjson::Value& datapoint) {

    std::string beforeLog = ConstantsSystem::NamePlugin + " - ConfigPlugin::m_importDatapoint :";
    if (!datapoint.IsObject()) {
        UtilityPivot::log_error("%s datapoint is not an object", beforeLog.c_str());
        return false;
    }
    
    if (!datapoint.HasMember(ConstantsSystem::JsonPivotType) || !datapoint[ConstantsSystem::JsonPivotType].IsString()) {
        UtilityPivot::log_error("%s pivot_type not found in datapoint or is not a string", beforeLog.c_str());
        return false;
    }

    std::string type = datapoint[ConstantsSystem::JsonPivotType].GetString();
    if (type != ConstantsSystem::JsonCdcSps && type != ConstantsSystem::JsonCdcDps) {
        // Ignore datapoints that are not a TS
        return false;
    }

    if (!datapoint.HasMember(ConstantsSystem::JsonPivotId) || !datapoint[ConstantsSystem::JsonPivotId].IsString()) {
        UtilityPivot::log_error("%s pivot_id not found in datapoint or is not a string", beforeLog.c_str());
        return false;
    }
    std::string pivot_id = datapoint[ConstantsSystem::JsonPivotId].GetString();

    if (!datapoint.HasMember(ConstantsSystem::JsonPivotSubtypes) || !datapoint[ConstantsSystem::JsonPivotSubtypes].IsArray()) {
        // No pivot subtypes, nothing to do
        return false;
    }

    if (!datapoint.HasMember(ConstantsSystem::JsonLabel) || !datapoint[ConstantsSystem::JsonLabel].IsString()) {
        UtilityPivot::log_error("%s label not found in datapoint or is not a string", beforeLog.c_str());
        return false;
    }
    std::string label = datapoint[ConstantsSystem::JsonLabel].GetString();

    auto subtypes = datapoint[ConstantsSystem::JsonPivotSubtypes].GetArray();

    bool foundPrtInf = false;
    for (rapidjson::Value::ConstValueIterator itr = subtypes.Begin(); itr != subtypes.End(); ++itr) {
        if (!(*itr).IsString()) {
            continue;
        }
        std::string s = (*itr).GetString();
        if(s == "prt.inf") {
            foundPrtInf = true;
            break;
        }
    }

    return foundPrtInf;
}

void ConfigPlugin::importAsset(const std::string & assetConfig) {
    std::string beforeLog = ConstantsSystem::NamePlugin + " - ConfigPlugin::importAsset :";
    m_trackedAsset = assetConfig;
    UtilityPivot::log_debug("%s Connection loss asset tracked: %s", beforeLog.c_str(), m_trackedAsset.c_str());
}