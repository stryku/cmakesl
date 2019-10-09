import sys
import os.path
import json

FILE_TEMPLATE = """
#pragma once

#include <common/string.hpp>
#include <lexer/token.hpp>

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
    
    %methods%

private:
    source_view get_source_view() const
    {
        return m_path_to_documentation.has_value()
            ? source_view{ *m_path_to_documentation, %type_name%_documentation_source }
            : source_view{ %type_name%_documentation_source };
    }
    
private:
    std::optional<cmsl::string_view> m_path_to_documentation;
};
"""

TOKEN_PROVIDER_METHOD_TEMPLATE = """
    lexer::token %token_name%() const
    {
        const auto src_range = source_range{ source_location{ %line%, %column%, %absolute_position% },
                                             source_location{ %line%, %column% + %token_length%, %absolute_position% + %token_length% } };
        return lexer::token{ lexer::token_type::%token_type%, src_range, get_source_view()};
    }
"""

GENERATION_INFO = {
    'bool': {
        'tokens': {
            'name': ['class bool', 'class ', 'bool', 'kw_bool'],
            'default_constructor': ['bool();', '', 'bool'],
            'copy_constructor': ['bool(bool value);', '', 'bool'],
            'conversion_from_int_constructor': ['bool(int value);', '', 'bool'],
            'operator_equal': ['bool& operator=(bool rhs);', 'bool& operator', '='],
            'operator_equal_equal': ['bool operator==(bool rhs);', 'bool operator', '=='],
            'operator_pipe_pipe': ['bool operator||(bool rhs);', 'bool operator', '||'],
            'operator_amp_amp': ['bool operator&&(bool rhs);', 'bool operator', '&&'],
            'operator_unary_exclaim': ['bool operator!();', 'bool operator', '!'],
            'to_string': ['string to_string();', 'string ', 'to_string']
        }
    },
    'int': {
        'tokens': {
            'name': ['class int', 'class ', 'int', 'kw_int'],
            'default_constructor': ['int();', '', 'int'],
            'copy_constructor': ['int(int value);', '', 'int'],
            'conversion_from_bool_constructor': ['int(bool value);', '', 'int'],
            'conversion_from_double_constructor': ['int(double value);', '', 'int'],
            'to_string': ['string to_string();', 'string ', 'to_string'],
            'operator_plus': ['int operator+(int rhs);', 'int operator', '+'],
            'operator_unary_plusplus': ['int& operator++();', 'int& operator', '++'],
            'operator_minus': ['int operator-(int rhs);', 'int operator', '-'],
            'operator_unary_minus': ['int operator-();', 'int operator', '-'],
            'operator_unary_minusminus': ['int& operator--();', 'int& operator', '--'],
            'operator_star': ['int operator*(int rhs);', 'int operator', '*'],
            'operator_slash': ['int operator/(int rhs);', 'int operator', '/'],
            'operator_equal': ['int& operator=(int rhs);', 'int& operator', '='],
            'operator_plus_equal': ['int& operator+=(int rhs);', 'int& operator', '+='],
            'operator_minus_equal': ['int& operator-=(int rhs);', 'int& operator', '-='],
            'operator_star_equal': ['int& operator*=(int rhs);', 'int& operator', '*='],
            'operator_slash_equal': ['int& operator/=(int rhs);', 'int& operator', '/='],
            'operator_less': ['bool operator<(int rhs);', 'bool operator', '<'],
            'operator_less_equal': ['bool operator<=(int rhs);', 'bool operator', '<='],
            'operator_greater': ['bool operator>(int rhs);', 'bool operator', '>'],
            'operator_greater_equal': ['bool operator>=(int rhs);', 'bool operator', '>='],
            'operator_equal_equal': ['bool operator==(int rhs);', 'bool operator', '=='],

        }
    },
    'double': {
        'tokens': {
            'name': ['class double', 'class ', 'double', 'kw_double'],
            'default_constructor': ['double();', '', 'double'],
            'copy_constructor': ['double(double value);', '', 'double'],
            'conversion_from_int_constructor': ['double(int value);', '', 'double'],
            'operator_plus': ['double operator+(double value);', 'double operator', '+'],
            'operator_unary_plusplus': ['double& operator++();', 'double& operator', '++'],
            'operator_minus': ['double operator-(double value);', 'double operator', '-'],
            'operator_unary_minus': ['double operator-();', 'double operator', '-'],
            'operator_unary_minusminus': ['double& operator--();', 'double& operator', '--'],
            'operator_start': ['double operator*(double value);', 'double operator', '*'],
            'operator_slash': ['double operator/(double value);', 'double operator', '/'],
            'operator_equal': ['double& operator=(double value);', 'double& operator', '='],
            'operator_plus_equal': ['double& operator+=(double value);', 'double& operator', '+='],
            'operator_minus_equal': ['double& operator-=(double value);', 'double& operator', '-='],
            'operator_start_equal': ['double& operator*=(double value);', 'double& operator', '*='],
            'operator_slash_equal': ['double& operator/=(double value);', 'double& operator', '/='],
            'operator_less': ['bool operator<(double value);', 'bool operator', '<'],
            'operator_less_equal': ['bool operator<=(double value);', 'bool operator', '<='],
            'operator_greater': ['bool operator>(double value);', 'bool operator', '>'],
            'operator_greater_equal': ['bool operator>=(double value);', 'bool operator', '>='],
            'to_string': ['string to_string();', 'string ', 'to_string']
        }
    },
    'string': {
        'tokens': {
            'name': ['class string', 'class ', 'string', 'kw_string'],
            'default_constructor': ['string();', '', 'string'],
            'copy_constructor': ['string(string value);', '', 'string'],
            'value_initialization_constructor': ['string(string value, int count);', '', 'string'],
            'empty': ['bool empty();', 'bool ', 'empty'],
            'size': ['int size();', 'int ', 'size'],
            'operator_equal_equal': ['bool operator==(string rhs);', 'bool operator', '=='],
            'operator_not_equal': ['bool operator!=(string rhs);', 'bool operator', '!='],
            'operator_less': ['bool operator<(string rhs);', 'bool operator', '<'],
            'operator_less_equal': ['bool operator<=(string rhs);', 'bool operator', '<='],
            'operator_greater': ['bool operator>(string rhs);', 'bool operator', '>'],
            'operator_greater_equal': ['bool operator>=(string rhs);', 'bool operator', '>='],
            'operator_plus': ['string operator+(string rhs);', 'string operator', '+'],
            'operator_plus_equal': ['string& operator+=(string rhs);', 'string& operator', '+='],
            'clear': ['void clear();', 'void ', 'clear'],
            'insert': ['string& insert(int position, string str);', 'string& ', 'insert'],
            'erase_pos': ['string& erase(int position);', 'string& ', 'erase'],
            'erase_pos_count': ['string& erase(int position, int count);', 'string& ', 'erase'],
            'starts_with': ['bool starts_with(string str);', 'bool ', 'starts_with'],
            'ends_with': ['bool ends_with(string str);', 'bool ', 'ends_with'],
            'replace': ['string& replace(int position, int count, string str);', 'string& ', 'replace'],
            'substr_pos': ['string substr(int position);', 'string ', 'substr'],
            'substr_pos_count': ['string substr(int position, int count);', 'string ', 'substr'],
            'resize_size': ['void resize(int new_size);', 'void ', 'resize'],
            'resize_size_fill': ['void resize(int new_size, string fill);', 'void ', 'resize'],
            'find_str': ['int find(string str);', 'int ', 'find'],
            'find_str_pos': ['int find(string str, int position);', 'int ', 'find'],
            'find_not_of_str': ['int find_not_of(string str);', 'int ', 'find_not_of'],
            'find_not_of_str_pos': ['int find_not_of(string str, int position);', 'int ', 'find_not_of'],
            'find_last': ['int find_last(string str);', 'int ', 'find_last'],
            'find_last_not_of': ['int find_last_not_of(string str);', 'int ', 'find_last_not_of'],
            'contains': ['bool contains(string str);', 'bool ', 'contains'],
            'lower': ['void lower();', 'void ', 'lower'],
            'make_lower': ['string make_lower();', 'string ', 'make_lower'],
            'upper': ['void upper();', 'void ', 'upper'],
            'make_upper': ['string make_upper();', 'string ', 'make_upper']
        }
    },
    'version': {
        'tokens': {
            'name': ['class version', 'class ', 'version', 'kw_version'],
            'constructor_major': ['version(int major);', '', 'version'],
            'constructor_major_minor': ['version(int major, int minor);', '', 'version'],
            'constructor_major_minor_patch': ['version(int major, int minor, int patch);', '', 'version'],
            'constructor_major_minor_patch_tweak': ['version(int major, int minor, int patch, int tweak);', '',
                                                    'version'],
            'operator_equal_equal': ['bool operator==(version rhs);', 'bool operator', '=='],
            'operator_not_equal': ['bool operator!=(version rhs);', 'bool operator', '!='],
            'operator_less': ['bool operator<(version rhs);', 'bool operator', '<'],
            'operator_less_equal': ['bool operator<=(version rhs);', 'bool operator', '<='],
            'operator_greater': ['bool operator>(version rhs);', 'bool operator', '>'],
            'operator_greater_equal': ['bool operator>=(version rhs);', 'bool operator', '>='],
            'major_': ['int major();', 'int ', 'major'],
            'minor_': ['int minor();', 'int ', 'minor'],
            'patch_': ['int patch();', 'int ', 'patch'],
            'tweak_': ['int tweak();', 'int ', 'tweak'],
            'to_string': ['string to_string();', 'string ', 'to_string']
        }
    },
    'list': {
        'tokens': {
            'name': ['class list', 'class ', 'list', 'kw_list'],
            'default_constructor': ['list();', '', 'list'],
            'push_back_value': ['void push_back(value_type value);', 'void ', 'push_back'],
            'push_back_list': ['void push_back(list<value_type> value);', 'void ', 'push_back'],
            'operator_plus_value': ['list<value_type> operator+(value_type value);', 'list<value_type> operator', '+'],
            'operator_plus_list': ['list<value_type> operator+(list<value_type> value);', 'list<value_type> operator',
                                   '+'],
            'operator_plus_equal_value': ['list<value_type>& operator+=(value_type value);',
                                          'list<value_type>& operator', '+='],
            'operator_plus_equal_list': ['list<value_type>& operator+=(list<value_type> value);',
                                         'list<value_type>& operator', '+='],
            'push_front_value': ['void push_front(value_type value);', 'void ', 'push_front'],
            'push_front_list': ['void push_front(list<value_type> value);', 'void ', 'push_front'],
            'pop_back': ['void pop_back();', 'void ', 'pop_back'],
            'pop_front': ['void pop_front();', 'void ', 'pop_front'],
            'at': ['value_type& at(int position);', 'value_type& ', 'at'],
            'front': ['value_type front();', 'value_type ', 'front'],
            'back': ['value_type back();', 'value_type ', 'back'],
            'insert_pos_value': ['void insert(int position, value_type value);', 'void ', 'insert'],
            'insert_pos_list': ['void insert(int position, list<value_type> value);', 'void ', 'insert'],
            'erase_pos': ['void erase(int position);', 'void ', 'erase'],
            'erase_pos_count': ['void erase(int position, int count);', 'void ', 'erase'],
            'remove_value': ['int remove(value_type value);', 'int ', 'remove'],
            'remove_value_count': ['int remove(value_type value, int count);', 'int ', 'remove'],
            'remove_last': ['int remove_last(value_type value, int count);', 'int ', 'remove_last'],
            'clear': ['void clear();', 'void ', 'clear'],
            'resize': ['void resize(int new_size);', 'void ', 'resize'],
            'sort': ['void sort();', 'void ', 'sort'],
            'reverse': ['void reverse();', 'void ', 'reverse'],
            'min': ['int min();', 'int ', 'min'],
            'max': ['int max();', 'int ', 'max'],
            'sublist_pos': ['list<value_type> sublist(int position);', 'list<value_type> ', 'sublist'],
            'sublist_pos_count': ['list<value_type> sublist(int position, int count);', 'list<value_type> ', 'sublist'],
            'size': ['int size();', 'int ', 'size'],
            'empty': ['bool empty();', 'bool ', 'empty'],
            'find_value': ['int find(value_type value);', 'int ', 'find'],
            'find_value_pos': ['int find(value_type value, int position);', 'int ', 'find']
        }
    },
    'extern': {
        'tokens': {
            'name': ['class extern', 'class ', 'extern'],
            'constructor_name': ['extern(string name);', '', 'extern'],
            'has_value': ['bool has_value();', 'bool ', 'has_value'],
            'value': ['ValueType value();', 'ValueType ', 'value'],
        }
    },
    'project': {
        'tokens': {
            'name': ['class project', 'class ', 'project', 'kw_project'],
            'constructor_name': ['project(string name);', '', 'project'],
            'add_executable': ['executable add_executable(string name, list<string> sources);', 'executable ',
                               'add_executable'],
            'add_library': ['library add_library(string name, list<string> sources);', 'library ', 'add_library'],
            'find_library': ['library find_library(string name);', 'library ', 'find_library']
        }
    },
    'library': {
        'tokens': {
            'name': ['class library', 'class ', 'library', 'kw_library'],
            'method_name': ['string name();', 'string ', 'name'],
            'link_to': ['void link_to(library lib);', 'void ', 'link_to'],
            'link_to_lib_visibility': ['void link_to(library lib, visibility v);', 'void ', 'link_to'],
            'include_directories': ['void include_directories(list<string> dirs);', 'void ', 'include_directories'],
            'include_directories_dirs_visibility': ['void include_directories(list<string> dirs, visibility v);', 'void ', 'include_directories']
        }
    },
    'executable': {
        'tokens': {
            'name': ['class executable', 'class ', 'executable', 'kw_executable'],
            'method_name': ['string name();', 'string ', 'name'],
            'link_to': ['void link_to(library lib);', 'void ', 'link_to'],
            'link_to_lib_visibility': ['void link_to(library lib, visibility v);', 'void ', 'link_to'],
            'include_directories': ['void include_directories(list<string> dirs);', 'void ', 'include_directories'],
            'include_directories_dirs_visibility': ['void include_directories(list<string> dirs, visibility v);', 'void ', 'include_directories']
        }
    },
    'void': {
        'tokens': {
            'name': ['class void', 'class ', 'void', 'kw_void']
        }
    },
    'option': {
        'tokens': {
            'name': ['class option', 'class ', 'option', 'kw_option'],
            'constructor_description': ['option(string name, string description);', '', 'option'],
            'constructor_description_value': ['option(string name, string description, bool value);', '', 'option'],
            'value': ['bool value();', 'bool ', 'value']
        }
    },
    'cmake': {
        'tokens': {
            'name': ['namespace cmake', 'namespace ', 'cmake'],
            'module_path': ['list<string> module_path;', 'list<string> ', 'module_path'],
            'minimum_required': ['void minimum_required(version v);', 'void ', 'minimum_required'],
            'message': ['void message(string msg);', 'void ', 'message'],
            'warning': ['void warning(string msg);', 'void ', 'warning'],
            'error': ['void error(string msg);', 'void ', 'error'],
            'fatal_error': ['void fatal_error(string msg);', 'void ', 'fatal_error'],

            'cxx_compiler_id_name': ['enum cxx_compiler_id', 'enum ', 'cxx_compiler_id'],
            'cxx_compiler_id_clang': ['clang', '', 'clang'],
            'cxx_compiler_info_name': ['class cxx_compiler_info', 'class ', 'cxx_compiler_info'],
            'cxx_compiler_info_id': ['cxx_compiler_id id;', 'cxx_compiler_id ', 'id'],
            'get_cxx_compiler_info': ['cxx_compiler_info get_cxx_compiler_info();', 'cxx_compiler_info ', 'get_cxx_compiler_info'],

            'cxx_standard_value_name': ['enum cxx_standard_value', 'enum ', 'cxx_standard_value'],
            'cxx_standard_value_cpp_11': ['cpp_11,', '', 'cpp_11'],
            'cxx_standard_value_cpp_14': ['cpp_14,', '', 'cpp_14'],
            'cxx_standard_value_cpp_17': ['cpp_17,', '', 'cpp_17'],
            'cxx_standard_value_cpp_20': ['cpp_20', '', 'cpp_20'],
            'cxx_standard': ['cxx_standard_type cxx_standard;', 'cxx_standard_type ', 'cxx_standard'],

            'install_executable': ['void install(executable exe);', 'void ', 'install'],
            'install_executable_destination': ['void install(executable exe, string destination);', 'void ', 'install'],
            'install_library': ['void install(library lib);', 'void ', 'install'],
            'install_library_destination': ['void install(library lib, string destination);', 'void ', 'install'],

            'enable_ctest': ['void enable_ctest();', 'void ', 'enable_ctest'],
            'add_test': ['void add_test(executable exe);', 'void ', 'add_test'],

            'visibility': ['enum visibility', 'enum ', 'visibility'],
            'visibility_interface': ['interface,', '', 'interface'],
            'visibility_private': ['private,', '', 'private'],
            'visibility_public': ['public', '', 'public'],

            'root_source_dir': ['string root_source_dir();', 'string ', 'root_source_dir'],
            'current_binary_dir': ['string current_binary_dir();', 'string ', 'current_binary_dir'],

            'add_custom_command': ['void add_custom_command(list<string> command, string output);', 'void ', 'add_custom_command'],
            'make_directory': ['void make_directory(string dir);', 'void ', 'make_directory'],
            'set_old_style_variable': ['void set_old_style_variable(string name, string value);', 'void ', 'set_old_style_variable'],

            'system_id_name': ['enum system_id', 'enum ', 'system_id'],
            'system_id_windows': ['windows,', '', 'windows'],
            'system_id_unix': ['unix', '', 'unix'],
            'system_info_name': ['struct system_info', 'struct ', 'system_info'],
            'system_info_id': ['system_id id;', 'system_id ', 'id'],
            'get_system_info': ['system_info get_system_info();', 'system_info ', 'get_system_info'],

            'add_custom_target': ['void add_custom_target(string name, list<string> command);', 'void ', 'add_custom_target'],
        }
    }
}


