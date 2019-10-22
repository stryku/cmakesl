import subprocess
import os


CMAKE_BINARY_PATH = os.path.expanduser('~/integration_tests/install/bin/cmake')
INTEGRATION_TESTS_DIR = os.path.expanduser('~/integration_tests')
CMAKESL_TEST_DIR = os.path.join(INTEGRATION_TESTS_DIR, 'cmakesl_test')


def clone_cmakesl():
    print('Cloning cmakesl...')
    cmakesl_path = os.path.join(CMAKESL_TEST_DIR, 'cmakesl')
    command = [
        'git',
        'clone',
        'https://github.com/stryku/cmakesl',
        cmakesl_path
    ]
    subprocess.check_output(command)


def build_cmakesl():
    cmakesl_path = os.path.join(CMAKESL_TEST_DIR, 'cmakesl')
    cmakesl_build_path = os.path.join(CMAKESL_TEST_DIR, 'build')
    cmakesl_install_path = os.path.join(CMAKESL_TEST_DIR, 'install')

    os.makedirs(cmakesl_build_path)

    command = [
        CMAKE_BINARY_PATH,
        cmakesl_path,
        '-Wno-dev',
        '-DCMAKE_CXX_COMPILER=clang++-8',
        '-DCMAKE_C_COMPILER=clang-8',
        '-DCMAKESL_WITH_TESTS=ON',
        '-DCMAKESL_WITH_TOOLS=ON',
        '-DCMAKESL_WITH_EXAMPLES=ON',
        '-DCMAKE_INSTALL_PREFIX={}'.format(cmakesl_install_path)
    ]
    print('Configuring cmakesl using cmakesl {}...'.format(command))
    subprocess.check_output(command, cwd=cmakesl_build_path)

    command = ['make', '-j']
    print('Building cmakesl using cmakesl {}...'.format(command))
    subprocess.check_output(command, cwd=cmakesl_build_path)

    print('ctest...')
    command = ['ctest', '-R', 'ast_test|ast_error_test|errors_observer_test|exec_test|lexer_test|lexer_error_test|sema_test|source_location_manipulator_test']
    subprocess.check_output(command, cwd=cmakesl_build_path)

    print('make install...')
    command = ['make','install']
    subprocess.check_output(command, cwd=cmakesl_build_path)


def run_hello_world():
    cmakesl_install_path = os.path.join(CMAKESL_TEST_DIR, 'install', 'bin')
    hello_world_file_path = os.path.join(cmakesl_install_path, 'CMakeLists.cmsl')

    with open(hello_world_file_path, 'w') as f:
        f.write('int main() { cmake::message("Hello CMakeSL World!"; return 0; }')

    command = ['./cmakesl', hello_world_file_path]
    subprocess.check_output(command, cwd=cmakesl_install_path)


def build_cmakesl_using_cmakesl():
    clone_cmakesl()
    build_cmakesl()
    run_hello_world()

if __name__ == 'main':
    build_cmakesl_using_cmakesl()
    print('Everything succeed')
