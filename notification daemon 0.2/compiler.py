import os
from os import listdir
from os.path import isfile, join
import os.path
from os import path
import json
import subprocess
import sys
from threading import Thread
import hashlib

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

def compare_file_hashes(raw_file, hash_file):
    if path.exists(raw_file) and path.exists(hash_file):
        #get old hash from the hash file
        opened_hash_file = open(hash_file, "r")
        old_hash = opened_hash_file.read()

        #generate the new hash file from the raw file
        opened_raw_file = open(raw_file, "r")
        read_raw_file = opened_raw_file.read().encode()

        sha1_raw_file_hash = hashlib.sha1(read_raw_file)
        new_hash = sha1_raw_file_hash.hexdigest()

        return str(old_hash) == str(new_hash)

def update_hash_file(raw_file, hash_file):
    #generate the new hash file from the raw file
    opened_raw_file = open(raw_file, "r")
    read_raw_file = opened_raw_file.read().encode()

    sha1_raw_file_hash = hashlib.sha1(read_raw_file)
    new_hash = sha1_raw_file_hash.hexdigest()

    opened_hash_file = open(hash_file, "w")
    opened_hash_file.write(new_hash)
    opened_raw_file.close()
    opened_hash_file.close()

class CompilerThread (Thread):
    def __init__(self, command):
        Thread.__init__(self)
        self.command = command
    def run(self):
        exec_command(self.command)

#if the dirs are does not exist create them
if not path.exists("obj"):
    os.system("mkdir obj")

if not path.exists("obj/hashes"):
    os.system("mkdir obj/hashes")

actual_dir = os.getcwd()
print("finding files in this directory and subdirectories with .cpp extension\nfiles:")

#find all .cpp files in all the subdirectories
cpp_files = filter_string_if_contains(get_all_files_in_dir_root(actual_dir), [".cpp"])
if cpp_files != None:
    for file in cpp_files:
        print(file.split("/")[-1])
    print("total files: " + str(len(cpp_files)))
else:
    print("no .cpp file found, exiting")
    exit()

print("\n\nreading the compilation_options.json file")
(compiler_op, linker_op) = load_options_form_json_file(actual_dir + '/' + "compilation_options.json")

#creating the compiler command
compiler_command = "g++ -g -c "
#adding all the cpp files

end_compiler_command = ""

#adding the include paths
for include_path in compiler_op.include_paths:
    end_compiler_command += " -I" + include_path

#adding to the compilation command the compiler standard option
if compiler_op.standard != "":
    end_compiler_command += " -std=" + compiler_op.standard

#adding to the compilation command the compiler optimization option
if compiler_op.optimization != "":
    end_compiler_command += " -" + compiler_op.optimization
end_compiler_command += " -fpermissive"

print("\ngenerated compiler command:")
print(compiler_command + "xxx.cpp" + end_compiler_command + " -o obj/xxx.o")




#creating the linker command
linker_command = "g++"

#add to the linker command all the cpp files
for file in cpp_files:
    linker_command += " obj/" + file.split("/")[-1][:-3]+"o"

#add to the linker command all the library paths
for lib_path in linker_op.library_paths:
    linker_command += " -L" + lib_path

#add to the linker command all the library in the compilation options
for lib in linker_op.libs:
    linker_command += " -l" + lib

#add to the linker command the name of the executable
linker_command += " -o " + linker_op.executable_name

print("\ngenerated linker command:")
print(linker_command)

print("\nstart compiling...\n")

thread_pool = []

for file in cpp_files:
    hash_file_path_base = "obj/hashes/" + file.split("/")[-1][:-3]
    need_to_compile = False
    #check if the corresponding hash file for the .h file exist if not create the file and recompile
    if (path.exists(file[:-3]+"h")) and (not path.exists(hash_file_path_base+"h.hash")):
        need_to_compile = True
        f = open(hash_file_path_base+"h.hash", "x")
        f.close()

    #same thing as fot the previous if but with the .cpp file
    if (not path.exists(hash_file_path_base+"cp.hash")):
        need_to_compile = True
        f = open(hash_file_path_base+"cp.hash", "x")
        f.close()

    #check if the hash of .h and the hash of saved in the file are the same, if not update the hash in the file and recompile
    #if the hash isn't the same it's because the file has changed from the last compilation
    if (path.exists(file[:-3]+"h")) and (not compare_file_hashes(file[:-3]+"h", hash_file_path_base+"h.hash")):
        need_to_compile = True
        update_hash_file(file[:-3]+"h", hash_file_path_base+"h.hash")

    #same thing as the previous if but with the .cpp file
    if (not compare_file_hashes(file, hash_file_path_base+"cp.hash")):
        need_to_compile = True
        update_hash_file(file, hash_file_path_base+"cp.hash")
    
    #if compilation id needed recompile the file
    if need_to_compile:
        final_command = compiler_command + file.replace(" ", "\\ ") + end_compiler_command + " -o obj/" + file.split("/")[-1][:-3]+"o"
        thread_pool.append(CompilerThread(final_command))
        thread_pool[-1].start()
        print("compiling file: " + file.split("/")[-1])
    else:
        print("compilation isn't needed for the file: " + file.split("/")[-1])

#wait until all the threads have finished running the compilation command
for thread in thread_pool:
    thread.join()
print("\ncompilation finished\n")

print("\nstart linking..\n")
exec_command(linker_command)
print("\nlinker finished")

#if in the arguments list there is the -run option run the executable after the compilation has finished
if len(sys.argv) >1 and sys.argv[1] == "-run":
    print("\nrunning "+ linker_op.executable_name)
    exec_command("./" + linker_op.executable_name)