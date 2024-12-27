// #ifndef SYNTAX_TREE_CPP
// #define SYNTAX_TREE_CPP

// #include "SyntaxTree.hpp"
// #include "carry.hpp"

// clef::Statement* clef::SyntaxTree::endStatement() {
//    if (!_buf.size()) { //no nodes yet -> must be an empty statement
//       _buf.emplace_back(Statement(nullptr, true));
//       _root = _buf.begin();
//       _back = _root;

//       _spine.emplace_back(mcsl::pair<astNode*>{_back, nullptr});
//    }


//    for (uint i = _spine.size(); mcsl::carry::SUB(i, 1);) {
//       const auto& curr = _spine[i];
//       switch(curr.first->type()) { //type of most recent scope
//          case NodeType::EXPR    :
//          case NodeType::STMT    :
//             //must be an empty statement
//             _buf.emplace_back(Statement(nullptr,true));
//             break;
//          case NodeType::STMT_SEQ:
//          case NodeType::IDEN    :
//          case NodeType::BLOCK   :
//          case NodeType::TYPE    :
//          case NodeType::LIT     :
//          case NodeType::OBJ     :
//          case NodeType::FUNC    :
//          case NodeType::OP      :
//          case NodeType::TEMPLATE:
//       }
//    }
// }

// #endif //SYNTAX_TREE_CPP