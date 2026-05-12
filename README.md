# Thesis

**Author:** Lukáš Pšeja  
**Supervisor:** Dr. Ing. Petr Peringer  

## Requirements

### Debian/Ubuntu

```bash
sudo apt install texlive-full
```

## Getting Started

### 1. Clone the repository

```bash
git clone -b thesis git@github.com:pseja/gcc-plugin-for-static-analyzer-support.git
cd gcc-plugin-for-static-analyzer-support
```

### 2. Build the PDF

```bash
make
```

This produces `xpsejal00-gcc-plugin.pdf` in the current directory, which is the submitted version of the text.

### 3. Clean build artifacts

```bash
make clean
```

Removes all intermediate files and the output PDF.