def load_type_documentation(type_name):
    documentation_path = '{}/{}.cmsl'.format(sys.argv[1], type_name)

    if not os.path.exists(documentation_path):
        return None

    with open(documentation_path, 'r') as file:
        return file.read()


def find_absolute(documentation_content, to_search, to_skip):
    found = documentation_content.find(to_search)
    token_absolute_position = found + len(to_skip)
    return token_absolute_position


def find_line_and_column(documentation_content, to_search, to_skip):
    lines = documentation_content.splitlines()
    line_no = 0
    for line in lines:
        line_no = line_no + 1
        if to_search in line:
            column = line.find(to_search) + len(to_skip)
            return line_no, column

    return None


def generate_providers():
    print('Generating providers...')

    generated_providers = ''

    for type_name, type_dict in GENERATION_INFO.items():
        print('Generating provider {}...'.format(type_name))

        type_documentation = load_type_documentation(type_name)

        provider_source = TOKENS_PROVIDER_TEMPLATE
        provider_source = provider_source.replace('%type_name%', type_name)

        methods_source = ''

        for token, token_search_info in type_dict['tokens'].items():
            print('Generating provider {}::{}...'.format(type_name, token))
            method_source = TOKEN_PROVIDER_METHOD_TEMPLATE
            method_source = method_source.replace('%token_name%', token)

            to_search = token_search_info[0]
            to_skip = token_search_info[1]
            token_value = token_search_info[2]
            token_type = token_search_info[3] if len(token_search_info) > 3 else 'identifier'
            token_absolute_position = find_absolute(type_documentation, to_search, to_skip)
            token_line, token_column = find_line_and_column(type_documentation, to_search, to_skip)
            token_length = len(token_value)
            method_source = method_source.replace('%absolute_position%', str(token_absolute_position))
            method_source = method_source.replace('%line%', str(token_line))
            method_source = method_source.replace('%column%', str(token_column))
            method_source = method_source.replace('%token_length%', str(token_length))
            method_source = method_source.replace('%token_type%', str(token_type))

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

    file_content = FILE_TEMPLATE. \
        replace('%documentation_string_views%', documentation_string_views). \
        replace('%token_providers%', providers)

    destination_path = sys.argv[2]
    with open(destination_path, 'w') as file:
        file.write(file_content)


main()
