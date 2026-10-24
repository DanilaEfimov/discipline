#pragma once

#include "workspace.hpp"
#include "arithmetic.hpp"
#include "../ir/format.hpp"
#include <vector>
#include <stdexcept>


namespace pl {

class Runtime {
    using offset_t = int64_t;

    Workspace workspace;
    Arithmetic calculator;

    void shift_ip(std::size_t session_idx, offset_t offset) {
        // ! this function sucks
    }

public:
    Runtime(std::vector<Module> modules)
        : workspace(modules) {}
        
    void set_ip(std::size_t session_idx, ip_t new_ip) noexcept {
        auto& current_context = this->workspace.get_context(session_idx);
        current_context.ip = new_ip;
    }

    void shirt_ip_right(std::size_t session_idx, offset_t offset) {
        this->shift_ip(session_idx, offset);
    }

    void shift_ip_left(std::size_t session_idx, offset_t offset) {
        this->shift_ip(session_idx, -offset);
    }

};

} // namespace pl
