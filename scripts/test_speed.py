#!/usr/bin/env python3

""" This module is designed to install and run benchmark tests and generate the reports.
    When the module is executed with the "comparewith" option, it will run 
    the benchmark tests for both the AMD optimized vs reference methods   
    where the reference is either vanilla version or IPP patched version
    and it store the performance results to CSV files.

    It will also display the summarized report and highlights of the performamce differences
    between the two versions on the console.

    This module works on both Linux and Windows.

    Dependencies:
    =============
    Python3 and python3-pip
    prettytable (https://pypi.org/project/prettytable/)
    pandas (https://pypi.org/project/pandas/)
    If any required modules are not already installed on the system, this python module will autmatically install them. 
    If the automatic installation fails, please install the required modules manually. 
"""

##  Usage example for AMD optimized vs Reference for methods lz4, snappy and zlib levels 1 and 2:
#
#   python3 test_speed.py --dataset $PATH_DATASETS_DIR -m lz4 snappy zlib:1 zlib:2 -cw vanilla

##  Usage example for AMD optimized vs IPP for lz4 Method:
#
#   python3 test_speed.py --dataset $PATH_DATASETS_DIR -m lz4 -cw ipp --ipp $IPP_PATCHED_LZ4_LIBS_PATH


__copyright__ = """
    Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
    * 
    * Redistribution and use in source and binary forms, with or without
    * modification, are permitted provided that the following conditions are met:
    *
    * 1. Redistributions of source code must retain the above copyright notice,
    * this list of conditions and the following disclaimer.
    * 2. Redistributions in binary form must reproduce the above copyright notice,
    * this list of conditions and the following disclaimer in the documentation
    * and/or other materials provided with the distribution.
    * 3. Neither the name of the copyright holder nor the names of its
    * contributors may be used to endorse or promote products derived from this
    * software without specific prior written permission.
    *
    * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    * POSSIBILITY OF SUCH DAMAGE.
 """

__author__ = 'Anand Kumar'

import argparse
import sys
import os
import time
import subprocess
from statistics import mean
import re
import platform
import shutil

## =========================
# Install the Python modules 
## =========================

def install_required_modules():
    """
        Install the Python modules in the modules list, if they are not already installed.
    """

    modules = ['prettytable', 'pandas']
    pip_lists = subprocess.run(
        ['python3', '-m', 'pip', 'list'], stdout=subprocess.PIPE, check=True)
    for module in modules:
        if pip_lists.stdout.decode("utf-8").find(module) <= 0:
            print("Module {} not found. Installing...".format(module))
            subprocess.run(["python3", "-m", "pip", "install",
                           "-U", module], check=True)
try:
    from prettytable import PrettyTable, from_csv
    import pandas as pd
except ImportError:
    install_required_modules()


FILENAME_TEST_CONFIG = "pretest_config.json"
DEFAULT_DATASETS = 'http://sun.aei.polsl.pl/~sdeor/corpus/silesia.zip'
ITERS = 50
pwd = os.getcwd()
FILEPATH_TEST_CONFIG = os.path.join(
    pwd, FILENAME_TEST_CONFIG)  # /path/to/pretestconfig file
DEFAULT_REPO = os.path.join(pwd, '..')
DEFAULT_BUILD = os.path.join(DEFAULT_REPO, 'build')
install = 'pip install'
ipp = 'off'
error = "ERROR: "
args = None
which_platform = platform.system()  # Check Platfrom before running

def remove_file_folder(path):
    """ Remove output file or dir if needed """
    
    if os.path.isfile(path) or os.path.islink(path):
        os.remove(path)  # remove the file
    elif os.path.isdir(path):
        shutil.rmtree(path)   # remove the dir
    else:
        pass

## =========================
# Calculate %Gain/Regression
## =========================

def get_percentage_inc_dec(num_amd_opt_yes, num_amd_opt_no):
    try:
        return ((num_amd_opt_yes / num_amd_opt_no) - 1) * 100
    except ZeroDivisionError:
        return "AMD_NO_OPT is {}".format(num_amd_opt_no)

# Prints log
def write(msg):
    print(time.strftime("%Y/%m/%d %H:%M:%S") + ' - ' + msg + '\n')

