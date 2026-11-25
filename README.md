# Bandyopadhyay-Mehler Hydropathy Plot

A static website that computes the Bandyopadhyay-Mehler hydropathy scale with a sliding window,
plots it and lets you download the results as CSV, SVG or PNG.

It is a JavaScript port of [`c/finalwebcode.c`](c/finalwebcode.c) that runs entirely in the browser, so it can be
hosted for free on GitHub Pages with no server. It uses the same scores, the same sliding-window calculation and the
same CSV format as the C program (`CenterPosition,AminoAcid,AverageHydrophobicityScore`, numbers printed like `%.17g`).

## Features
- Paste a plain sequence or multi-sequence FASTA, or upload a `.fasta` file
- Choose any window size (3 by default, the same as the C program)
- Plot for each sequence, with SVG/PNG download
- CSV for each sequence, or all of them as a zip (`Hydropathy_CSV_window_length_N/`)

## How the calculation works
- Each amino acid is given its score from Table II ("This work") of the reference paper below.
- Glycine (G) is not listed in the paper's table and is given 0.00. Any unrecognised letter also scores 0.00.
- A window of the chosen size slides along the sequence one residue at a time, and the average score of the window
  is assigned to the residue at its center.
- Only full windows are used, so a sequence of length L with window size w gives L − w + 1 rows. The first and last
  few residues therefore have no value. If the sequence is shorter than the window, the CSV contains only the header.
- With an odd window size (3, 5, 7, ...) the window is exactly centered on a residue. With an even window size
  there is no single middle residue, so the residue just right of the middle is used.

## Files
```
index.html   page, including the JavaScript (hydropathy calculation, plotting, downloads)
style.css    styles
assets/      logo, example figures, reference paper
c/           original C program
.nojekyll    tells GitHub Pages to serve the files as they are
README.md    this file (shown on GitHub only, not used by the website)
```

## Run locally
Open `index.html` in a browser. Keep the `assets/` and `c/` folders next to it, otherwise the logo, figures and links
will not load.

## Publish on GitHub Pages
1. Create a new public repository on GitHub, or use an existing one. Below, `<owner>` is the GitHub account that owns
   the repository and `<repo-name>` is its name.
2. In this folder, run:
   ```
   git init
   git add .
   git commit -m "Hydropathy plot website"
   git branch -M main
   git remote add origin https://github.com/<owner>/<repo-name>.git
   git push -u origin main
   ```
3. On GitHub, go to **Settings → Pages**. Under **Build and deployment**, set Source to **Deploy from a branch**,
   choose branch `main` and folder `/ (root)`, then click **Save**.
4. After a minute the site is live at `https://<owner>.github.io/<repo-name>/`.

## Running the C program on your own computer
The website is the easiest way to use this tool. The C program does the same calculation from the command line,
which is handy for many sequences at once.

### 1. Get a C compiler
- **macOS:** open Terminal and run `xcode-select --install`. This installs `gcc`.
- **Linux:** `gcc` is usually installed already. If not, install it with your package manager
  (for example `sudo apt install build-essential` on Ubuntu or Debian).
- **Windows:** the program will not compile as written with the usual Windows compilers. Install
  [WSL](https://learn.microsoft.com/windows/wsl/install) (Windows Subsystem for Linux), open the Linux terminal
  and follow the Linux steps.

Check it works by running `gcc --version`.

### 2. Download the program
Download `c/finalwebcode.c` from this repository (or clone the whole repository) and open a terminal in the folder
that contains it.

### 3. Prepare your FASTA file
The program reads a FASTA file: each sequence starts with a header line beginning with `>`, followed by the
sequence on one or more lines. For example, save this as `my_sequences.fasta`:
```
>2CZS
MVSGEVRTKKVPLDTNHKRFYDA
>another_protein
MKTAYIAKQRQISFVKSHFSRQ
```
Every sequence needs a `>` header line, otherwise it is skipped.

### 4. Point the program at your file
Open `finalwebcode.c` in any text editor and find these lines near the top:
```c
#define WINDOW_LENGTH 3
#define FASTA_FILE "/path/to/your/sequences.fasta"
```
- Replace the path inside the quotes with the full path to your own FASTA file, for example
  `"/home/yourname/my_sequences.fasta"`. Keep the quotation marks.
- To use a different window size, change `3` in `WINDOW_LENGTH` to another whole number.

Save the file. You must do this **before** compiling, and compile again every time you change either line.

### 5. Compile and run
```
gcc -o hydropathy finalwebcode.c -lm
./hydropathy
```
The `-lm` flag links the math library, which some Linux setups need; it is harmless elsewhere. Adjust the first command's file name if your terminal is not in the same folder as the program
(for example `gcc -o hydropathy c/finalwebcode.c -lm`). You will see a line like `Processing sequence 1/2: 2CZS` for each
sequence, and `Hydropathy values (centered window) saved successfully.` at the end.

### 6. Find the results
The program creates a folder called `Hydropathy_CSV_window_length_3` (the number is the window size) in the
folder you ran it from, with one CSV file per sequence, named `<sequence id>_window_<size>.csv`. Each file looks like
this (numbers shortened here; the real files have about 17 significant digits):
```
CenterPosition,AminoAcid,AverageHydrophobicityScore
2,V,0.4733...
3,S,0.2933...
4,G,-0.0633...
```
The CSV files are the same format that the website produces, so you can open them in Excel or Google Sheets.

### Differences from the website
- The C program only reads FASTA files that have `>` header lines. The website also accepts a bare sequence.
- It stops after 500 sequences. The website has no limit.
- The window size is set in the source code (step 4). On the website you type it into a box.

## Reference
Bandyopadhyay, D., & Mehler, E. L. (2008). Quantitative expression of protein heterogeneity: Response of amino acid
side chains to their local environment. *Proteins*, 72(2), 646–659. https://doi.org/10.1002/prot.21958
