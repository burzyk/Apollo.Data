require 'rake'
require 'fileutils'

BUILD_DIR='./build'
BINARIES_DIR=BUILD_DIR + '/bin'
TESTS_DIR=BUILD_DIR + '/tests'

task :default => [:build_binaries]

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

task :build_packages => [:run_tests] do
    sh('cd ' + BINARIES_DIR + ' && cpack && cd ../..')
end
