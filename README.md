Simple library providing quick access to cmd line arguments. Main features:

* Uses root TEnv storage.
* Allows to load defaults from a file.
* Allows to define global and local arguments.

* Supports following data types: bool, int, double, string;

# Depedencies

* set of usefull cmake scripts from ```https://github.com/StableCoder/cmake-scripts```

# Installation

### Clone the repository

```git clone https://github.com/SiFi-CC/CmdLineArgs```

### Init submodules

```cd CmdLineArgs```

```git submodule update --init --recursive```

    or
```./prepare_tree.sh```

### Prepare build

```mkdir build && cd build```

```cmake .. # -DCMAKE_INSTALL_PREFIX=... some path other than default /usr/local```

```make ```

and optional

```make install```

# Usage

CmdLineArgs supports three types of arguments:
* optional - starting with ```-```
* positional
* positional greedy

Optional are self explaining.

Positional arguments are those which are required by the application and the order of them is well defined. Sometimes the list of optional argument is open and then extra arguments are defined as greedy.

For example a ```hadd``` program from the ROOT framework to add histograms from many files. It is called in following way:

    hadd [-f] output_file.root input_file.root [more input files]

* ```-f``` is an optional argument, means force to overwrite to ```output_file.root```
* ```output_file.root``` is positional and is required, must be given at the first position
* ```input_file.root``` is positional and is also required, something must be add to the output file
* ```[more input files]``` are optional, all those will be also add to the output

## Define command line optional arguments

Define the options in the beginning of the ```main``` function or the global scope of the library (not recommended).

    CmdLineOption bool_val("CustomBoolArgName", "-bool", "Help message" , true);
    CmdLineOption int_val("CustomIntegerArgName", "-int", "Help message" , 13);
    CmdLineOption double_val("CustomDoubleArgName", "-double", "Help message" , 3.1415);
    CmdLineOption string_val("CustomStringArgName", "-string", "Help message" , "pi");

## Parse the command line optional arguments

    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

## Access the command line arguments in any place of the code (functions, classes, libraries)

    std::cout << "Bool value   = " << CmdLineOption::GetBoolValue("CustomBoolArgName") << std::endl;
    std::cout << "Int value    = " << CmdLineOption::GetIntValue("CustomIntegerArgName") << std::endl;
    std::cout << "Double value = " << CmdLineOption::GetDoubleValue("CustomDoubleArgName") << std::endl;
    std::cout << "String value = " << CmdLineOption::GetStringValue("CustomStringArgName") << std::endl;

## Define command line positional arguments

    CmdLineArg(const char* name, const char* help, OptionType type, void (*f)() = nullptr, bool greedy = false);
e.g.

    CmdLineArg pos1("name", "name of file", CmdLineArg::kString);

To define argument you must give a name at which it will be identified, the help string and type of data, among of ```kFlag```, ```kBool```, ```kInt```, ```kDouble```, ```kString```.

There can be as many as needed positional but they have to have different names.

### Greedy arguments

To define greedy arguments call ```AmdLineArg``` with empty name. Only one greedy argument is allowed.

    CmdLineArg pos_g("", "more files", CmdLineArg::kString);

### Example

Order of positional and greedy arguments matter and is used to properly map command line arguments to certain groups. To reproduce behaviour of ```hadd``` command one would define following options and arguments:

    CmdLineOption opt_force("ForceOverwrite", "-f", "Overwrite output if exists");
    CmdLineArg arg_outf("output", "output file", CmdLineArg::kString);
    CmdLineArg arg_inpf("input", "input file", CmdLineArg::kString);
    CmdLineArg arg_inpfs("", "more input files", CmdLineArg::kString);

The help message for a such definitions looks like:

    Usage: ./example [options] output input [...]
      -h                  show this help
      -dd                 Set path to data directory (char*)
      -f                  Overwrite output if exists (flag)
       output             output file (char*)
       input              input file (char*)
       [...]              more input files (char*)

See also ```example.cc``` for minimal usage example.

### Access arguments

Optional argument can be obtain using:

    const Positional& pargs = CmdLineConfig::instance()->GetPositionalArguments();

Optional arguments are accessable by name. The ```Positional``` object is a map mapping string (name) to ```CmdLineArg``` object pointer. Recommended way of accessing in the example of ```hadd```:

    TString outf = pargs.at("output")->GetStringValue();
    TString inpf = pargs.at("input")->GetStringValue();

To access geedy, use ```Greedy``` object and appropriate function. Greedy abjects are vectors, tyhey hav no name so access is by index:

    const Greedy& gargs = CmdLineConfig::instance()->GetGreedyArguments();
    gargs[0]->GetStringValue();

### Other order

The example of ```hadd``` could be also write as follows:

    CmdLineOption opt_force("ForceOverwrite", "-f", "Overwrite output if exists");
    CmdLineArg arg_outf("output", "output file", CmdLineArg::kString);
    CmdLineArg arg_inpfs("", "more input files", CmdLineArg::kString);
    CmdLineArg arg_inpf("input", "input file", CmdLineArg::kString);

and result in

    Usage: ./example [options] output [...] input
      -h                  show this help
      -dd                 Set path to data directory (char*)
      -f                  Overwrite output if exists (flag)
       output             output file (char*)
       [...]              more input files (char*)
       input              input file (char*)

## From terminal

From the command line:

    myexec -a aaa -b bbb -c ccc xxx yyy zzz

```-a```, ```-b```, ```-c``` are command line options with ```aaa```, ```bbb```, ```ccc``` being they values.
```xxx```, ```yyy```, ```zzz``` are positional arguments.

Special options:

* ```-h``` - will list of all available options
* ```-p``` - will print names of the parameters and their current (or default) values, order of these arguments matters


# Credits

Major art of the code is based on RootSorter SorterConfig developed for COSY
Jülich.

Further changes done by Rafał Lalik (Rafal.Lalik@uj.edu.pl).
