#pragma once

#include "symbols.hpp"
#include "../diagnostics/diagnostic.hpp"
#include "../helper.hpp"
#include <istream>

namespace plpp {

struct PlppContext {
    plc::GlobalPosition pos;
    char last;
    bool is_waiting_for_newline = false;
};

class Plpp {
    std::istream& source;
    PlppContext context;

    void update_position() {
        if(this->context.last == plc::ReservedSymbols::sym_oel) {
            this->context.pos.local.line++;
            this->context.pos.local.column = 1;
        }
    }

    void skip_whitespace() {
        char ch = this->source.peek();
        
        while(is_space(this->context.last) && is_space(ch) 
            || this->context.is_waiting_for_newline) {
            this->context.pos.local.column++;
            this->context.last = ch;
            ch = this->source.get();
            this->context.is_waiting_for_newline = 
                ch == plpp::start_of_comment 
                && !this->context.is_waiting_for_newline;
        }

        this->update_position();
    }

public:
    Plpp(std::istream& in) : source(in) {};

    int peek() {
        this->skip_whitespace();

        return this->source.peek();
    }

    int get() {
        this->skip_whitespace();

        int ch = this->source.get();
        this->context.last = ch;

        return ch;
    }

    plc::GlobalPosition get_current_position() { return this->context.pos; }
};

} // namespace plpp
