import subprocess
import os

INTEGRATION_TESTS_DIR = os.path.expanduser('~/integration_tests')

def clone_cmake():
    print('Cloning cmake...')
    cmake_path = os.path.join(INTEGRATION_TESTS_DIR, 'cmake')
    command = [
        'git',
        'clone',
        'https://github.com/stryku/cmake_for_cmakesl',
        cmake_path
    ]
    subprocess.check_output(command)

    command = [
        'git',
        'checkout',
        'cmakesl'
    ]
    subprocess.check_output(command, cwd=cmake_path)


def clone_cmakesl():
    print('Cloning cmakesl...')
    cmakesl_path = os.path.join(INTEGRATION_TESTS_DIR, 'cmake', 'Source', 'cmakesl')
    command = [
        'git',
        'clone',
        'https://github.com/stryku/cmakesl',
        cmakesl_path
    ]
    subprocess.check_output(command)


def build_cmake():
    print('Building cmake...')
    building_path = os.path.join(INTEGRATION_TESTS_DIR, 'build')
    cmake_path = os.path.join(INTEGRATION_TESTS_DIR, 'cmake')

    os.makedirs(building_path)

    command = [
        'cmake',
        cmake_path,
        '-DCMAKE_CXX_COMPILER=clang++-8',
        '-DCMAKE_C_COMPILER=clang-8',
        '-DCMAKESL_WITH_TESTS=ON',
        '-DCMAKESL_WITH_TOOLS=ON',
        '-DCMAKESL_WITH_EXAMPLES=ON',
        '-DCMAKE_INSTALL_PREFIX={}'.format(os.path.join(INTEGRATION_TESTS_DIR, 'install'))
    ]
    print('Configuring cmake {}...'.format(command))
    subprocess.check_output(command, cwd=building_path)

    print('make -j...')
    command = ['make', '-j']
    subprocess.check_output(command, cwd=building_path)

    print('ctest...')
    command = ['ctest', '-R', 'ast_test|ast_error_test|errors_observer_test|exec_test|lexer_test|lexer_error_test|sema_test|source_location_manipulator_test']
    subprocess.check_output(command, cwd=building_path)

    print('make install...')
    command = ['make','install']
    subprocess.check_output(command, cwd=building_path)


def build_cmake_with_cmakesl_support():
    print('Dir: {}'.format(INTEGRATION_TESTS_DIR))
    clone_cmake()
    clone_cmakesl()
    build_cmake()

if __name__ == 'main':
    build_cmake_with_cmakesl_support()
    print('Everything succeed')
