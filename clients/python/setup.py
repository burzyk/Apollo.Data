from setuptools import setup, find_packages
from codecs import open
from os import path, environ

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

version = environ['SDB_VERSION']

setup(
    name='pyshaka',

    version=version,

    description='ShakaDB python client library',
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

    keywords='shakadb client timeseries',

    packages=find_packages(exclude=['tests'])
)
