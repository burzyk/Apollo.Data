require 'rake'
require 'fileutils'
require 'open3'

THIS_DIR=File.dirname(__FILE__)
BUILD_DIR=THIS_DIR + '/build'
BINARIES_DIR=BUILD_DIR + '/bin'
TESTS_DIR=BUILD_DIR + '/tests'
INTEGRATION_TESTS_DATA_DIR=BUILD_DIR + '/integration-tests'
PYTHON_CLIENT_DIR=THIS_DIR + '/clients/python'
PYTHON_CLIENT_DIST=PYTHON_CLIENT_DIR + '/dist'
DOTNET_CLIENT_DIR=THIS_DIR + '/clients/dotnet'

def start_test_instance()
    puts "Starting test instance ..."

    FileUtils.rm_rf(INTEGRATION_TESTS_DATA_DIR) if Dir.exists?(INTEGRATION_TESTS_DATA_DIR)
    Dir.mkdir(INTEGRATION_TESTS_DATA_DIR)

    sh("#{BINARIES_DIR}/shakadb -d #{INTEGRATION_TESTS_DATA_DIR} &> /dev/null &")
end

def stop_test_instance()
    puts "Stopping test instance ..."
    sh("killall shakadb -USR1")
end

def run_python_tests()
    sh("python -m pip install pytest")
    sh("python3 -m pip install pytest")

    sh("PYTHONPATH=#{PYTHON_CLIENT_DIR} python -m pytest #{PYTHON_CLIENT_DIR}/tests/*.py")
    sh("PYTHONPATH=#{PYTHON_CLIENT_DIR} python3 -m pytest #{PYTHON_CLIENT_DIR}/tests/*.py")
end

def run_dotnet_tests()
    sh("dotnet test #{DOTNET_CLIENT_DIR}/ShakaDB.Client.Tests/ShakaDB.Client.Tests.csproj")
end

task :set_debug do
    ENV['SDB_CONFIGURATION'] = "debug"
end

task :set_release do
    ENV['SDB_CONFIGURATION'] = "release"
end


task :default => [:build_binaries, :run_tests]

task :build_common => [:default, :build_clients]
task :build_debug => [:set_debug, :build_common]
task :build_release => [:set_release, :build_common, :run_integration_tests]

task :init do
    puts "Initializing build ..."

    FileUtils.rm_rf(BUILD_DIR) if Dir.exists?(BUILD_DIR)
    FileUtils.rm_rf(PYTHON_CLIENT_DIST) if Dir.exists?(PYTHON_CLIENT_DIST)
    Dir.mkdir(BUILD_DIR)
    Dir.mkdir(BINARIES_DIR)
    Dir.mkdir(TESTS_DIR)

    git_descr, _, _ = Open3.capture3("git", "describe")
    git_commit, _, _ = Open3.capture3("git", "rev-parse", "HEAD")

    ENV['SDB_VERSION'] = git_descr.strip
    ENV['SDB_BUILD'] = git_commit.strip
end

task :build_binaries => [:init] do
    sh("cmake -H. -B#{BINARIES_DIR}")
    sh("cmake  --build #{BINARIES_DIR} --target all -- -j 8")
end

task :run_tests => [:build_binaries] do
    puts "running tests ..."
    sh("#{BINARIES_DIR}/shakadb.test --directory #{TESTS_DIR}")
end

task :build_pyshaka_package => [:init] do
    sh("python3 -m pip install wheel")
    sh("cd #{PYTHON_CLIENT_DIR} && python3 setup.py bdist_wheel")
    sh("cp #{PYTHON_CLIENT_DIST}/pyshaka-*-py2.py3-none-any.whl #{BINARIES_DIR}")
end

task :build_dotnet_package => [:init] do
    puts "building dotnet package ..."

    version = "#{ENV['SDB_VERSION']}"
    sh("dotnet restore #{DOTNET_CLIENT_DIR}/ShakaDB.Client")
    sh("dotnet restore #{DOTNET_CLIENT_DIR}/ShakaDB.Client.Tests")
    sh("dotnet pack -o #{BINARIES_DIR} /p:PackageVersion=#{version} #{DOTNET_CLIENT_DIR}/ShakaDB.Client")
end

task :build_docker_image => [:init] do
    puts "building docker image ..."

    sh("git archive HEAD | bzip2 > #{BUILD_DIR}/source-latest.bz2")
    sh("docker build . --squash --tag persephonecloud.com/shakadb:$SDB_VERSION --build-arg version=${SDB_VERSION} --build-arg build=${SDB_BUILD}")
end

task :build_clients => [
    :build_pyshaka_package,
    :build_dotnet_package]

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
    start_test_instance()
    sleep 1

    run_python_tests()
    run_dotnet_tests()

    sleep 1
    stop_test_instance()
end

