#ifndef FOREC_AST_BINARY_EXPRESSION_HPP
#define FOREC_AST_BINARY_EXPRESSION_HPP

/*
 *  CastExpression.hpp
 *  
 *
 *  Created by Eugene Kin Chee Yip on 24/12/10.
 *  Copyright 2010 Potential Publishers. All rights reserved.
 *
 */

#include "Node.hpp"

namespace forec {
	namespace ast {
		class BinaryExpression : public Node {
			public:				
				BinaryExpression(Node &node, const std::string &variant = "none");
				BinaryExpression(Node &node0, Node &node1, const std::string &variant = "none");
				BinaryExpression(Node &node0, Node &node1, Node &node2, const std::string &variant = "none");
												
				static int getCount(void);
				
				void updateSymbolTable(const std::string &type = "none", const bool isUsage = false, const bool isRead = false);

				void prettyPrint(std::ostream &output);
				
				
			protected:
				static int globalCount;
		};
	}
}

#endif // FOREC_AST_BINARY_EXPRESSION_HPP
