//todo remove this
//#include "exec/fundamental_instance.hpp"
//
//namespace cmsl
//{
//    namespace exec
//    {
//        fundamental_instance::fundamental_instance(const ast::type& type)
//            : unnamed_instance{ type }
//            , m_value{ 0u }
//        {}
//
//        fundamental_instance::fundamental_instance(const ast::type& type, int value)
//            : unnamed_instance{ type }
//            , m_value{ value }
//        {}
//
//        int fundamental_instance::get_value() const
//        {
//            return m_value;
//        }
//
//        void fundamental_instance::assign(std::unique_ptr<fundamental_instance> val)
//        {
//            m_value = val->get_value();
//        }
//
//        void fundamental_instance::assign(int val)
//        {
//            m_value = val;
//        }
//    }
//}
