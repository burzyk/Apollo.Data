from setuptools import setup, find_packages
from codecs import open
from os import path, environ

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

version = '{0}.{1}.{2}'.format(
    environ['SDB_VERSION_MAJOR'],
    environ['SDB_VERSION_MINOR'],
    environ['SDB_VERSION_PATCH'])

setup(
    name='pyshaka',

    version=version,

    description='ShakaDB python ctl library',
    long_description=long_description,

    url='http://shakadb.com',

    author='burzyk',
    author_email='p.k.burzynski@gmail.com',

    license='MIT',

    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
    ],

    keywords='shakadb ctl timeseries',

    packages=find_packages(exclude=['tests'])
)
