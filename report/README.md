A paper template
================

## How to import this template to your paper repository

```bash
git subtree add --prefix paper git@github.com:cosmoss-vt/paper-template.git master
```

## Prequisites
- Up-to-date system
- Python 3.x
- Pygmentize
- PdfCrop
- TexLive
- Python Developer kits
- Pip packages
- LibreOffice

### Fedora:

```bash
sudo dnf update && sudo dnf install python3 python-pygments texlive* python2-devel python3-devel python-tkinter && pip install matplotlib pandas
```


## How to create the pdf

### Step 1. Build
```bash
make distclean && make
```

### Step 2. Continuous build
```bash
while [ 1 ]; do make; sleep 3; done
```

### Step 3. Work Environment
Once you save your change, just run `make` to recompile the pdf (p.pdf)


## Contribute back
Once you finish submitting a paper, please contribute back your *.bib
files to this repository.


## Other useful tools
- [Tables Generator](https://www.tablesgenerator.com/)

