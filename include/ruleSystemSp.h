#ifndef INCLUDE_FILTER_SYSTEM_SP_H_
#define INCLUDE_FILTER_SYSTEM_SP_H_

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

#include <datapoint_utility.h>
#include <config_category.h>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>

#include "configPlugin.h"

using FuncPtr = void (*)(void *, void *);

namespace systemspr {

class RuleSystemSp
{
public:
    void reconfigure(const ConfigCategory& config);
    void setJsonConfig(const ConfigCategory& config);
    const ConfigPlugin& getConfigPlugin() const { return m_configPlugin; }
    bool isEnabled() const { return m_enabled; }

    bool evalRule(const std::string& assetValues);
    std::string getReason() const;
    std::string getTriggers() const;

private:
    ConfigPlugin             m_configPlugin;
    mutable std::mutex       m_configMutex;
    std::atomic<bool>        m_enabled{false};
    std::string              m_asset;
    std::string              m_reason;
};
};

#endif  // INCLUDE_FILTER_SYSTEM_SP_H_