#ifndef INCLUDE_CONFIG_PLUGIN_H_
#define INCLUDE_CONFIG_PLUGIN_H_

/*
 * Import confiugration Exchanged data
 *
 * Copyright (c) 2020, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Yannick Marchetaux
 * 
 */
#include <vector>
#include <string>
#include <memory>

#include <rapidjson/document.h>

namespace systemspr {

class ConfigPlugin {
public:  
    void importExchangedData(const std::string & exchangeConfig);
    void importAsset(const std::string & assetConfig);
    bool hasConnectionLossTracking() const { return m_connectionLossTracking; }
    const std::string& getTrackedAsset() const { return m_trackedAsset; }
    
private:
    bool m_importDatapoint(const rapidjson::Value& datapoint);

    bool        m_connectionLossTracking{false};
    std::string m_trackedAsset;
};
};

#endif  // INCLUDE_CONFIG_PLUGIN_H_