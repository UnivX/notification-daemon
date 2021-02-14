import os
from os import listdir
from os.path import isfile, join
import json
import subprocess
import sys

def exec_command(cmd):
    cut = subprocess.Popen(cmd,
                            shell=True,
                            stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE,
                            )

    end_of_pipe = cut.stdout
    for line in end_of_pipe:
        print('\t', line.strip())

def get_all_file_in_dir(directory):
    return [(directory + '/'+ f) for f in listdir(directory) if isfile(join(directory, f))]

def get_all_dirs_in_dirs(directory):
    return [f for f in listdir(directory) if not(isfile(join(directory, f)))]

def get_all_dirs_in_dir_root(directory):
    all_dirs = list()
    local_dirs = get_all_dirs_in_dirs(directory)
    for dir in local_dirs:
        if(dir != None and dir != ""):
            founded_dirs = get_all_dirs_in_dir_root(directory + '/' + dir)
            for founded_dir in founded_dirs:
                all_dirs.append(founded_dir)
            all_dirs.append(directory + '/' + dir)
    return all_dirs

def get_all_files_in_dir_root(directory):
    all_dirs = get_all_dirs_in_dir_root(directory)
    all_files = get_all_file_in_dir(directory)
    for dir in all_dirs:
        files = get_all_file_in_dir(dir)
        for file in files:
            all_files.append(file)
    return all_files

def filter_string_if_contains(string_array, filters_array : list):
    array = string_array
    for filt in filters_array:
        array = [f for f in array if filt in f]
    return array

class compiler_options:
    include_paths : list
    optimization : str
    standard : str

class liker_options:
    library_paths : list
    libs : list
    executable_name : str

def load_options_form_json_file(file):
    '''return (compiler_op, linker_op) '''

    with open(file) as f:
        data = json.load(f)
    linker_op = liker_options()
    compiler_op = compiler_options()

    compiler_op.include_paths = data["compiler_options"]["include_paths"]
    compiler_op.optimization = data["compiler_options"]["optimization"]
    compiler_op.standard = data["compiler_options"]["standard"]

    linker_op.library_paths = data["linker_options"]["library_paths"]
    linker_op.libs = data["linker_options"]["libs"]
    linker_op.executable_name = data["linker_options"]["executable_name"]
    return (compiler_op, linker_op)

actual_dir = os.getcwd()
print("finding files in " + actual_dir +" with .cpp extension\nfiles:")
cpp_files = filter_string_if_contains(get_all_files_in_dir_root(actual_dir), [".cpp"])
if cpp_files != None:
    for file in cpp_files:
        print(file)
    print("total files: " + str(len(cpp_files)))
else:
    print("no .cpp file found, exiting")
    exit()

print("reading the compilation_options.json file")
(compiler_op, linker_op) = load_options_form_json_file(actual_dir + '/' + "compilation_options.json")

#creating the compiler command
compiler_command = "g++ -c "
#adding all the cpp files

end_compiler_command = ""

#adding the include paths
for include_path in compiler_op.include_paths:
    end_compiler_command += " -I" + include_path

if compiler_op.standard != "":
    end_compiler_command += " -std=" + compiler_op.standard

if compiler_op.optimization != "":
    end_compiler_command += " -" + compiler_op.optimization
end_compiler_command += " -fpermissive"
print("\nusing the compiler command:")
print(compiler_command + "xxx.cpp" + end_compiler_command + " -o obj/xxx.o")




#creating the linker command
linker_command = "g++"

for file in cpp_files:
    linker_command += " obj/" + file.split("/")[-1][:-3]+"o"

for lib_path in linker_op.library_paths:
    linker_command += " -L" + lib_path

for lib in linker_op.libs:
    linker_command += " -l" + lib

linker_command += " -o " + linker_op.executable_name

print("\nusing the linker command:")
print(linker_command)

print("\nexecuting the compilation and linker command\n")

for file in cpp_files:
    final_command = compiler_command + file.replace(" ", "\\ ") + end_compiler_command + " -o obj/" + file.split("/")[-1][:-3]+"o"
    exec_command(final_command)
    print("command: " + final_command)

exec_command(linker_command)

if len(sys.argv) >1 and sys.argv[1] == "-run":
    print("\nrunning "+ linker_op.executable_name)
    exec_command("./" + linker_op.executable_name)