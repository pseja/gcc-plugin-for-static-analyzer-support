# Thesis

**Author:** Lukáš Pšeja  
**Supervisor:** Dr. Ing. Petr Peringer

## Requirements

### Debian/Ubuntu

```bash
sudo apt install texlive-full
```

## Usage

### Clone the repository

```bash
git clone -b thesis git@github.com:pseja/gcc-plugin-for-static-analyzer-support.git thesis
cd thesis
```

### Build the PDF

```bash
make
```

This produces `xpsejal00-gcc-plugin.pdf` in the current directory, which is the submitted version of the text. 
> The officially published PDF is also available [here](https://www.vut.cz/www_base/zav_prace_soubor_verejne.php?file_id=293456).

### Clean build artifacts

```bash
make clean
```

Removes all intermediate files and the output PDF.
