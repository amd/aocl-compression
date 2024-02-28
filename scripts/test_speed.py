#!/usr/bin/env python3

""" This module is designed to install and run benchmark tests and generate the reports.
    When the module is executed with the "comparewith" option, it runs 
    the benchmark tests for both the AMD optimized vs reference methods   
    where the reference is either vanilla version or IPP patched version
    and it stores the performance results to CSV files.

    It will also display the summarized report and highlights of the performance differences
    between the two versions on the console.

    This module works on both Linux and Windows.

    Dependencies:
    =============
    Python3 and python3-pip
    prettytable (https://pypi.org/project/prettytable/)
    pandas (https://pypi.org/project/pandas/)
    If any required modules are not already installed on the system, this python module will automatically install them. 
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
import math
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


DEFAULT_DATASETS = 'http://sun.aei.polsl.pl/~sdeor/corpus/silesia.zip'
ITERS = 50
pwd = os.path.dirname(os.path.abspath(__file__)) # Get the absolute path of the current python script
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

def get_percentage_inc_dec(first_num, second_num):
    try:
        return ((first_num / second_num) - 1) * 100
    except ZeroDivisionError:
        return 0

# Get Geometric mean
def geo_mean(itr):
    ls = list(itr)
    # Contain only positive numbers
    pos_ls = [val for val in ls if val > 0]
    if len(pos_ls) == 0:
        return 0
    else:
        return math.exp(sum(math.log(n) for n in pos_ls) / len(pos_ls))


# Print log
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
        

def parse_line(line, amd_opt):
    """
    Do a regex search against all defined regexes and
    return the key and match result of the first matching regex

    """
    rx_dict = {
        'libdata': re.compile(r'AOCL-COMPRESSION \[(.*)\] \[Filename:(.*)\] --'),
        'comp': re.compile(r'Compression: (.*) speed\(best\) (.*) MB/s,'),
        'decomp': re.compile(r'Decompression: (.*) speed\(best\) (.*) MB/s,'),
        'ratio': re.compile(r'Ratio:(.*)\n'),
    }
    
    # Check if Error occurred
    if "Error" in line:
        print("\n" + line)
        exit()

    # If IPP option enabled
    if amd_opt == "IPP":
        rx_dict['libdata'] = re.compile(r'IPP \[(.*)\] \[Filename:(.*)\] --')
    
    # If Native API option enabled
    if amd_opt == "NAPI":
        rx_dict['libdata'] = re.compile(r'COMPRESSION Native API \[(.*)\] \[Filename:(.*)\] --')

    for key, rx in rx_dict.items():
        match = rx.search(line)
        if match:
            return key, match
    # if there are no matches
    return None, None


def proc(cmd_args, pipe=True):
    """
    Args:
        cmd_args: The command argument is passed as the cmd_args parameter.
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
    # Delete old file 
    remove_file_folder('parse_out.log')

    # open the file and read through it line by line
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


## ===================
# Execute the command 
## ===================

def run_command(cmd, filename='build_logs.log', print_command=True, print_output=False, print_error=True, param_shell=True):
    """
    Parameters:
        cmd: Pass the command for execution.
        filename: Save Console's output to file. Defaults to 'build_logs.log'.
        print_command: Print the command to the console before being executed.
        print_output: Print the command's output to the console.
        print_error: Print error messages to the console.
        param_shell: cmd string is passed to the system's shell for execution if it is true.
    Raises:
        RuntimeError: Function raises a RuntimeError if the command's return code is non-zero(means subprocess completed with error).
    Returns: Function returns a list of the lines of output generated with the command.
    """
    if print_command:
        write(cmd)
    popen = subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=param_shell,
        cwd=run_command.cwd)
    output, error = popen.communicate()
    output = output.decode()
    error = error.decode()
    if print_output:
        if output or (error and print_error):
            with open(filename, 'a') as f:
                f.write(output + '\n')
                f.write(error + '\n')
    if popen.returncode !=0:
        if error and not print_output and print_error:
            print(error)
        raise RuntimeError(output + error).splitlines()
    return (output + error).splitlines()

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

