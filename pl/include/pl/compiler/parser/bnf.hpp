#pragma once

#include "../tokenizer/token.hpp"
#include "../diagnostics/diagnostic.hpp"
#include <array>
#include <span>
#include <vector>
#include <iterator>

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
    Sep,        // |
};

using TK = plc::TokenCategory;
using SC = Scope;

struct Term {
    Scope scope;
    TK category = TK::Unknown;
};

using Rule = std::span<const Term>;

struct RuleRepo {

    enum class RuleName {
        Operation,
        Declaration,
    };

    // operation := keyword [<identifier | literal> [<identifier | literal>]]
    static constexpr Term operation_terms[] = {
        {SC::Fix, TK::Keyword},
        {SC::OptOpen},
            {SC::AltOpen},
                {SC::Fix, TK::Identifier},
                {SC::Sep},
                {SC::Fix, TK::Literal},
            {SC::AltClose},
            {SC::OptOpen},
                {SC::AltOpen},
                    {SC::Fix, TK::Identifier},
                    {SC::Sep},
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
                {SC::Sep},
                {SC::Fix, TK::Literal},
            {SC::AltClose},
        {SC::OptClose},
    };

    // ! order has to match RuleName order
    static constexpr std::array<Rule, 2> rules = {
        Rule{operation_terms},
        Rule{declaration_terms},
    };
};

inline constexpr bool bnf_assert(Rule rule) {
    // * Checks:
    // * every Optional block has to be balanced
    // * every Alternative block has to be balanced
    // * every Fixed term has to be non-unknown category
    // * opt block has to be closed by OptClose
    // * alt block has to be closed by AltClose

    int top = 0;
    int cur = 0;
    int alt_level = 0;
    bool inside_alt = false;
    std::array<SC, 64> scopes;

    for(auto& term : rule) {
        switch(term.scope) {
            case SC::OptOpen: {
                if(top >= std::size(scopes)) 
                    throw "bnf_assert: rule is too long";
                scopes[top++] = term.scope;
            } break;
            case SC::AltOpen: {
                if(top >= std::size(scopes)) 
                    throw "bnf_assert: rule is too long";
                scopes[top++] = term.scope;
                alt_level++;
                inside_alt = true;
            } break;
            case SC::AltClose: {
                if(top <= 0 || scopes[top - 1] != SC::AltOpen)
                    throw "bnf_assert: unbalanced alternative block";
                top--;
                alt_level--;
                inside_alt = alt_level > 0;
            } break;
            case SC::OptClose: {
                if(top <= 0 || scopes[top - 1] != SC::OptOpen)
                    throw "bnf_assert: unbalanced optional block";
                top--;
            } break;
            case SC::Fix: {
                if(term.category == TK::Unknown)
                    throw "bnf_assert: unknown token category";
            } break;
            case SC::Sep: {
                if(cur <= 0 || !inside_alt || rule[cur - 1].scope != SC::Fix)
                    throw "bnf_assert: nothing to separate";
            } break;
            default: break;
        }
        cur++;
    }

    if(top != 0)
        throw "bnf_assert: unclosed block at end of rule";
    return true;
}

namespace {

constexpr bool all_valid() {
    for (Rule r : RuleRepo::rules)
        if (!bnf_assert(r)) return false;
    return true;
}

static_assert(all_valid(), "BNF rule is malformed");

} // anonymous namespace

struct BnfTree {

static bool definition_match(const std::vector<plc::Token>& sentence, RuleRepo::RuleName rule, plc::Diagnostic& what) {
    const Rule r = RuleRepo::rules[static_cast<std::size_t>(rule)];

    return false;
}

};

} // namespace pl::bnf