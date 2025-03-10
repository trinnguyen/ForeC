#ifndef FOREC_AST_TERNARY_EXPRESSION_HPP
#define FOREC_AST_TERNARY_EXPRESSION_HPP

/*
 *  TernaryExpression.hpp
 *  
 *
 *  Created by Eugene Kin Chee Yip on 24/12/10.
 *  Copyright 2010 Potential Publishers. All rights reserved.
 *
 */

#include "Node.hpp"

namespace forec {
	namespace ast {
		class TernaryExpression : public Node {
			public:				
				TernaryExpression(Node &node, const std::string &variant = "none");
				TernaryExpression(Node &node0, Node &node1, Node &node2, const std::string &variant = "none");
												
				static int getCount(void);

				void prettyPrint(std::ostream &output);
				
				
			protected:
				static int globalCount;
		};
	}
}

#endif // FOREC_AST_TERNARY_EXPRESSION_HPP
