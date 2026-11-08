#pragma once

#include "../tokenizer/token.hpp"
#include "../diagnostics/diagnostic.hpp"
#include <array>
#include <span>
#include <vector>

#define ALT_INN true
#define ALT_OUT false

#define OPT_INN true
#define OPT_OUT false

namespace pl::bnf {

enum class Scope {
    OptOpen,    // [
    OptClose,   // ]
    AltOpen,    // <
    AltClose,   // >
    Fix,        // token
};

using TK = plc::TokenCategory;
using SC = Scope;

struct Term {
    Scope scope;
    TK category{};
};

using Rule = std::span<const Term>;

struct RuleRepo {
    // operation := keyword [<identifier | literal> [<identifier | literal>]]
    static constexpr Term operation_terms[] = {
        {SC::Fix, TK::Keyword},
        {SC::OptOpen},
            {SC::AltOpen},
                {SC::Fix, TK::Identifier},
                {SC::Fix, TK::Literal},
            {SC::AltClose},
            {SC::OptOpen},
                {SC::AltOpen},
                    {SC::Fix, TK::Identifier},
                    {SC::Fix, TK::Literal},
                {SC::AltClose},
            {SC::OptClose},
        {SC::OptClose},
    };

    // declaration := . identifier [<identifier | literal>]
    static constexpr Term declaration_terms[] = {
        {SC::Fix, TK::Period},
        {SC::Fix, TK::Identifier},
        {SC::OptOpen},
            {SC::AltOpen},
                {SC::Fix, TK::Identifier},
                {SC::Fix, TK::Literal},
            {SC::AltClose},
        {SC::OptClose},
    };

    static constexpr std::array<Rule, 2> rules = {
        Rule{operation_terms},
        Rule{declaration_terms},
    };
};

inline constexpr bool bnf_assert(Rule rule) {
    // TODO: implement
    return true;
}

constexpr bool all_valid() {
    for (Rule r : RuleRepo::rules)
        if (!bnf_assert(r)) return false;
    return true;
}
static_assert(all_valid(), "BNF rule is malformed");

inline bool definition_match(const std::vector<plc::Token>& sentence, Rule rule, plc::Diagnostic& what) {
    // TODO: implement
    return false;
}

} // namespace pl::bnf