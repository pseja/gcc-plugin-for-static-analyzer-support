# GCC Plugin for Static Analyzer Support

**Author:** Lukáš Pšeja  
**Supervisor:** Dr. Ing. Petr Peringer

## Abstract

> This bachelor’s thesis modernizes the Code Listener infrastructure, which serves as an integration layer between compilers and static analyzers for C and C++ programs. It first analyzes the original architecture and identifies its main limitations, namely mandatory transformations and the tight coupling of analysis results with the internal program representation. Based on this analysis, the thesis designs and implements a new GCC plugin built around a compiler-independent CodeModel, lazily evaluated annotation services, a unified analyzer interface, and JSON export for offline processing. The solution preserves backward compatibility with Predator through an adapter that reconstructs the original callback stream from the new CodeModel. Correctness and practical viability were validated by native regression suites and by 1 192 Predator compatibility tests executed after rebuilds with GCC 12, 13, 14, and 15.

## Repository Layout

- [implementation branch](https://github.com/pseja/gcc-plugin-for-static-analyzer-support/tree/implementation) - The new GCC plugin implementation
- [presentation branch](https://github.com/pseja/gcc-plugin-for-static-analyzer-support/tree/presentation) - The files used to create the presentation for the defense
- [thesis branch](https://github.com/pseja/gcc-plugin-for-static-analyzer-support/tree/thesis) - The LaTeX source of the thesis

More information can be found in the respective `README.md` files inside the `implementation`, `presentation`, and `thesis` branches.
