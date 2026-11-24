/*
 Computes hydropathy scale using a centered sliding window and saves CSV
 output.
 Compile:  gcc -o codewithchanges codewithchanges.c -lm
 Run:      ./codewithchanges
*/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define WINDOW_LENGTH 3
#define MAX_LINE_LEN 4096
#define MAX_SEQ_LEN 100000
#define MAX_SEQUENCES 500
#define MAX_ID_LEN 1024
#define OUTPUT_FOLDER "Hydropathy_CSV_window_length_3"
#define FASTA_FILE "/Users/shauryasood/Documents/DOP/sequences_new_og.fasta"

/* Hydropathy scores */

static double hydropathy_score(char aa) {
  switch (toupper((unsigned char)aa)) {
  case 'G':
    return 0.00;
  case 'C':
    return 1.15;
  case 'I':
    return 0.97;
  case 'L':
    return 0.87;
  case 'F':
    return 0.85;
  case 'V':
    return 0.83;
  case 'W':
    return 0.67;
  case 'Y':
    return 0.60;
  case 'M':
    return 0.54;
  case 'A':
    return 0.33;
  case 'P':
    return 0.32;
  case 'H':
    return 0.25;
  case 'T':
    return 0.21;
  case 'S':
    return 0.05;
  case 'R':
    return -0.01;
  case 'Q':
    return -0.05;
  case 'N':
    return -0.07;
  case 'D':
    return -0.22;
  case 'E':
    return -0.24;
  case 'K':
    return -0.40;
  default:
    return 0.00; /* unknown residue → 0.0 */
  }
}

/* Data structures  */

typedef struct {
  char id[MAX_ID_LEN];
  char *seq; /* dynamically allocated */
  int seq_len;
} FastaEntry;

/* ── Sanitize filename (replaces \ / : * ? " < > | [ ] ( ) with '_') ── */

static void sanitize_filename(const char *src, char *dst, size_t dst_size) {
  size_t j = 0;
  for (size_t i = 0; src[i] != '\0' && j < dst_size - 1; i++) {
    char c = src[i];
    if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' ||
        c == '<' || c == '>' || c == '|' || c == '[' || c == ']' || c == '(' ||
        c == ')') {
      dst[j++] = '_';
    } else {
      dst[j++] = c;
    }
  }
  dst[j] = '\0';
}

/*Parse FASTA file */

static int parse_fasta(const char *filename, FastaEntry *entries,
                       int max_entries) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "Error: cannot open FASTA file '%s': %s\n", filename,
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  char line[MAX_LINE_LEN];
  int count = -1;  /* index of current entry */
  int seq_cap = 0; /* capacity of current seq buffer */

  while (fgets(line, sizeof(line), fp)) {
    /* strip trailing newline / carriage return */
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
      line[--len] = '\0';

    if (line[0] == '>') {
      /* new header */
      count++;
      if (count >= max_entries) {
        fprintf(stderr,
                "Warning: exceeded max sequences (%d). "
                "Increase MAX_SEQUENCES.\n",
                max_entries);
        count--;
        break;
      }
      /* store id (everything after '>') */
      strncpy(entries[count].id, line + 1, MAX_ID_LEN - 1);
      entries[count].id[MAX_ID_LEN - 1] = '\0';

      /* allocate initial sequence buffer */
      seq_cap = MAX_LINE_LEN;
      entries[count].seq = (char *)malloc(seq_cap);
      if (!entries[count].seq) {
        fprintf(stderr, "Error: out of memory\n");
        exit(EXIT_FAILURE);
      }
      entries[count].seq[0] = '\0';
      entries[count].seq_len = 0;
    } else if (count >= 0 && len > 0) {
      /* append sequence data (uppercase) */
      int new_len = entries[count].seq_len + (int)len;
      if (new_len + 1 > seq_cap) {
        seq_cap = new_len * 2 + 1;
        entries[count].seq = (char *)realloc(entries[count].seq, seq_cap);
        if (!entries[count].seq) {
          fprintf(stderr, "Error: out of memory\n");
          exit(EXIT_FAILURE);
        }
      }
      strcpy(entries[count].seq + entries[count].seq_len, line);
      entries[count].seq_len = new_len;
    }
  }

  fclose(fp);
  return count + 1; /* total number of sequences */
}

/* Main */

int main(void) {
  /* Create output directory (equivalent to os.makedirs with exist_ok) */
  struct stat st;
  if (stat(OUTPUT_FOLDER, &st) != 0) {
    if (mkdir(OUTPUT_FOLDER, 0755) != 0 && errno != EEXIST) {
      fprintf(stderr, "Error: cannot create directory '%s': %s\n",
              OUTPUT_FOLDER, strerror(errno));
      return EXIT_FAILURE;
    }
  }

  /* Parse FASTA */
  FastaEntry *entries = (FastaEntry *)calloc(MAX_SEQUENCES, sizeof(FastaEntry));
  if (!entries) {
    fprintf(stderr, "Error: out of memory\n");
    return EXIT_FAILURE;
  }
  int num_sequences = parse_fasta(FASTA_FILE, entries, MAX_SEQUENCES);

  /* Process each sequence */
  for (int idx = 0; idx < num_sequences; idx++) {
    printf("Processing sequence %d/%d: %s\n", idx + 1, num_sequences,
           entries[idx].id);

    const char *seq = entries[idx].seq;
    int slen = entries[idx].seq_len;

    /* Build hydropathy array */
    double *hydro = (double *)malloc(slen * sizeof(double));
    if (!hydro) {
      fprintf(stderr, "Error: out of memory\n");
      return EXIT_FAILURE;
    }
    for (int i = 0; i < slen; i++)
      hydro[i] = hydropathy_score(seq[i]);

    /* Sanitize filename */
    char sanitized_id[MAX_ID_LEN * 2];
    sanitize_filename(entries[idx].id, sanitized_id, sizeof(sanitized_id));

    /* Build output CSV path */
    char csv_path[MAX_ID_LEN * 2 + 128];
    snprintf(csv_path, sizeof(csv_path), "%s/%s_window_%d.csv", OUTPUT_FOLDER,
             sanitized_id, WINDOW_LENGTH);

    /* Open CSV */
    FILE *csv = fopen(csv_path, "w");
    if (!csv) {
      fprintf(stderr, "Warning: cannot create '%s': %s — skipping.\n", csv_path,
              strerror(errno));
      free(hydro);
      continue;
    }

    /* Header */
    fprintf(csv, "CenterPosition,AminoAcid,AverageHydrophobicityScore\n");

    /* Sliding window (centered) */
    int num_windows = slen - WINDOW_LENGTH + 1;
    for (int i = 0; i < num_windows; i++) {
      double sum = 0.0;
      for (int w = 0; w < WINDOW_LENGTH; w++)
        sum += hydro[i + w];
      double avg = sum / WINDOW_LENGTH;

      int center = i + WINDOW_LENGTH / 2; /* 0-based center */

      fprintf(csv, "%d,%c,%.17g\n", center + 1, /* 1-based position */
              seq[center], avg);
    }

    fclose(csv);
    free(hydro);
  }

  /* Cleanup */
  for (int i = 0; i < num_sequences; i++)
    free(entries[i].seq);
  free(entries);

  printf("Hydropathy values (centered window) saved successfully.\n");
  return EXIT_SUCCESS;
}
