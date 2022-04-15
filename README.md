Elf Excavator
=============

Elf Excavator is a Graphical ELF file inspector. It is primarily intended to assist
embedded developers deciper the contents of their elf files, such as determining what
symbols and sections are taking up the most space, what assembly was generated for a
function, and searching/inspecting section contents:

Section searching uses decimal integers, floating point numbers, UTF-8 strings, or raw
hexadecimal bytes:

![sections](docs/img/sections.jpg)

Each column can be filtered. Text is filtered using regular expressions, and numbers
are filtered using simple expressions in the form of `<comparator> <value>`, where
`<comparator>` is `>,>=,<,<=,==,!=`, and value is a decimal or hexadecimal integer.
If no comparator is supplied, `==` is assumed. Invalid filter terms will be outlined in
red.

![symbols](docs/img/symbols.jpg)

The lovely pickaxe icon was created by
[wanicon](https://www.flaticon.com/authors/wanicon)
from [flaticon.com](https://www.flaticon.com/free-icons/pickaxe).

Usage
-----

Elf Excavator can be launched using the following:

```bash
elfexcavator "<path-to-elf>"
```

Bulding & Installing
--------------------

_This guide assumes an ubuntu system._

To build Elf Excavator, you will need the dependencies in `tools/scripts/setup.sh`.

>**_NOTE_:** This project requires `cmake>=20`. ubuntu<22.04 does not provide this in
`apt`. Consider using `pip`, `snap`, or [Kitware's download](https://cmake.org/download/).

```bash
cmake --preset=install
cmake --build --preset=install
# Either install directly
sudo cmake --build --preset=install --target install
# Or use the .deb package generator, use "dpkg -c *.deb; dpkg -I *.deb" to inspect contents
pushd build/install && cpack -G DEB; popd
```