## ==================================
# Download and unzip silesia dataset 
## ==================================

def download_datasets(url, extract_dir):
    from urllib import request
    from zipfile import ZipFile
    zip_path, _ = request.urlretrieve(url)
    with ZipFile(zip_path, "r") as f:
        f.extractall(extract_dir)

## ================================================================
# Retrieve the relevant components and move to the processing steps
## ================================================================
        
rx_dict = {
    'libdata': re.compile(r'AOCL-COMPRESSION \[(.*)\] \[Filename:(.*)\] --'),
    'comp': re.compile(r'Compression: (.*) speed\(best\) (.*) MB/s,'),
    'decomp': re.compile(r'Decompression: (.*) speed\(best\) (.*) MB/s,'),
    'ratio': re.compile(r'Ratio:(.*)\n'),
}

def parse_line(line, amd_opt):
    """
    Do a regex search against all defined regexes and
    return the key and match result of the first matching regex

    """
    # If IPP option enabled
    if amd_opt == "IPP":
        rx_dict['libdata'] = re.compile(r'IPP \[(.*)\] \[Filename:(.*)\] --')
    else:
        pass

    for key, rx in rx_dict.items():
        match = rx.search(line)
        if match:
            return key, match
    # if there are no matches
    return None, None


def proc(cmd_args, pipe=True):
    """
    Args:
        cmd_args: The command argument is pass as the cmd_args parameter.
        pipe: stdout is redirected to a pipe. Defaults to True.
    """
    if pipe:
        subproc = subprocess.Popen(
            cmd_args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True)
    else:
        subproc = subprocess.Popen(cmd_args)

    outs, errs = subproc.communicate()
    orig_stdout = sys.stdout
    f = open('out.log', 'a')
    sys.stdout = f
    print(outs)
    sys.stdout = orig_stdout
    f.close()

## =================================================
# Extract the relevant data from the output log file
## =================================================
                
def parse_file(fileName, amd_opt):
    # create an empty list to collect the data
    data = [['method', 'dataset', 'comp', 'decomp', 'ratio'],]
    # open the file and read through it line by line
    remove_file_folder('parse_out.log')
    with open(fileName, 'r') as file_object:
        line = file_object.readline()
        while line:
            # at each line check for a match with a regex
            key, match = parse_line(line, amd_opt)

            if key == 'libdata':
                lib = match.group(1)
                dataset = match.group(2)
            if key == 'comp':
                comp = match.group(2)
            if key == 'decomp':
                decomp = match.group(2)
            if key == 'ratio':
                ratio = match.group(1).strip()
                dd = " ".join([lib, dataset, comp, decomp, ratio])
                with open("parse_out.log", "a", encoding="utf-8") as f:
                    f.write(dd + '\n')
            line = file_object.readline()
    return data

## ========================
# To run test bench command 
## ========================

def testbench(args, pipe=True):
    return proc([bench_cmd] + args, pipe)


## ================
# Execute commands 
## ================
                
def run_command(cmd, filename='build_logs.log', print_command=True, print_output=False, print_error=True, param_shell=True):
    """
    Parameters:
        cmd: Pass the command as the cmd parameter.
        filename: Save Console's output to file. Defaults is in 'build_logs.log'.
        print_command: Print command to the console before being executed.
        print_output: Command's output print to the console.
        print_error: If error messages, print to the console.
        param_shell: cmd string is passed to the system's shell for execution if it is true.
    Raises:
        RuntimeError: Function raises a RuntimeError if the command's return code is non-zero and not None
    Returns: Returns a list of the lines of output generated with the command.
    """
    if print_command:
        write(cmd)
    popen = subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=param_shell,
        cwd=run_command.cwd)
    stdout_lines, stderr_lines = popen.communicate()
    stderr_lines = stderr_lines.decode("utf-8")
    stdout_lines = stdout_lines.decode("utf-8")
    if print_output:
        if stdout_lines:
            with open(filename, "a") as f:
                f.write(stdout_lines + '\n')
        if stderr_lines:
            with open(filename, "a") as f:
                f.write(stderr_lines + '\n')
    if popen.returncode is not None and popen.returncode != 0:
        if stderr_lines and not print_output and print_error:
            print(stderr_lines)
        raise RuntimeError(stdout_lines + stderr_lines)
    return (stdout_lines + stderr_lines).splitlines()

