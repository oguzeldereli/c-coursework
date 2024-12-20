import platform
import sys
import subprocess
import os
import shutil
from datetime import datetime

working_directory = os.getcwd()

linker = "ld"
java = "java"
cc = "gcc"
cc_supports_linking = True

drawapp = os.path.join(working_directory, "drawapp-4.0.jar")

src_dir = os.path.join(working_directory, "src/")
object_output = os.path.join(working_directory, "obj/")
binary_output = os.path.join(working_directory, "bin/")
logs_output = os.path.join(working_directory, "logs/")

run_file = "c-coursework"
win_suffix = ".exe"
run_arguments = []

def clear_directories():
    if os.path.exists(object_output):
        shutil.rmtree(object_output)
    os.makedirs(object_output, exist_ok=True)
    if os.path.exists(binary_output):
        shutil.rmtree(binary_output)
    os.makedirs(binary_output, exist_ok=True)

# generated by ChatGPT-o1-preview
def check_compiler():
    try:
        result = subprocess.run([cc, '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if result.returncode == 0:
            return True
        else:
            return False
    except FileNotFoundError:
            return False
    except Exception as e:
        print(f"An error occurred: {e}")
        return False

# generated by ChatGPT-o1-preview
def check_java():
    try:
        result = subprocess.run([java, '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if result.returncode == 0:
            return True
        else:
            return False
    except FileNotFoundError:
            return False
    except Exception as e:
        print(f"An error occurred: {e}")
        return False

# generated by ChatGPT-o1-preview
def check_linker():
    try:
        result = subprocess.run([linker, '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if result.returncode == 0:
            return True
        else:
            return False
    except FileNotFoundError:
            return False
    except Exception as e:
        print(f"An error occurred: {e}")
        return False

# generated by ChatGPT-o1-preview
def find_src_files(source_directory):
    src_files = []
    for root, dirs, files in os.walk(source_directory):
        for file in files:
            if file.endswith(".c"):
                src_files.append(os.path.join(root, file))
    return src_files

def run_shell(command, args):
    result = subprocess.run([command] + args, capture_output=True, text=True, shell=False)
    if result.stderr:
        return result.stderr
    return result.stdout

def get_filename(path_to_c_file):
    return os.path.basename(path_to_c_file)

def get_object_filename(path_to_c_file):
    return get_filename(path_to_c_file).replace(".c", ".o")

def make_object(path_to_c_file):
    return run_shell(cc, ["-o", f"{object_output}/{get_object_filename(path_to_c_file)}", "-c", path_to_c_file])

def link_files(object_dir, output_filename):
    object_files = [os.path.join(object_dir, f) for f in os.listdir(object_dir) if f.endswith('.o')]

    if not object_files:
        print("No object files found in the directory.")
        return ""

    command = ""
    if cc_supports_linking:
        command = [cc, "-o",  f"{binary_output}/{output_filename}"] + object_files
    else:
        command = [linker, "-o",  f"{binary_output}/{output_filename}"] + object_files

    try:
        result = subprocess.run(command, check=True, capture_output=True, text=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        return e.output

def check_file(path):
    return os.path.exists(path)

def run(draw):
    global run_arguments
    global win_suffix
    
    executable = f"{binary_output}/{run_file + win_suffix}"
    if not check_file(executable):
        print("Executable not found")
        exit(-1)

    if draw and check_file(drawapp):
        with subprocess.Popen([executable] + run_arguments, stdout=subprocess.PIPE) as proc1:
            with subprocess.Popen([java, "-jar", str(drawapp)], stdin=proc1.stdout, stdout=subprocess.PIPE, text=True) as proc2:
                proc1.stdout.close() 
                output, error = proc2.communicate()
                if error:
                    print(f"Error: {error}")
                return output
    else:
        if not check_file(drawapp):
            print("Drawapp not found, make sure it is in the same directory as this build script. Running without drawing:")
        return run_shell(executable, run_arguments)

def log_output(output):
    os.makedirs(logs_output, exist_ok=True)
    with open(f"{logs_output}/log-{datetime.today().strftime('%Y-%m-%d')}.txt", "a") as file:
        file.write(f"{'-'*12} Results From {datetime.now()} {'-'*12}\n")
        file.write(output)

def build(arguments):
    global win_suffix
    
    os_type = platform.system()
    win_suffix = ".exe" if os_type == "Windows" else ""

    if not check_compiler():
        print(f"Compiler not found, change compiler in build.py or make sure the default compiler '{cc}' is installed.")
        exit(-1)

    if not cc_supports_linking and not check_linker():
        print(f"Linker not found and compiler doesn't support linking, change linker in build.py or make sure the default linker '{linker}' is installed.")
        exit(-1)

    clear_directories()

    if not os.path.exists(src_dir):
        print(f"Source directory {src_dir} not found.")
        exit(-1)

    src_files = find_src_files(src_dir)
    output = ""

    for file in src_files:
        result = make_object(file)
        if result:
            print(f"Error compiling {file}: {result}")
            log_output(result)
            exit(-1)

    if output:
        print(output)
        log_output(output)
        exit(-1)

    print("Compiled all source files into object files")

    output = link_files(object_output, run_file + win_suffix)
    
    if output:
        print(output)
        log_output(output)
        exit(-1)

    global run_arguments
    print("Linked all objects files into an executable")
    if any(arg == "-run" for arg in arguments):
        draw = False
        run_arguments = arguments[2:]
        if any(arg == "-draw" for arg in arguments):
            draw = True
            run_arguments = arguments[3:]

        output = run(draw)

    print(output)
    log_output(output)

# 1 possible arg -run
if __name__ == "__main__":
    build(sys.argv)



