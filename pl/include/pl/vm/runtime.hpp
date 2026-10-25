#pragma once

#include "workspace.hpp"
#include "arithmetic.hpp"
#include "mmu.hpp"
#include "excode.hpp"
#include "../ir/format.hpp"
#include "exceptions.hpp"
#include "decoder.hpp"
#include "executor.hpp"
#include <vector>
#include <ostream>


namespace pl {

class Runtime {
    using offset_t = int64_t;
    using address_t = uint64_t;

    Workspace workspace;
    Arithmetic calculator;
    MemoryManager mmu;
    Executor executor;

    Command select_first_command() {
        // ! this function sucks
    }

    bool is_value_address_out_of_range(const Session& session, address_t address) noexcept {
        offset_t offset = address + sizeof(value_t);
        return this->is_offset_out_of_range(session, offset);
    }

    bool is_offset_out_of_range(const Session& session, offset_t offset) noexcept {
        const std::uint64_t& current_ip = session.context.ip;
        std::uint64_t length = session.module.data.header.length;

        if(offset >= 0) {
            return (offset >= length - current_ip || current_ip >= length);
        } 
        else {
            const std::uint64_t abs_offset = 
                static_cast<std::uint64_t>(-(offset + 1)) + 1;
            return abs_offset > current_ip;
        }

        return false;
    }

    void shift_ip(offset_t offset) {
        auto& current_session = this->workspace.get_current_session();
        std::uint64_t& current_ip = current_session.context.ip;
        std::uint64_t length = current_session.module.data.header.length;
        
        if(this->is_offset_out_of_range(current_session, offset)) 
            throw PlSegmentFault("instruction pointer is out of range");

        if(offset >= 0)
            current_ip += static_cast<std::uint64_t>(offset);
        else {
            const std::uint64_t abs_offset = 
                static_cast<std::uint64_t>(-(offset + 1) + 1);
            current_ip -= abs_offset;
        }
    }

public:
    Runtime(std::vector<Module> modules, std::ostream& out)
        : workspace(modules, out), executor(mmu, calculator, workspace) {}
        
    void set_ip(std::size_t session_idx, ip_t new_ip) noexcept {
        auto& current_context = this->workspace.get_context(session_idx);
        current_context.ip = new_ip;
    }

    void shirt_ip_right(offset_t offset) {
        this->shift_ip(offset);
    }

    void shift_ip_left(offset_t offset) {
        this->shift_ip(-offset);
    }

    void set_field(address_t address, value_t value) {
        auto& current_session = this->workspace.get_current_session();
        auto& current_data = current_session.module.data.bytes;

        if(this->is_value_address_out_of_range(current_session, address)) 
            throw PlSegmentFault("value address is out of range");

        this->mmu.write_literal(value, current_data, address);
    }

    value_t get_field(address_t address) {
        auto& current_session = this->workspace.get_current_session();
        auto& current_data = current_session.module.data.bytes;

        if(this->is_value_address_out_of_range(current_session, address))
            throw PlSegmentFault("value address is out of range");

        return this->mmu.read_literal(current_data, address);
    }

    ExCode run() {
        Command current = this->select_first_command();

        while(current.op != OpCode::End) {
            this->executor.run_command(current);
            current = Decoder::next(this->workspace.get_current_session());
        }
        
        return ExCode::Success;
    };

};

} // namespace pl
