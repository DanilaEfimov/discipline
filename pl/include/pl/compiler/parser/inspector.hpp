#pragma once

#include "../diagnostics/diagnostic.hpp"
#include <vector>

namespace plp {

enum class InspectionLevel {
    None,
    Verbose,
    All,
};

struct InspectorConfig {
    InspectionLevel level;
};

class Inspector {
    std::vector<plc::Diagnostic> diags;
    InspectorConfig config;

public:
    Inspector(InspectorConfig cfg) : config(cfg) {}

    void note(plc::Diagnostic& diag) {
        if(this->config.level == InspectionLevel::None) return;
        if(this->config.level == InspectionLevel::Verbose 
            && diag.severity != plc::Severity::Note) {
            this->diags.push_back(std::move(diag));
            return;
        }
        if(this->config.level == InspectionLevel::All) {
            this->diags.push_back(std::move(diag));
            return;
        }
    }

    const InspectorConfig& get_config() const {
        return this->config;
    }

    const std::vector<plc::Diagnostic>& get_diagnostics() const {
        return this->diags;
    }
};

} // namespace plp
