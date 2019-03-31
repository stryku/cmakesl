#include "exec/global_executor.hpp"
#include "exec/source_compiler.hpp"
#include "exec/compiled_source.hpp"
#include "sema/user_sema_function.hpp"
#include "exec/execution2.hpp"
#include "global_executor.hpp"


namespace cmsl::exec
{
    global_executor::global_executor(facade::cmake_facade &cmake_facade)
        :m_cmake_facade{ cmake_facade }
    {}

    global_executor::~global_executor() = default;

    int global_executor::execute(source_view source)
    {
        source_compiler compiler{ m_errors_observer,
                                  m_type_factory,
                                  m_function_factory,
                                  m_context_factory };

        auto compiled = compiler.compile(source);
        const auto main_function = compiled->get_main();
        const auto& global_context = compiled->get_global_context();

        // Todo: hanlde no main

        m_compiled_sources.emplace_back(std::move(compiled));

        const auto casted = dynamic_cast<const sema::user_sema_function*>(main_function);

        execution2 e{ m_cmake_facade };

        inst::instances_holder instances{ global_context };
        auto main_result = e.call(*casted, {}, instances);
        return main_result->get_value_cref().get_int();
    }
}
