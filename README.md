# TCSS 372 Final Project 6

Welcome to the LC-3 simulator! <deleteThisChange>

## Getting Started

Depending on which branch of development you're working with, you can run the following command to build and run the program in one go.
```
make && ./a.out
```

## Coding style

Most of us are using Visual Studio Code at this point. To make formatting our source code files easier try to use the `Format Document` tool each time before pushing to Github (Ctrl+Shift+F OR Ctrl+Shift+I depending on your operating system).

There are many format styles available to use by VS Code. The default `Visual Studio` style will use more lines than necessary for method brackets and other things. There is now a file called .clang-format in the root of this project which VS Code will automatically use to format the document. You can check your User Settings (File -> Preferences -> Settings) to make sure the following settings are in your user settings file (the right side pane). If these settings are not in your user settings file that means you don't need to worry as they haven't been changed from the default. The `.clang-format` file is exactly the same as the LLVM format style except the column limit has been increased from 80 to 110, and the tab size has been increased from 2 to 4 spaces.
```
{
    "C_Cpp.clang_format_fallbackStyle": "LLVM",
    "C_Cpp.clang_format_style": "file"
}
```
[Further reading on code formatting: https://embeddedartistry.com/blog/2017/10/23/creating-and-enforcing-a-coding-standard-with-clang-format](https://embeddedartistry.com/blog/2017/10/23/creating-and-enforcing-a-coding-standard-with-clang-format)

## Building manually

Your system should already have NCURSES installed. Some systems (like a barebones installation of Ubuntu make not have ncurses installed, in which case just do sudo apt-get install ncurses). The following build command instructs the c-compiler to use the system-specific version of ncurses in the program.

-g: This embeds debug information into the compiled file which allows step through debugging of the LC-3 emulator possible.

-lncurses: This links your system's ncurses library with the LC-3 simulator. Linux, mac and windows all have different terminals and command prompts, so this is necessary

-lmenu: Links the menus extension of ncurses.

-lm: Links the math library. (Probably not necessary unless you're running a stripped version of ubuntu)

```
gcc -g debug_monitor.c slc3.c -lncurses -lmenu -lm
```

## Debugging

### with VSCode

VSCode is really nice for debugging C. With the C/C++ Extensions (the one from Microsoft with 7.6 million downloads) you can create a debug configuration in the debug menu (Ctrl+Shift+D). Select C/C++ (GDB/LLDB), give it path of your executable (probably ./a.out)

For reference, the configuration file looks like this
```
"configurations": [
    
    {
        "name": "(gdb) Launch",
        "type": "cppdbg",
        "request": "launch",
        "program": "${workspaceFolder}/a.out",
        "args": [],
        "stopAtEntry": false,
        "cwd": "${workspaceFolder}",
        "environment": [],
        "externalConsole": true,
        "MIMode": "gdb",
        "setupCommands": [
            {
                "description": "Enable pretty-printing for gdb",
                "text": "-enable-pretty-printing",
                "ignoreFailures": true
            }
        ]
    }
]
```
### with Valgrind

If you're dealing with a segfault, it's faster to first check with Valgrind what line of code is causing the segfault before setting breakpoints and stepping through code. After you install valgrind you can run the program with the following command:

```
make && valgrind --log-file="valgrind_report.txt" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes -v ./a.out
```

Run the program as normal until you're ready to exit, or you encounter a segfault.. Then valgrind will write out a full report about how the program was running (memory leaks, e.t.c) as well as detailed information about any segfaults.
