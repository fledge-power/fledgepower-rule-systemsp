/*
 * Fledge filter sets value back to 0 for system status point
 *
 * Copyright (c) 2020, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Yannick Marchetaux
 *
 */
#include <regex>
#include <datapoint.h>
#include <reading.h>
#include <plugin_api.h>
#include <rapidjson/document.h>

#include "ruleSystemSp.h"
#include "constantsSystem.h"
#include "datapoint_utility.h"
#include "utilityPivot.h"

using namespace DatapointUtility;
using namespace systemspr;


/**
 * Modification of configuration
 *
 * @param config : configuration ExchangedData + Asset
 */
void RuleSystemSp::setJsonConfig(const ConfigCategory& config) {
    if (config.itemExists("exchanged_data")) {
        m_configPlugin.importExchangedData(config.getValue("exchanged_data"));
    }
    if (config.itemExists("asset")) {
        m_configPlugin.importAsset(config.getValue("asset"));
    }
}

/**
 * Evaluated if the rule is matched by one of the input assets
 *
 * @param assetValues : JSON string document with notification data.
 */
bool RuleSystemSp::evalRule(const std::string& assetValues) {
    std::lock_guard<std::mutex> guard(m_configMutex);
    std::string beforeLog = ConstantsSystem::NamePlugin + " - RuleSystemSp::evalRule :";
    // Reinitialize reason, asset cause
    m_reason = "";
    m_asset = "";
    // Plugin disabled, no filtering
    if (!isEnabled()) {
        return false;
    }
    // No asset to track, no filtering
    if (!m_configPlugin.hasConnectionLossTracking()) {
        return false;
    }
    rapidjson::Document doc;
	doc.Parse(assetValues.c_str());
	if (doc.HasParseError()) {
        UtilityPivot::log_error("%s JSON parse error in: %s", beforeLog.c_str(), assetValues.c_str());
		return false;
	}

    if (!doc.IsObject()) {
        UtilityPivot::log_error("%s Asset is not an object, ignoring: %s", beforeLog.c_str(), assetValues.c_str());
        return false;
    }

    const std::string& trackedAsset = m_configPlugin.getTrackedAsset();
    if (!doc.HasMember(trackedAsset.c_str())) {
        UtilityPivot::log_debug("%s Asset is not the one being tracked, ignoring: %s", beforeLog.c_str(), assetValues.c_str());
        return false;
    }

    const rapidjson::Value& reading = doc[trackedAsset.c_str()];
    if (!reading.IsObject()) {
        UtilityPivot::log_error("%s Reading is not an object, ignoring: %s", beforeLog.c_str(), assetValues.c_str());
        return false;
    }

    if (!reading.HasMember("south_event")) {
        UtilityPivot::log_debug("%s Reading is not a south event, ignoring: %s", beforeLog.c_str(), assetValues.c_str());
        return false;
    }

    const rapidjson::Value& south_event = reading["south_event"];
    if (!south_event.IsObject()) {
        UtilityPivot::log_error("%s South event is not an object, ignoring: %s", beforeLog.c_str(), assetValues.c_str());
        return false;
    }

    std::string connx_status;
    if (south_event.HasMember("connx_status") && south_event["connx_status"].IsString()) {
        connx_status = south_event["connx_status"].GetString();
    }
    std::string gi_status;
    if (south_event.HasMember("gi_status") && south_event["gi_status"].IsString()) {
        gi_status = south_event["gi_status"].GetString();
    }

    if (connx_status == "not connected") {
        UtilityPivot::log_debug("%s Sending connection lost notification", beforeLog.c_str());
        m_asset = "connx_status";
        m_reason = "not connected";
        return true;
    }
    else if (gi_status == "finished") {
        UtilityPivot::log_debug("%s Sending connected notification", beforeLog.c_str());
        m_asset = "gi_status";
        m_reason = "finished";
        return true;
    }

    return false;
}

/**
 * Returns the json string containing the notification data
 *
 * @return The JSON containing the notification reason
 */
std::string RuleSystemSp::getReason() const {
    if (m_reason.empty()) {
        return m_reason;
    }
    std::string asset = m_asset;
    if (asset.empty()) {
        asset = "prt.inf";
    }

    static const std::string reasonTemplate = QUOTE({
        "asset": "<asset>",
        "reason": "<reason>"
    });
    std::string result = std::regex_replace(reasonTemplate, std::regex("<asset>"), asset);
    return std::regex_replace(result, std::regex("<reason>"), m_reason);
}

/**
 * Returns the json triggers that should cause eval to be called
 *
 * @return The JSON containing the trigger asset
 */
std::string RuleSystemSp::getTriggers() const {
    std::lock_guard<std::mutex> guard(m_configMutex);
    const std::string& trackedAsset = m_configPlugin.getTrackedAsset();
    if (trackedAsset.empty()) {
        return QUOTE({"triggers": []});
    }

    static const std::string triggersTemplate = QUOTE({
		"triggers": [
			{
				"asset": "<asset>"
			}
		]
	});
    return std::regex_replace(triggersTemplate, std::regex("<asset>"), trackedAsset);
}

/**
 * Reconfiguration entry point to the filter.
 *
 * This method runs holding the configMutex to prevent
 * ingest using the regex class that may be destroyed by this
 * call.
 *
 * Pass the configuration to the base FilterPlugin class and
 * then call the private method to handle the filter specific
 * configuration.
 *
 * @param newConfig  The JSON of the new configuration
 */
void RuleSystemSp::reconfigure(const ConfigCategory& config) {
    std::lock_guard<std::mutex> guard(m_configMutex);
    if (config.itemExists("enable")) {
        m_enabled = config.getValue("enable").compare("true") == 0 ||
                    config.getValue("enable").compare("True") == 0;
    }
    setJsonConfig(config);
}