run_command.cwd = None

def get_results(fileName):
    """
    To get relevant information from the processed output file
    """
    if not os.path.isfile(fileName):
        return None, None, None, None, None

    with open(fileName, 'r') as f:
        (lib, dtype, dsize, dspeed, dratio) = ([], [], [], [], [])
        words = []
        for line in f:
            word = list(line.split(" "))
            lib.append(word[0])
            dtype.append(word[1])
            dsize.append(word[2])
            dspeed.append(word[3])
            dratio.append(word[4])
        words.append(lib)
        words.append(dtype)
        words.append(dsize)
        words.append(dspeed)
        words.append(dratio)
        return words

## ===============================
# Calculate and prints highlights 
## ===============================

def compare_benchmarks_numbers(amd_opt_yes, compare, method_name, amd_opt):
    """
    parameters:
        amd_opt_yes (string): Enable AOCL optimization 
        compare (string):  Comparison lib optimization
        method_name (string): Compression method 
    """
    fnamereport = 'final_comparision_report_{}.csv'.format(method_name.replace(':', '_Level_'))
    csv1 = pd.read_csv(amd_opt_yes)
    csv2 = pd.read_csv(compare)
    merged_data = csv1.merge(csv2, on=["Method", "DataSet"], how="left")
    merged_data.to_csv(fnamereport, index=False)
    csv3 = pd.read_csv(fnamereport)
    sumrTable = PrettyTable()
    DsumrTable = PrettyTable()
    RsumrTable = PrettyTable()
    print("------------AMD optimization ON vs {} tests summary for the Method {} -----------\n".format(amd_opt.replace("AMD_OPT_", ""), method_name.replace(":", " Level ")))
    sumrTable.field_names = [
        "Method",
        "ComprSpeed_AMD_OPT_ON",
        "ComprSpeed_{}".format(amd_opt),
        "ComprSpeed Diff(%)"]
    comprspeed_y = round(csv3["ComprSpeed_AMD_OPT_ON"].mean(), 2)
    comprspeed_n = round(csv3["ComprSpeed_{}".format(amd_opt)].mean(), 2)
    dcomprspeed_y = round(csv3["DecomprSpeed_AMD_OPT_ON"].mean(), 2)
    dcomprspeed_n = round(csv3["DecomprSpeed_{}".format(amd_opt)].mean(), 2)
    comprRatio_y = round(csv3["ComprRatio_AMD_OPT_ON"].mean(), 2)
    comprRatio_n = round(csv3["ComprRatio_{}".format(amd_opt)].mean(), 2)

    diffS = round(get_percentage_inc_dec(comprspeed_y, comprspeed_n), 2)
    if diffS < 0:
        print("* Regression in {} compression speed: {}%".format(method_name, abs(diffS)))
    else:
        print("* Gain in {} compression speed: {}%".format(method_name, abs(diffS)))
    sumrTable.add_row([method_name, comprspeed_y, comprspeed_n, diffS])
    print(sumrTable, "\n")
    DsumrTable.field_names = [
        "Method",
        "DecomprSpeed_AMD_OPT_ON",
        "DecomprSpeed_{}".format(amd_opt),
        "DecomprSpeed Diff(%)"]

    diffS = round(get_percentage_inc_dec(dcomprspeed_y, dcomprspeed_n), 2)
    if diffS < 0:
        print("* Regression in {} Decompression speed:{}%".format(method_name, abs(diffS)))
    else:
        print("* Gain in {} Decompression speed: {}%".format(method_name, abs(diffS)))
    DsumrTable.add_row([method_name, dcomprspeed_y, dcomprspeed_n, diffS])
    print(DsumrTable, "\n")
    RsumrTable.field_names = [
        "Method",
        "ComprRatio_AMD_OPT_ON",
        "ComprRatio_{}".format(amd_opt),
        "ComprRatio Diff(%)"]

    diffS = round(get_percentage_inc_dec(comprRatio_y, comprRatio_n), 2)

    if diffS <= 0:
        print("* Gain in {} compression ratio: {}%".format(method_name, abs(diffS)))
    else:
        print("* Regression in {} compression ratio: {}%".format(method_name, abs(diffS)))

    RsumrTable.add_row([method_name, comprRatio_y, comprRatio_n, diffS])
    print(RsumrTable, "\n")

    with open(fnamereport) as fp:
        ptable = from_csv(fp)
    print(ptable)

