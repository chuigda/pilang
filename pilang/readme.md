# Pi-Lang introduction
<br/>

## brief
PiLang is a tiny programming language for very beginners and casual
devices such as raspberry pi and smart phones. "Pi" means raspberry
__Pi__, a kind of small computer for educations.

## Features
### No `Shift` key
PiLang avoids using shift key which is indended for inputing special
characters frequently.

C++:
```
int rfact(int n) {
  if (n == 1) return 1;
  else return n * rfact(n-1);
}
```

PiLang:
```
function rfact takes n returns f begin
  if n eq 1 then
    f = 1;
  else
    f = n times rfact[n minus 1];
  end if
end function
```

PiLang eliminates shift keys by replacing symbols with corresponding
words. This is useful when user does not have a standard keyboard. 
However, It is also possible to use these symbols directly. What's more,
one symbol may have more than one replacement, so it is unnecessary to
memorize these replacements.

### Dynamically typed
PiLang is dynamically typed. Variable does not have to be declared
before its first use, and PiLang does not require types to be written
out explicitly. This may be friendly to freshmen. <del>However, type
annonations are also possible, and linting tools will come out 
 soon.</del>
There will never be any type annonations and linting tools.

## Building from source
### Building on linux
To build PiLang on linux, you need the following packages
* `markdown`
* `YACC`(`bison`) _note: `lex` is not required_
* `tree`
* `cc`

For debian users, simply install these packages by `apt`:

`sudo apt install bison markdown gcc tree`

Checkout the project with git, get into the pilang directory, and then
`make`.

### Building on windows
To build PiLang on windows, it is recommended to use a MinGW32 GCC
compiler, mingw32-make and a GnuWin32 Bison. These packages will soon be
available for download from icey's site. After installing these
packages, simply get into the pilang directory and then

`mingw32-make --file="makefile.win"`
