#pragma once

#include "ast.hpp"
#include "../tokenizer/tokenizer.hpp"
#include "../diagnostics/diagnostic.hpp"
#include "inspector.hpp"
#include "semantics.hpp"
#include <vector>

namespace plp {

class Parser {
    plc::Tokenizer& tokenizer;
    Inspector inspector;
    program_t program;

    std::vector<plc::Token> line;
    bool eof_seen = false;

    void fill_line() {
        this->line.clear();
        plc::Token last = this->tokenizer.next();
        while(last.category != plc::TokenCategory::Eol) {
            if(last.category == plc::TokenCategory::Eof) {
                this->eof_seen = true;
                return;
            }
            this->line.push_back(std::move(last));
        }
    }

    std::optional<Declaration> parse_declaration() {

    }

    std::optional<Operation> parse_operation() {

    }

    std::optional<statement_t> parse_line() {
        plc::Diagnostic what;
        StatementType type = StatementRecognizer::recognize(this->line, what);
        switch(type) {
            case StatementType::Declaration: return this->parse_declaration();
            case StatementType::Operation: return this->parse_operation();
            default:
                this->inspector.note(what);
                return std::nullopt;
        }
    }

public:
    explicit Parser(plc::Tokenizer& tk, InspectorConfig config)
        : tokenizer(tk), inspector(config) {}

    void parse_program() {
        while(!this->finished()) {
            this->parse_next();
        }
    }

    void parse_next() {
        this->fill_line();

        auto statement = this->parse_line();
        if(statement.has_value()) {
            StatementType type = static_cast<StatementType>(statement->index());
            switch(type) {
                case StatementType::Declaration: 
                    this->program.push_back(
                        std::move(
                            std::get<Declaration>(
                                statement.value()
                            )
                        )
                    ); 
                    break;
                
                case StatementType::Operation:
                    this->program.push_back(
                        std::move(
                            std::get<Operation>(
                                statement.value()
                            )
                        )
                    ); 
                    break;
                
                default:
                    break;
            }
        }
    }

    const program_t& get_program() const {
        return  this->program;
    }

    bool finished() const {
        return this->eof_seen;
    }

    const std::vector<plc::Diagnostic>& get_diagnostics() const {
        return this->inspector.get_diagnostics();
    }
};

} // namespace plp
