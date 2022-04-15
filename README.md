Elf Excavator
=============

Elf Excavator is a Graphical ELF file inspector. It is primarily intended to assist
embedded developers deciper the contents of their elf files, such as determining what
symbols and sections are taking up the most space, what assembly was generated for a
function, and searching/inspecting section contents.

Usage
-----

Elf Excavator can be launched using the following:

```bash
elfexcavator --file "<path-to-elf>"
```

Bulding & Installing
--------------------

_This guide assumes and ubuntu system._

To build Elf Excavator, you will need the dependencies in `tools/scripts/setup.sh`.

>**_NOTE_:** This project requires `cmake>=20`. ubuntu<22.04 does not provide this in
`apt`. Consider using `pip`, `snap`, or [Kitware's download](https://cmake.org/download/).

```bash
cmake --preset=install
cmake --build --preset=install
# Either install directly
sudo cmake --build --preset=install --target install
# Or use the .deb package generator
pushd build/install && cpack -G DEB; popd
```
