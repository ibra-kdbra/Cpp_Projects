# About ModernCLI

This is a package to collect and centralize testing for many inline header
libraries I had written and then duplicated in other packages.  These headers
are being gathered under a common package that can then be vendored or use
moderncli as a core dependency.  Hence, one interpretation for for this package
is "Modern C++" Library Interfaces.

## Participation and Documentation

Basic documentation is provided as markdown files. Developer documentation can
be generated from source file headers using Doxygen and the ``docgen'' target.
Documentation generated from the latest release tag is found at
<https://doc.gnutelephony.org/moderncli>.

A more complete overview of participation is provided in CONTRIBUTING.md. This
project uses cmake, and c++17 for core development. I use the ctest
framework for unit testing and gcovr for coverage reports. ModernCLI can be
built with gcc or clang and can be tested on just about any posix platform,
including bsd systems. It may also build and run on Microsoft Windows, and I
may even test this with mingw and wine on Debian in the future.

## Support

Support is offered thru <https://git.gnutelephony.org/moderncli/issues>. When
entering a new support issue, please mark it part of the support project. I
also have dyfet@jabber.org. I may maintain system packaging for some GNU/Linux
distributions, including Arch and Debian.  I also have my own build
infrastructure for Alpine Linux using ProduceIt and I publish apk binary
packages thru <https://public.tychosoft.com/alpine>. In the future maybe other
means of support will become possible.
