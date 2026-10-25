#pragma once

#include "../ir/format.hpp"
#include <vector>

namespace pl {

using ip_t = std::size_t;

struct SessionContext {
    ip_t ip; // instruction pointer
};

struct Session {
    Module module;
    SessionContext context;

    Session(Module module)
        : module(module), context()
        {}
};

class Workspace {
    std::vector<Session> sessions;
    std::size_t current_session = 0;

public:
    Workspace(std::vector<Module> modules)
        : sessions(modules.begin(), modules.end()) {}

    const std::size_t sessions_count() const {
        return this->sessions.size();
    }

    const Module& get_module(std::size_t idx) const {
        return this->sessions.at(idx).module;
    }

    Module& get_module(std::size_t idx) {
        return this->sessions.at(idx).module;
    }

    const Session& get_session(std::size_t idx) const {
        return this->sessions.at(idx);
    }

    Session& get_session(std::size_t idx) {
        return this->sessions.at(idx);
    }

    const SessionContext& get_context(std::size_t idx) const {
        return this->sessions.at(idx).context;
    }

    SessionContext& get_context(std::size_t idx) {
        return this->sessions.at(idx).context;
    }

    const Session& get_current_session() const {
        return this->get_session(this->current_session);
    }

    Session& get_current_session() {
        return this->get_session(this->current_session);
    }

    std::size_t get_current_session_index() const {
        return this->current_session;
    }

    void switch_session(std::size_t idx) {
        this->current_session = idx;
    }

};

} // namespace pl
