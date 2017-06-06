require 'rake'
require 'fileutils'

THIS_DIR=File.dirname(__FILE__)
BUILD_DIR=THIS_DIR + '/build'
BINARIES_DIR=BUILD_DIR + '/bin'
TESTS_DIR=BUILD_DIR + '/tests'
INTEGRATION_TESTS_DATA_DIR=BUILD_DIR + '/integration-tests'
PYTHON_WRAPPER_DIR=THIS_DIR + '/wrappers/python'
DOTNET_WRAPPER_DIR=THIS_DIR + '/wrappers/dotnet'

def start_test_instance()
    puts "Starting test instance ..."

    FileUtils.rm_rf(INTEGRATION_TESTS_DATA_DIR) if Dir.exists?(INTEGRATION_TESTS_DATA_DIR)
    Dir.mkdir(INTEGRATION_TESTS_DATA_DIR)

    sh('shakadb -d ' + INTEGRATION_TESTS_DATA_DIR + ' &> /dev/null &')
end

def stop_test_instance()
    puts "Stopping test instance ..."
    sh('killall shakadb -USR1')
end

def run_python_tests()
    sh('python -m pip install pytest')
    sh('python3 -m pip install pytest')

    sh('PYTHONPATH=' + PYTHON_WRAPPER_DIR + ' python -m pytest ' + PYTHON_WRAPPER_DIR + '/tests/*.py')
    sh('PYTHONPATH=' + PYTHON_WRAPPER_DIR + ' python3 -m pytest ' + PYTHON_WRAPPER_DIR + '/tests/*.py')
end

def run_dotnet_tests()
    sh("dotnet test #{DOTNET_WRAPPER_DIR}/ShakaDB.Client.Tests/ShakaDB.Client.Tests.csproj")
end

task :default => [:build_binaries, :run_tests]

task :build_common => [:default, :build_packages]
task :build_debug => [:build_common]
task :build_release => [:build_common, :run_integration_tests]

task :init do
    puts "Initializing build ..."

    FileUtils.rm_rf(BUILD_DIR) if Dir.exists?(BUILD_DIR)
    Dir.mkdir(BUILD_DIR)
    Dir.mkdir(BINARIES_DIR)
    Dir.mkdir(TESTS_DIR)
end

task :build_binaries => [:init] do
    sh('cmake -H. -B' + BINARIES_DIR)
    sh('cmake  --build ' + BINARIES_DIR + ' --target all -- -j 8')
end

task :run_tests => [:build_binaries] do
    puts "running tests ..."
    sh(BINARIES_DIR + '/shakadb.test ' + TESTS_DIR)
end

task :build_shakadb_package => [:build_binaries] do
    sh('cd ' + BINARIES_DIR + ' && cpack')
end

task :build_pyshaka_package => [:init] do
    sh('python3 -m pip install wheel')
    sh('cd ' + PYTHON_WRAPPER_DIR + ' && python3 setup.py bdist_wheel')
    sh('cp ' + PYTHON_WRAPPER_DIR + '/dist/pyshaka-*-py2.py3-none-any.whl ' + BINARIES_DIR)
end

task :build_dotnet_package => [:init] do
    puts "building dotnet package ..."

    version = "#{ENV['SDB_VERSION_MAJOR']}.#{ENV['SDB_VERSION_MINOR']}.#{ENV['SDB_VERSION_PATCH']}"
    sh("dotnet pack -o #{BINARIES_DIR} /p:PackageVersion=#{version} #{DOTNET_WRAPPER_DIR}/ShakaDB.Client")
end

task :build_packages => [:build_shakadb_package, :build_pyshaka_package, :build_dotnet_package]

task :start_test_instance do
    start_test_instance()
end

task :stop_test_instance do
    stop_test_instance()
end

task :run_python_tests do
    run_python_tests()
end

task :run_dotnet_tests do
    run_dotnet_tests()
end

task :run_integration_tests => [:build_binaries] do
    sh('sudo cmake  --build ' + BINARIES_DIR + ' --target install')

    start_test_instance()
    sleep 1

    run_python_tests()
    run_dotnet_tests()

    sleep 1
    stop_test_instance()
end

