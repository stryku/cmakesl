import sys
import os.path
import json


FILE_TEMPLATE = """
#pragma once

%documentation_string_views%

namespace cmsl::sema
{
    %token_providers%
}
"""

TYPE_DOCUMENTATION_STRING_VIEW_TEMPLATE = """
static const cmsl::string_view %type_name%_documentation_source{ %documentation_content% };
"""

TOKENS_PROVIDER_TEMPLATE = """
class %type_name%_tokens_provider
{
public:
    explicit %type_name%_tokens_provider(std::optional<cmsl::string_view> path_to_documentation)
        : m_path_to_documentation{ path_to_documentation }
    {}

    source_view get_source_view() const
    {
        return source_view{ m_path_to_documentation, %type_name%_documentation_source };
    }
    
    %methods%

private:
    source_view get_source_view() const
    {
        return m_path_to_documentation.has_value()
            ? source_view{ m_path_to_documentation, %type_name%_documentation_source }
            : source_view{ %type_name%_documentation_source }
    }
    
private:
    std::optional<cmsl::string_view> m_path_to_documentation;
};
"""

TOKEN_PROVIDER_METHOD_TEMPLATE = """
    lexer::token %token_name%() const
    {
        const auto src_range = source_range{ source_location{ 0, 0, %absolute_position% },
                                             source_location{ 0, 0, %absolute_position% + %token_length% } };
        return lexer::token{ lexer::token_type::identifier, src_range, get_source_view()};
    }
"""

GENERATION_INFO = {
    'bool': {
        'tokens': {
            'name' : ['class bool', 'class ', 'bool'],
            'default_constructor' : ['bool();', '', 'bool']
        }
    }
}


def load_type_documentation(type_name):
    documentation_path =  '{}/{}.cmsl'.format(sys.argv[1], type_name)

    if not os.path.exists(documentation_path):
        return Null

    with open(documentation_path, 'r') as file:
        return file.read()


def generate_providers():
    print('Generating providers...')

    generated_providers = ''

    for type_name, type_dict in GENERATION_INFO.items():
        print('Generating provider {}, {}...'.format(type_name, str(type_dict)))

        type_documentation = load_type_documentation(type_name)

        provider_source = TOKENS_PROVIDER_TEMPLATE
        provider_source = provider_source.replace('%type_name%', type_name)

        methods_source = ''

        for token, token_search_info in type_dict['tokens'].items():
            print('Generating provider token {}, {}...'.format(token, str(token_search_info)))
            method_source = TOKEN_PROVIDER_METHOD_TEMPLATE
            method_source = method_source.replace('%token_name%', token)

            to_search = token_search_info[0]
            to_skip = token_search_info[1]
            token_value = token_search_info[2]
            found = type_documentation.find(to_search)
            token_absolute_position = found + len(to_skip)
            token_length = len(token_value)
            method_source = method_source.replace('%absolute_position%', str(token_absolute_position))
            method_source = method_source.replace('%token_length%', str(token_length))

            methods_source += method_source

        provider_source = provider_source.replace('%methods%', methods_source)

        generated_providers += provider_source

    return generated_providers


def generate_documentation_source_views():
    generated_source_views = ''

    for type_name, type_dict in GENERATION_INFO.items():
        type_documentation = load_type_documentation(type_name)
        string_view_source = TYPE_DOCUMENTATION_STRING_VIEW_TEMPLATE

        string_view_source = string_view_source.replace('%type_name%', type_name)

        escaped_documentation = json.dumps(type_documentation)
        string_view_source = string_view_source.replace('%documentation_content%', escaped_documentation)

        generated_source_views += string_view_source

    return generated_source_views


def main():
    documentation_string_views = generate_documentation_source_views()
    providers = generate_providers()

    file_content = FILE_TEMPLATE.\
        replace('%documentation_string_views%', documentation_string_views).\
        replace('%token_providers%', providers)

    destination_path = '{}/builtin_token_providers.hpp'.format(sys.argv[2])
    with open(destination_path, 'w') as file:
        file.write(file_content)


main()