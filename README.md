# drui - Declarative Reactive UI in C++
I'm mainly a C++ programmer, and I'm certainly not a UI specialist. But, having dabbled with [Flutter](https://flutter.dev/) and having seen some [recent news on React](https://react.dev/blog/2024/02/15/react-labs-what-we-have-been-working-on-february-2024), I thought I'd try my hand at writing a [declarative reactive gui](https://docs.flutter.dev/get-started/flutter-for/declarative) in C++.

## Build system
I code on Windows and use premake5 to configure Visual Studio solutions. In the `build` directory you'll find the following batch files:
- `config.bat` Use this to create `drui.soln`.
- `clean.bat` Use this to clean the solution.
- `b.bat` Use this to build the solution. (Be sure you run this from a Developer Command Prompt.)
- `r.bat` Use this to run the built solution.

## Dependencies
You'll see from `build/premake5.lua` that I link against some external libraries, found on my system outside the solution under a folder `lib`. These are the x64 SDL libraries `SDL2` `SDL2_image` `SDL2_ttf` and `SDL2_gfx`. This is how these external dependencies are laid out under my `lib` folder:
```
bin/SDL2.dll
bin/SDL2_image.dll
bin/SDL2_ttf.dll

link/SDL2/SDL2.lib
link/SDL2/SDL2_image.lib
link/SDL2/SDL2_ttf.lib
link/SDL2/SDL2_gfx.lib

include/SDL2/SDL*.h
```
Note:
- the `lib/bin` folder is in my `PATH` environment variable.
- the entry `SDL*.h` denotes **all** the header files provided by the above-mentioned SDL2 libraries. (Yes, I put them all in one folder.)
