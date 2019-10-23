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


## Define command line arguments

Define the options in the beginning of the ```main``` function or the global scope of the library (not recommended).

    CmdLineOption bool_val("CustomBoolArgName", "-bool", "Help message" , true);
    CmdLineOption int_val("CustomIntegerArgName", "-int", "Help message" , 13);
    CmdLineOption double_val("CustomDoubleArgName", "-double", "Help message" , 3.1415);
    CmdLineOption string_val("CustomStringArgName", "-string", "Help message" , "pi");

## Parse the command line arguments

    CmdLineConfig::instance()->ReadCmdLine(argc, argv);

## Access the command line arguments in any place of the code (functions, classes, libraries)

    std::cout << "Bool value   = " << CmdLineOption::GetBoolValue("CustomBoolArgName") << std::endl;
    std::cout << "Int value    = " << CmdLineOption::GetIntValue("CustomIntegerArgName") << std::endl;
    std::cout << "Double value = " << CmdLineOption::GetDoubleValue("CustomDoubleArgName") << std::endl;
    std::cout << "String value = " << CmdLineOption::GetStringValue("CustomStringArgName") << std::endl;

## Access positional arguments

    PositionalArgs pargs = CmdLineOption::GetPositionalArguments();

Since ```PositionalArgs``` is a typedef of ```std::vector<TString>``` all ```std::vector``` members apply to it, like:

    pargs.size()

will return the numer of arguments.


See ```example.cc``` for minimal usage example.

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
