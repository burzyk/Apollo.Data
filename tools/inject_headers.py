import glob
import sys
import itertools as it


def process_files(header, pattern):
    for file_name in glob.glob(pattern, recursive=True):
        print('processing file: ' + file_name)
        inject_header(header, file_name)


def inject_header(header, file_name):
    with open(file_name) as f:
        no_header_lines = list(it.dropwhile(lambda l: len(l) > 1 and l[1] == '*', f.readlines()))
        new_content = header + no_header_lines

    with open(file_name, mode='w') as f:
        f.writelines(new_content)


if __name__ == "__main__":
    root = sys.argv[1] if len(sys.argv) > 1 else '..'
    # root = '/Users/pburzynski/projects/ShakaDB.Performance'
    with open(root + '/LICENSE') as lic:
        header = ['/*\n'] + [' * ' + l for l in lic.readlines()] + [' */\n']
        extensions = ['h', 'cc', 'c']
        paths = ['src', 'test']

        for ext in extensions:
            for path in paths:
                process_files(header, root + '/{0}/**/*.{1}'.format(path, ext))