## ===================================
#  Create a performance summary tables
##====================================
        
def create_tables(fileName, amd_opt, method_name):
    """ This is a function that take detailed input file and create beautiful tables.
    """

    tableName = amd_opt
    rescsvFile = 'final_{}_{}.csv'.format(method_name.replace(':', '_Level_'), tableName)
    CspeedOPT = "ComprSpeed_{}".format(tableName)
    DspeedOPT = "DecomprSpeed_{}".format(tableName)
    ComprRatio = "ComprRatio_{}".format(tableName)
    print("Compression benchmarking completed for the method {} with {}".format(
        method_name, tableName.replace("IPP", "IPP_OPT_ON")))
    tableName = PrettyTable()
    tableName.field_names = [
        "Method",
        "DataSet",
        CspeedOPT,
        DspeedOPT,
        ComprRatio]

    # Parse the output file
    parse_file('out.log', amd_opt)
    mName = get_results(fileName)[0]
    csize = get_results(fileName)[1]
    cspeed = get_results(fileName)[2]
    dspeed = get_results(fileName)[3]
    cratio = get_results(fileName)[4]  

    for m, cs, csp, dsp, cr in zip(mName, csize, cspeed, dspeed, cratio):
        tableName.add_row([m, cs, csp, dsp, cr.strip()])
    cspeed = [eval(csp) for csp in cspeed]
    dspeed = [eval(dsp) for dsp in dspeed]
    cratio = [eval(cr) for cr in cratio]

    tableName.add_row(["Method:", "Average", round(
        mean(cspeed), 2), round(mean(dspeed), 2), round(mean(cratio), 2)])
    print("\n")

    with open(rescsvFile, 'w', newline='') as f:
        f.write(tableName.get_csv_string())

## =============================================
#   Sets the user options and trigger benchmarks
## =============================================

def run_benchmark_test(datasetFile_lst, method_name, iters, amd_opt="on", comparewith="off", ipp='off', run=True):
    """Sets the user options and trigger benchmark routine """

    if amd_opt == "on" or amd_opt == "ON":
        amd_opt = 'AMD_OPT_ON'
    if amd_opt == "off" or amd_opt == "OFF":
        amd_opt = 'AMD_OPT_OFF'
    method = "-e{}".format(method_name)
    testOptions = []
    compare = comparewith
    iters = "-i{}".format(iters)

    # Check and perform comparison tests if the user selects that option 
    while run:
        print("\nCompression benchmarking started for the method {} with {} .....".format(
            method_name, amd_opt.replace("IPP", "IPP_OPT_ON")))
        remove_file_folder("out.log")
        for dataset in datasetFile_lst:
            testOptions = ["-a", "-t", "-p"]
            if amd_opt == "IPP" or amd_opt == "ipp":
                testOptions.append("-c")
                testOptions.append("-v3")
            if amd_opt == "AMD_OPT_OFF" or amd_opt == "off":
                testOptions.append("-o")
            cmd = "{}/{}".format(args.dataset, dataset)
            testOptions.append(cmd)
            testOptions.append(method)
            testOptions.append(iters)
            testbench(testOptions, True)
        create_tables('parse_out.log', amd_opt, method_name)

        if comparewith == "ipp":
            if ipp != 'off':
                amd_opt = "IPP"
            else:
                print(error + "Provide IPP library path with --ipp flag to compare")
                exit()
        elif comparewith == "vanilla":
            amd_opt = 'AMD_OPT_OFF'
        else:
            run = False
        comparewith = "off"

    if compare == 'ipp' or compare == 'vanilla':
        compare_benchmarks_numbers('final_{}_AMD_OPT_ON.csv'.format(
            method_name.replace(':', '_Level_')), 'final_{}_{}.csv'.format(method_name.replace(':', '_Level_'), amd_opt), 
            method_name, amd_opt)
    else:
        with open('final_{}_{}.csv'.format(method_name.replace(':', '_Level_'), amd_opt)) as fp:
            detailsTable = from_csv(fp)

        print(detailsTable)

            
