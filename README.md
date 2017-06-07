ShakaDB ![build-badge](https://travis-ci.org/burzyk/shakadb.svg?branch=master)
============================

ShakaDB is a simple, fast and lightweight time series database. It has been developed to hold financial data, but can be used to store any time series data.

ShakaDB data model
============================
In ShakaDB all data is organized into data series. Each data series has a numerical id (for example 1, 33, 9778). It is up to the user what data is stored in each data series. There is no upfront need of defining schema, just start writing to ShakaDB and the engine will create all time series for you.

Each data series is responsible for storing data points. They are defined as a (time: int64, value: float) pairs. ShakaDB makes no assumptions about the semantical meaning of time or value. Time can be days, microseconds from a certain point or just an ID of an event. The only assumption is that they are stored in ascending order and for best performance should arrive to the database in this order.

Dependencies
============================
What dependencies? It's lightweight, remember? (ok, ok - pthreads are required...)

Getting started
============================
The easiest way of starting with ShakaDB is to download a package from the [release](https://github.com/burzyk/ShakaDB/releases) page. Currently only Linux packages are available, but the application is also tested and compatible with OSX.

To start the database please execute the `shakadb` command. This starts the application with default settings. By default the application data directory is set to be `/usr/local/shakadb/data` but this can be changed by specifying the `--directory` parameter. Please make sure the user who is running the database has write permissions on the directory. For more details on configuration please see `shakadb --help`

When the database is started the easiest way of connecting is using `shakadb.client` application. Please see `shakadb.client --help` for more details.

To write some data to the database please execute
```
shakadb.client localhost 8487 write 1 10 100
```
This will write data point (10, 100) into the time series with id of 1

to read the data please execute
```
shakadb.client localhost 8487 read 1 0 100
```
This will retrieve all data points from the series `1` between timestamps of `0` and `100` (inclusive of `0`, exclusive of `100`).

Building from scratch
============================
To build the application from scratch the following tools are required
* cmake >= 3.2.2
* gcc or any other C compiler
* ruby
* rake
* bundler

The build process is managed by a `rake` file and there are following targets
* build_binaries - builds the source code
* run_tests - builds the source code and runs all unit tests
* build_packages - builds, tests and creates installation packages. To run this step cpack, rpm and deb tools are required.

The binaries can be found in `./build/bin` folder.
