# Bandyopadhyay-Mehler Hydropathy Plot

A static website that computes the Bandyopadhyay-Mehler hydropathy scale with a centered sliding window,
plots it and lets you download the results as CSV, SVG or PNG.

It is a JavaScript port of [`c/finalwebcode.c`](c/finalwebcode.c) that runs entirely in the browser, so it can be
hosted for free on GitHub Pages with no server. The CSV output is identical to the C program's
(`CenterPosition,AminoAcid,AverageHydrophobicityScore`, numbers printed like `%.17g`).

## Features
- Paste a plain sequence or multi-sequence FASTA, or upload a `.fasta` file
- Choose any window size (the C program uses 3)
- Plot for each sequence, with SVG/PNG download
- CSV for each sequence, or all of them as a zip (`Hydropathy_CSV_window_length_N/`)

## Files
```
index.html   page, including the JavaScript (hydropathy calculation, plotting, downloads)
style.css    styles
assets/      logo, example figures, reference paper
c/           original C program
.nojekyll    tells GitHub Pages to serve the files as they are
```

## Run locally
Open `index.html` in a browser.

## Publish on GitHub Pages
1. Create a new public repository on GitHub, for example `hydropathy-web`.
2. In this folder, run:
   ```
   git init
   git add .
   git commit -m "Hydropathy plot website"
   git branch -M main
   git remote add origin https://github.com/<your-username>/hydropathy-web.git
   git push -u origin main
   ```
3. On GitHub, go to **Settings → Pages**. Under **Build and deployment**, set Source to **Deploy from a branch**,
   choose branch `main` and folder `/ (root)`, then click **Save**.
4. After a minute the site is live at `https://<your-username>.github.io/hydropathy-web/`.

## Running the C program
```
gcc -o hydropathy c/finalwebcode.c -lm
./hydropathy
```
Before compiling, set `FASTA_FILE` in the source to the path of your FASTA file.

## Reference
Bandyopadhyay, D., & Mehler, E. L. (2008). Quantitative expression of protein heterogeneity: Response of amino acid
side chains to their local environment. *Proteins*, 72(2), 646–659. https://doi.org/10.1002/prot.21958
# Bandyopadhyay-Mehler-Hydrophobicity-scale
# Bandyopadhyay-Mehler-Hydrophobicity-scale
