//#pragma once
//
//#include "exec/instance.hpp"
//
//namespace cmsl
//{
//    namespace exec
//    {
//        class fundamental_instance : public instance
//        {
//        public:
//            explicit fundamental_instance(const ast::type& type);
//            explicit fundamental_instance(const ast::type& type, int value);
//
//            virtual int get_value() const;
//            virtual void assign(std::unique_ptr<fundamental_instance> val);
//            virtual void assign(int val);
//
//        private:
//            int m_value;
//        };
//    }
//}