## =============================
# Calculate and print highlights 
## =============================

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
    comprspeed_y = round(geo_mean(csv3["ComprSpeed_AMD_OPT_ON"]), 2)
    comprspeed_n = round(geo_mean(csv3["ComprSpeed_{}".format(amd_opt)]), 2)
    dcomprspeed_y = round(geo_mean(csv3["DecomprSpeed_AMD_OPT_ON"]), 2)
    dcomprspeed_n = round(geo_mean(csv3["DecomprSpeed_{}".format(amd_opt)]), 2)
    comprRatio_y = round(geo_mean(csv3["ComprRatio_AMD_OPT_ON"]), 2)
    comprRatio_n = round(geo_mean(csv3["ComprRatio_{}".format(amd_opt)]), 2)

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
    if os.path.exists('out.log') and os.path.getsize('out.log') == 0:
        print(error + "The benchmark output file is empty. The tests might have failed")
    else:
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
        geo_mean(cspeed), 2), round(geo_mean(dspeed), 2), round(geo_mean(cratio), 2)])
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
    if amd_opt == "ipp" or amd_opt == "IPP":
        amd_opt = 'IPP'
    if amd_opt == "napi" or amd_opt == "NAPI":
        amd_opt = 'NAPI'

    method = "-e{}".format(method_name)
    testOptions = []
    compare = comparewith
    iters = "-i{}".format(iters)

    # Check and perform comparison tests if specified by the user 
    while run:
        print("\nCompression benchmarking started for the method {} with {} .....".format(
            method_name, amd_opt.replace("IPP", "IPP_OPT_ON")))
        remove_file_folder("out.log")
        for dataset in datasetFile_lst:
            testOptions = ["-t", "-p"]
            if amd_opt == "IPP" or amd_opt == "ipp":
                testOptions.append("-c{}".format(args.ipp))
            if amd_opt == "AMD_OPT_OFF" or amd_opt == "off":
                testOptions.append("-o")
            if amd_opt == "NAPI" or amd_opt == "napi":
                testOptions.append("-n")
            cmd = "{}/{}".format(args.dataset, dataset)
            testOptions.append(cmd)
            testOptions.append(method)
            testOptions.append(iters)
            testbench(testOptions, True)
        if os.path.exists('parse_out.log') and os.path.getsize('parse_out.log') == 0:
            print(error + "The output file is empty. The tests might have failed")
        else:
            create_tables('parse_out.log', amd_opt, method_name)

        if comparewith == "ipp":
            if ipp != 'off':
                amd_opt = "IPP"
            else:
                print(error + "Provide IPP library path with --ipp flag to compare")
                exit()
        elif comparewith == "vanilla":
            amd_opt = 'AMD_OPT_OFF'
        elif comparewith == "napi":
            amd_opt = 'NAPI'
        else:
            run = False
        comparewith = "off"

    if compare in ('ipp', 'vanilla', 'napi'):
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
        help='Specify custom compiler binary for building AOCL-Compression like -c compilerPath.')
    parser.add_argument(
        '--method', '-m',
        required=True, help='Pass a list of compression methods to run speed benchmark. Pass it like <method name>:<level>. For example: For zlib level 1, pass zlib:1 '
        'Supported methods and levels: lz4 - No levels, lz4hc - levels[1-12], snappy - No levels, lzma - levels[0-9], zlib - levels[1-9], zstd - levels[1-22], bzip2 - levels[1-9]', nargs="*")
    parser.add_argument(
        '--ipp', '-ipp',
        help='Provide IPP installed library path like for lz4: --ipp $PATH/lz4-1.9.3/lib) and for zlib: --ipp $PATH/zlib-1.2.11)', default=ipp)
    parser.add_argument(
        '--optimization', '-o', choices=['on', 'off', 'ipp', 'napi'],
        help='Specify one of the options: on(default - with AOCL optimization), off(without AOCL optimization), ipp(with IPP patch), napi(with NativeAPI)', type=str,
        default="AMD_OPT_ON")
    parser.add_argument(
        '--comparewith', '-cw', choices=['ipp', 'vanilla', 'napi'],
        help='Please specify a library for comparison, options are vanilla(AOCL optimization Vs Vanilla), ipp(AOCL optimization Vs IPP) napi(AOCL optimization Vs NativeAPI)', type=str
    )
    parser.add_argument(
        '--iterations', '-itr',
        help='Specify number of iterations for compression/decompression.', default=ITERS)
    parser.add_argument('--optionalFlags', '-flags',
                        help='Pass a list of supported optional flags (SNAPPY_MATCH_SKIP_OPT, AOCL_LZ4_OPT_PREFETCH_BACKWARDS, AOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST, AOCL_ZSTD_WILDCOPY_LONG and LZ4_FRAME_FORMAT_SUPPORT) as required. For example: -flags SNAPPY_MATCH_SKIP_OPT=ON AOCL_LZ4_OPT_PREFETCH_BACKWARDS=ON AOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST=ON AOCL_ZSTD_WILDCOPY_LONG=ON LZ4_FRAME_FORMAT_SUPPORT=ON',
                        type=str, nargs="*", default=[])
    parser.add_argument('--VSVersion', '-vs',
                        type=str, help='This is a Windows platform specific option. Use this option to provide an installed Visual Studio version available on the system. Default is "Visual Studio 17 2022"', default="Visual Studio 17 2022")
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
        
    # Linux specific library configuration and installation commands with default flags
    compression_cmds = {
        'config_cmd': 'cmake -B build . -DCMAKE_BUILD_TYPE=Release -DAOCL_LZ4_NEW_PRIME_NUMBER=ON -DSNAPPY_MATCH_SKIP_OPT=ON -DAOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST=ON -DCMAKE_INSTALL_PREFIX={}'.format(installation_path),
        'install_cmd': 'cmake --build build -v -j --target uninstall --target install'}
    # Windows specific library configuration and installation commands with default flags
    compression_cmds_windows = {
        'config_cmd': 'cmake -B build . -T ClangCl -G "{}" -DAOCL_LZ4_NEW_PRIME_NUMBER=ON -DSNAPPY_MATCH_SKIP_OPT=ON -DAOCL_ZSTD_SEARCH_SKIP_OPT_DFAST_FAST=ON -DCMAKE_INSTALL_PREFIX={}'.format(args.VSVersion, installation_path),
        'install_cmd': 'cmake --build ./build --config Release --target INSTALL'
    }
                
    def install_compression(cmds, optionalFlags):
        """
        Install aocl-compression library. This will generate libs/bin to the specified prefix path.

        """
        # Add all the optional flags if specified by the user
        for flag in optionalFlags:
            # Handle the duplication of the configuration flags 
            if flag not in cmds["config_cmd"]:
                if flag.split("=")[0] in cmds["config_cmd"]: 
                    flag_input = 'ON'
                    if flag.split("=")[1] == 'ON':
                        flag_input = 'OFF'
                    # Replace the configuration flags based on user input
                    cmds["config_cmd"] = cmds["config_cmd"].replace(flag.split("=")[0] + '=' + flag_input, flag)
                else:
                    cmds["config_cmd"] = cmds["config_cmd"] + " " + "-D{}".format(flag)   

        print("\nConfiguring AOCL-Compression for the installation .....\n")
        for k, val in cmds.items():
            # Check if the user passed a custom compiler path 
            if args.compiler:
                if os.path.exists(args.compiler):
                    # Check if compiler is clang/aocc
                    if "clang" in args.compiler:
                        # Get Clang C++ compiler path 
                        cxx_compiler = "{}++".format(args.compiler) 
                    # Check if compiler is gcc
                    elif args.compiler.endswith('gcc'):
                        # Get gcc C++ compiler path 
                        cxx_compiler = args.compiler[:-3] + 'g++'
                else:
                    # Check compiler path is not a valid path
                    print(error, "{} is not a valid compiler path.".format(args.compiler))
                    exit()

                # Get C compiler path
                c_compiler = args.compiler
                
                if k == 'config_cmd':
                    val = val + " " + "-DCMAKE_C_COMPILER={}".format(c_compiler) + " " + "-DCMAKE_CXX_COMPILER={}".format(cxx_compiler)
                else:
                    # If installation command running, Print the compiler name with library installing
                    print(
                            "Installing AOCL-Compression using {} Compiler..... \n".format(args.compiler.split("/")[-1]))
            else:
                # If installation command running, Print the compiler name with library installing
                if k == 'install_cmd':
                    if which_platform == 'Windows':
                        print(
                            "Installing AOCL-Compression using Clang Compiler ..... \n")
                    else:
                        print(
                            "Installing AOCL-Compression using gcc Compiler ..... \n")
                        
            # Check if the user wanted to collect build log
            if args.collectBuildlogs == "yes":
                run_command(val, 'build_logs.log', True, True)
            else:
                run_command(val, 'build_logs.log', True, False)
        
    # Check if test files are accessible
    if not os.path.exists(args.dataset) and not os.path.isfile(args.dataset):
        os.mkdir(args.dataset)
    if not os.listdir(args.dataset):
        print("Input dataset is not available at specified path: {}. So, downloading Silesia dataset to the path \n".format(args.dataset))
        download_datasets(DEFAULT_DATASETS, args.dataset)

    # Make list of the datasets that are available in the directory 
    datasetFile_lst = sorted(os.listdir("{}".format(args.dataset)))
    ipp_supports_methods = ['lz4', 'lz4hc', 'zlib', "bzip2"]

    # Checks IPP supports before running IPP patched method
    if (args.ipp != "off" or "ipp" in [args.comparewith, args.optimization]):
        if (not ((args.method[0].split(":")[0] in ipp_supports_methods) and which_platform != 'Windows')):
            print(error + "Compression test framework does not support IPP method: {} on {}".format(args.method, which_platform))
            exit()
    
    ## =====================================
    # Check pre test settings and conditions  
    ## ======================================

    if args.comparewith and args.optimization == "off":
        raise ValueError("comparewith doesn't support execution with optimization off")

    if "ipp" in [args.comparewith, args.optimization] and args.ipp == "off":
        print(error + "Provide the IPP library path using --ipp flag to perform IPP \n")
        exit()
    elif args.comparewith == "vanilla" and args.ipp != "off":
        print(error + "Comparewith(vanilla) and IPP path flags can not be enabled together \n")
        exit()
    elif args.comparewith == "ipp" and len(args.method) >1:
        print(error + "--method/-m argument does not accept list of methods when comparing with IPP. Instead, only pass the specific method that you want to compare with IPP.")
        exit()
    
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

    print("Compression benchmarking tests are completed. Please check the generated reports for details.")
    
