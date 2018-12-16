#include "ast/ast_node_visitor.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/return_node.hpp"
#include "ast/block_node.hpp"
#include "ast/class_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/translation_unit_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "ast/while_node.hpp"

#include <string>

namespace cmsl
{
    namespace ast
    {
        namespace test
        {
            class ast_tree_representation_visitor : public ast_node_visitor
            {
            public:
                virtual void visit(const block_node& node) override
                {
                    m_result += "block{";
                    for(auto n : node.get_expressions())
                    {
                        n->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const class_node2& node)
                {
                    m_result += "class{name:" + node.get_name().str().to_string() + ";members:";

                    for(auto n : node.get_nodes())
                    {
                        n->visit(*this);
                    }

                    m_result += "}";
                }

                virtual void visit(const conditional_node& node) override
                {
                    m_result += "conditional{condition:";
                    node.get_condition().visit(*this);
                    m_result += ";block:";
                    node.get_block().visit(*this);
                    m_result += "}";
                }

                virtual void visit(const if_else_node& node) override
                {
                    m_result += "if_else{ifs:";
                    for(const auto& if_ : node.get_ifs())
                    {
                        if_->visit(*this);
                    }
                    m_result += ";else:";
                    node.get_else()->visit(*this);
                    m_result += "}";
                }

                virtual void visit(const binary_operator_node& node) override
                {
                    m_result += "binary_operator{lhs:";
                    node.get_lhs().visit(*this);
                    m_result += ";operator:" + node.get_operator().str().to_string() + ";rhs:";
                    node.get_rhs().visit(*this);
                    m_result += "}";
                }

                virtual void visit(const class_member_access_node& node) override
                {
                    m_result += "class_member_access{lhs:";
                    node.get_lhs().visit(*this);
                    m_result += ";member_name:" + node.get_member_name().str().to_string() + "}";
                }

                virtual void visit(const function_call_node& node) override
                {
                    m_result += "function_call{name:" + node.get_name().str().to_string() + ";params:";
                    for(const auto& param : node.get_param_nodes())
                    {
                        param->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const member_function_call_node& node) override
                {
                    m_result += "function_call{name:" + node.get_name().str().to_string() + ";params:";
                    m_result += "function_call{lhs:";
                    node.get_lhs().visit(*this);
                    m_result += ";name:" + node.get_name().str().to_string() + ";params:";
                    for(const auto& param : node.get_param_nodes())
                    {
                        param->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const bool_value_node& node) override
                {
                    m_result += "bool_value{" + node.get_token().str().to_string() + "}";
                }

                virtual void visit(const int_value_node& node) override
                {
                    m_result += "int_value{" + node.get_token().str().to_string() + "}";
                }

                virtual void visit(const double_value_node& node) override
                {
                    m_result += "double_value{" + node.get_token().str().to_string() + "}";
                }

                virtual void visit(const string_value_node& node) override
                {
                    m_result += "string_value{" + node.get_token().str().to_string() + "}";
                }

                virtual void visit(const id_node& node) override
                {
                    m_result += "id{" + node.get_identifier().str().to_string() + "}";
                }

                virtual void visit(const return_node& node) override
                {
                    m_result += "return{";
                    node.get_expression().visit(*this);
                    m_result += "}";
                }

                virtual void visit(const translation_unit_node& node) override
                {
                    m_result += "translation_unit{";
                    for(const auto n : node.get_nodes())
                    {
                        n->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const user_function_node2& node) override
                {
                    m_result += "user_function{return_type:";
                    const auto ret_type_reference = node.get_return_type_reference();
                    m_result += ret_type_reference.to_string().to_string() + ";name:";
                    m_result += node.get_name().str().to_string() + ";params:";
                    for(const auto& param_declaration : node.get_param_declarations())
                    {
                        m_result += "param_declaration{type:" + param_declaration.ty.to_string().to_string();
                        m_result += ";name:" + param_declaration.name.str().to_string() + "}";
                    }
                    m_result += ";body:";
                    node.get_body().visit(*this);
                    m_result += "}";
                }

                virtual void visit(const variable_declaration_node& node) override
                {
                    m_result += "variable_declaration{type:";
                    const auto ret_type_reference = node.get_type_reference();
                    m_result += ret_type_reference.to_string().to_string() + ";name:";
                    m_result += node.get_name().str().to_string() + ";initialization:";
                    if(const auto init = node.get_initialization())
                    {
                        init->visit(*this);
                    }
                    m_result += "}";
                }

                virtual void visit(const while_node& node) override
                {
                    m_result += "while{";
                    node.get_node().visit(*this);
                    m_result += "}";
                }

                std::string result() const
                {
                    return m_result;
                }

            private:
                std::string m_result;
            };
        }
    }
}
