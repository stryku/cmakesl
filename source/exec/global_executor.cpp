#include "exec/global_executor.hpp"
#include "exec/source_compiler.hpp"
#include "exec/compiled_source.hpp"
#include "sema/user_sema_function.hpp"
#include "exec/execution2.hpp"

#include "cmake_facade.hpp"


#include <fstream>
#include <iterator>


namespace cmsl::exec
{
    global_executor::directory_guard::directory_guard(facade::cmake_facade& cmake_facade, const std::string &dir)
            : m_cmake_facade{ cmake_facade }
    {
        m_cmake_facade.go_into_subdirectory(dir);
    }
    global_executor::directory_guard::~directory_guard()
    {
        m_cmake_facade.go_directory_up();
    }

    global_executor::global_executor(const std::string& root_path, facade::cmake_facade &cmake_facade)
        : m_root_path { root_path }
        , m_cmake_facade{ cmake_facade }
    {
        m_cmake_facade.go_into_subdirectory(m_root_path);
    }

    global_executor::~global_executor() = default;

    int global_executor::execute(std::string source)
    {
        source_compiler compiler{ m_errors_observer,
                                  m_type_factory,
                                  m_function_factory,
                                  m_context_factory,
                                  *this };

        const auto src_view = source_view{ source };

        m_sources.emplace_back(std::move(source));
        auto compiled = compiler.compile(src_view);
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

    const sema::sema_function *
    global_executor::handle_add_subdirectory(cmsl::string_view name, const std::vector<std::unique_ptr<sema::expression_node>>& params)
    {
        directory_guard dg{ m_cmake_facade, std::string{ name } };

        auto path = m_cmake_facade.current_directory() + "/CMakeLists.cmsl";
        std::ifstream file{ path };
        if(!file.is_open())
        {
            // Todo: script not found
            return nullptr;
        }
        cmsl::string_view path_view = path;
        m_paths.emplace_back(std::move(path));

        std::string source(std::istreambuf_iterator<char>{file}, {});
        const auto src_view = source_view{ path_view, source };

        m_sources.emplace_back(std::move(source));

        source_compiler compiler{ m_errors_observer,
                               m_type_factory,
                               m_function_factory,
                               m_context_factory,
                               *this };

        auto compiled = compiler.compile(src_view);
        const auto main_function = compiled->get_main();
        const auto& global_context = compiled->get_global_context();

        // Todo: hanlde no main

        m_compiled_sources.emplace_back(std::move(compiled));

        //Todo: handle not matching params.
        return main_function;
    }
}