## ===============
# Help formatting
## ===============
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--dataset',
        required=True, help='Input dataset files path for speed benchmarks. If no dataset available in the path, the standard Silesia dataset will be downloaded automatically.')
    parser.add_argument(
        '--build', '-b', choices=['yes', 'no'],
        help='Use this option to control build and install AOCL-Compression before benchmark tests. yes: Build and install from the source package, no: Do not build and install, but reuse the last installed binaries. Default behavior: Yes(always build and install)', default="yes")
    parser.add_argument(
        '--collectBuildlogs', '-printlogs', choices=['yes', 'no'],
        help='Pass -printlogs yes to collect build logs to the file build_logs.log. By default, build logs are not collected.', default="no")
    parser.add_argument(
        '--compiler', '-c',
        help='Specify compiler for building AOCL-Compression like "compilerName CompilerPath". Default is GCC',
        default="gcc")
    parser.add_argument(
        '--method', '-m',
        required=True, help='Pass a list of compression methods to run speed benchmark. Pass it like <method name>:<level>. For example: For zlib level 1, pass zlib:1 '
        'Supported methods and levels: lz4 - No levels, lz4hc - levels[1-12], snappy - No levels, lzma - levels[0-9], zlib - levels[1-9], zstd - levels[1-22], bzip2 - levels[1-9]', nargs="*")
    parser.add_argument(
        '--ipp', '-ipp',
        help='Provide IPP installed library path like for lz4: --ipp $PATH/lz4-1.9.3/lib) and for zlib: --ipp $PATH/zlib-1.2.11)', default=ipp)
    parser.add_argument(
        '--optimization', '-o', choices=['on', 'off'],
        help='Specify one of the options: on(default - with AOCL optimization), off(without AOCL optimization)', type=str,
        default="AMD_OPT_ON")
    parser.add_argument(
        '--comparewith', '-cw', choices=['ipp', 'vanilla'],
        help='Please specify comparision library, options are vanilla(Vanilla VS AOCL optimization), ipp(IPP VS AOCL optimization)', type=str
    )
    parser.add_argument(
        '--iterations', '-itr',
        help='Specify number of iterations for compression/decompression.', default=ITERS)
    parser.add_argument('--optionalFlags', '-flags',
                        help='Pass a list of optional supportive flags (SNAPPY_MATCH_SKIP_OPT, AOCL_LZ4_OPT_PREFETCH_BACKWARDS, and LZ4_FRAME_FORMAT_SUPPORT) as required. For example: -flags SNAPPY_MATCH_SKIP_OPT=ON AOCL_LZ4_OPT_PREFETCH_BACKWARDS=ON LZ4_FRAME_FORMAT_SUPPORT=ON',
                        type=str, nargs="*", default=[])
    parser.add_argument('--VSVersion', '-vs',
                        help='This is a Windows platfrom specific option. Use this option to provide installed Visual Studio version on system. Default is Visual Studio 17 2022', default="Visual Studio 17 2022")
    args = parser.parse_args()

    run_command.cwd = DEFAULT_REPO
    ## ==============================
    # Installing Compression library
    ## ==============================
    installation_path = os.path.join(DEFAULT_REPO, 'amd-compression')
    build_path = os.path.join(DEFAULT_REPO, 'build')
    if which_platform == 'Windows':
        bench_cmd = os.path.join(
            DEFAULT_BUILD, 'Release/aocl_compression_bench')
    else:
        bench_cmd = os.path.join(
            installation_path, 'aocl_compression/bin/aocl_compression_bench')

    compression_cmds = {
        'firstCmd': 'cmake -B build . -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC_LIBS=OFF -DCMAKE_INSTALL_PREFIX={}'.format(installation_path),
        'secondCmd': 'cmake --build build -v -j --target uninstall --target install'}
    compression_cmds_windows = {
        'firstCmd': 'cmake -B build . -T ClangCl -G "{}" -DCMAKE_INSTALL_PREFIX={}'.format(args.VSVersion, installation_path),
        'secondCmd': 'cmake --build ./build --config Release --target INSTALL'
    }
                
    def install_compression(cmds, optionalFlags):
        """
        Install aocl-compression library. This will generate libs/bin to the specified prefix path.

        """

        for flag in args.optionalFlags:
            compression_cmds["firstCmd"] = compression_cmds["firstCmd"] + \
                " " + "-D{}".format(flag)

        for k, val in cmds.items():
            if args.compiler.split()[0] == "aocc":
                if args.ipp != "off":
                    print(error + "IPP comparision does not supports for AOCC compiler")
                    exit()
                clang = args.compiler.split()[1]
                if k == 'firstCmd':
                    print("\nConfiguring AOCL-Compression for the installation .....\n")
                    val = val + " " + "-DCMAKE_C_COMPILER={}".format(clang)
                else:
                    print("Installing AOCL-Compression using AOCC Compiler ..... \n")
            else:
                if k != 'firstCmd':
                    if which_platform == 'Windows':
                        print(
                            "Installing AOCL-Compression using Clang Compiler ..... \n")
                    else:
                        print(
                            "Installing AOCL-Compression using GCC Compiler ..... \n")
                else:
                    print("\nConfiguring AOCL-Compression for the installation .....\n")

            if args.collectBuildlogs == "yes":
                run_command(val, 'build_logs.log', True, True)
            else:
                run_command(val, 'build_logs.log', True, False)

    # Check if test files are accessible
    if not os.path.exists(args.dataset) and not os.path.isfile(args.dataset):
        os.mkdir(args.dataset)
    if not os.listdir(args.dataset):
        print("Input dataset is not available at specified path: {}. So, Silesia datasets are downloading to the path \n".format(args.dataset))
        download_datasets(DEFAULT_DATASETS, args.dataset)

    # Make list of the datasets that are available in the directory 
    datasetFile_lst = sorted(os.listdir("{}".format(args.dataset)))
    ipp_supports_methods = ['lz4', 'lz4hc', 'zlib']

    # Sets the IPP enviroment variable before running IPP patched method
    if args.ipp != "off" or args.comparewith == "ipp":
        if args.method[0].split(":")[0] in ipp_supports_methods:
            os.environ['LD_LIBRARY_PATH'] = args.ipp
        else:
            print(
                error + "Compression test framework does not support IPP method: {}".format(args.method))
            exit()
    
    ## =====================================
    # Check pre test settings and conditions  
    ## ======================================

    if args.comparewith and args.optimization == "off":
        raise ValueError("comparewith doesn't support with optimization off")

    if args.comparewith == "ipp" and args.ipp == "off":
        print(error + "Comparewith(IPP) and IPP path flags should be enabled together \n")
        exit()
    elif args.comparewith == "vanilla" and args.ipp != "off":
        print(error + "Comparewith(vanilla) and IPP path flags should not be enabled together \n")
        exit()
    elif args.comparewith == "ipp" and len(args.method) >1:
        print(error + "--method/-m argument does not accept list of methods when comparing with IPP. Instead, only pass the specific method that you want to compare with IPP.")
        exit()
    else:
        pass
    
    ## ===========================
    # Compression building control  
    ## ===========================

    if args.build.lower() == "yes":
        remove_file_folder("build_logs.log")
        if os.path.isdir(build_path):
            remove_file_folder(build_path)
        if which_platform == 'Windows':
            print("Building for Windows")
            install_compression(compression_cmds_windows, args.optionalFlags)
        else:
            install_compression(compression_cmds, args.optionalFlags)

    print("\n")

    # Delete previous reports if any
    [remove_file_folder(f) for f in os.listdir(
        pwd) if os.path.splitext(f)[1] == '.csv']
    
    # Check list of methods passed in the arguments and run benchmarks for each of them
    for m in args.method:
        run_benchmark_test(datasetFile_lst, m, args.iterations,
                       args.optimization, args.comparewith, args.ipp)

    print("Compression benchmarking tests are completed. Please find the generated reports.")
