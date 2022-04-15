CONTRIBUTING
============

Contributors are welcome, but because this project is still in an alpha state, roadmaps
and priorities could change dramatically. Bug reports and feature requests are greately
appreciated.

If you wish to contribute, please adhere to the following guidelines:

1. Be courteous, this is a project run by fallible volunteers in their spare time. It
should go without saying that harassment of any kind is not tolerated.
2. Strive for readability first and comment thoroughly. Qt makes `new` necessary in some
cases, so always comment how that memory gets freed, preferably while linking
documentation. Code should look similar to surrounding code. Commits should be
descriptive.
3. Code published to this repo is currently licensed under the GPLv3, as is required to
fulfill the licensing obligations of its dependencies.

Pull Requests
-------------
Please rebase off master before opening a PR. CI is expected to pass before PRs undergo
review. Let's keep the warning count under 50.

Contributors
------------
* _This could be you!_

Implementation Notes
====================

Elf Excavator roughly follows a Model View Presenter architecture. We reinterpret the
"Model" in Qt's traditional Model/View/Delegate as a presenter for our purposes.

Build System
------------

The currently selected vcpkg baseline does not work for the "big" libraries. System
packages should generally be preferred when possible, otherwise `FetchContent` will be
used to auto-download packages. This behavior can be disabled with advanced cmake
options `ElfExcavator_FETCH_<Dependency>`.

TODOS
-----
I'll be recording what needs doing here until Elf Excavator is out of alpha.

* Add Translation Support
* Bundle Licenses in Help -> About
* Code Coverage
* Create LeakSanitizer suppressions for libfontconfig
* Create a Desktop Icon
* Document Design Decisions
* Extend Unit Test Coverage
* Github Actions
* Install a .desktop file
* Investigate KDAB Dock Widget
* List Total Allocated Section/Symbol Size
* Proper LMA Support
* Reload ELF File without Restart
* Settings Menu
* Smarter objdump detection
* Windows build
