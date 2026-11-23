#include "../ir/opcode.hpp"
#include "../ir/signatures.hpp"
#include "keywords.hpp"

namespace plc {

inline pl::OpCode mnemonic_to_opcode(int kw_order, pl::OpSignature sign) {
    switch(kw_order) {
        case is_keyword(kw_add): {
            if(sign == op_signature(pl::OpCode::AddLiteral)) return pl::OpCode::AddLiteral;
            if(sign == op_signature(pl::OpCode::AddVar)) return pl::OpCode::AddVar;
        };
        case is_keyword(kw_mul): {
            if(sign == op_signature(pl::OpCode::MulLiteral)) return pl::OpCode::MulLiteral;
            if(sign == op_signature(pl::OpCode::MulVar)) return pl::OpCode::MulVar;
        };
        case is_keyword(kw_div): {
            if(sign == op_signature(pl::OpCode::DivLiteral)) return pl::OpCode::DivLiteral;
            if(sign == op_signature(pl::OpCode::DivVar)) return pl::OpCode::DivVar;
        };
        case is_keyword(kw_eq): {
            if(sign == op_signature(pl::OpCode::EqualLiteral)) return pl::OpCode::EqualLiteral;
            if(sign == op_signature(pl::OpCode::EqualVar)) return pl::EqualVar;
        };
        case is_keyword(kw_lt): {
            if(sign == op_signature(pl::OpCode::LessLiteral)) return pl::OpCode::LessLiteral;
            if(sign == op_signature(pl::OpCode::LessVar)) return pl::OpCode::LessVar;
        };
        case is_keyword(kw_write): {
            if(sign == op_signature(pl::OpCode::WriteLiteral)) return pl::OpCode::WriteLiteral;
            if(sign == op_signature(pl::OpCode::WriteVar)) return pl::OpCode::WriteVar;
        };
        case is_keyword(kw_read): {
            if(sign == op_signature(pl::OpCode::Read)) return pl::OpCode::Read;
        };
        case is_keyword(kw_print): {
            if(sign == op_signature(pl::OpCode::Print)) return pl::OpCode::Print;
        };
        case is_keyword(kw_jmp): {
            if(sign == op_signature(pl::OpCode::JumpLiteral)) return pl::OpCode::JumpLiteral;
            if(sign == op_signature(pl::OpCode::JumpVar)) return pl::OpCode::JumpVar;
        };
        case is_keyword(kw_jmz): {
            if(sign == op_signature(pl::OpCode::JumpIf)) return pl::OpCode::JumpIf;
        };
        case is_keyword(kw_switch): {
            if(sign == op_signature(pl::OpCode::SwitchLiteral)) return pl::OpCode::SwitchLiteral;
            if(sign == op_signature(pl::OpCode::SwitchVar)) return pl::OpCode::SwitchVar;
        };
        case is_keyword(kw_skip): {
            if(sign == op_signature(pl::OpCode::RightLiteral)) return pl::OpCode::RightLiteral;
            if(sign == op_signature(pl::OpCode::RightVar)) return pl::OpCode::RightVar;
        };
        case is_keyword(kw_back): {
            if(sign == op_signature(pl::OpCode::LeftLiteral)) return pl::OpCode::LeftLiteral;
            if(sign == op_signature(pl::OpCode::LeftVar)) return pl::OpCode::LeftVar;
        };
        case is_keyword(kw_end): {
            if(sign == op_signature(pl::OpCode::End)) return pl::OpCode::End;
        };
        default: {
            //TODO: throw invalid mnemonic exception
        }
        //TODO: throw invalid call exception
    }
};

} // namespace plc
