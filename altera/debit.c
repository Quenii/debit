/*
 * Copyright (C) 2006, 2007 Jean-Baptiste Note <jean-baptiste.note@m4x.org>
 *
 * This file is part of debit.
 *
 * Debit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Debit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with debit.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Command-line program to invoke the debit parser */

/*
 * Copyright (C) 2006 Jean-Baptiste Note <jean-baptiste.note@m4x.org>
 */

/** \file
    This is the command-line program to invoke the debit parser, glib
    version.

    @todo Switch to the new paradigm of using separate actions in
    debit_file, i.e., first ask for the bitstream to be parsed, then,
    according to flags, perform some actions. This will allow less
    clutter and hooks in the debit parser, will improve readability
    of the file, and will clarify the API. Pipdum already is an instance
    of this.

    \todo wrap mapping of file &al into a high-level function of the
    parser API

    \todo reorganize the data to seprate the bitstream data proper and
    the parsing structures. Some elements must be freed after parsing,
    but the data must not.
*/

#include <stdlib.h>
#include <glib.h>
#include "debitlog.h"
#include "bitarray.h"
#include "bitstream.h"

static gboolean labdump = FALSE;
static gboolean pipdump = FALSE;
static gboolean lutdump = FALSE;
static gboolean clearlut = FALSE;

static gchar *ifile = NULL;
static gchar *bitdump = NULL;
static gchar *m4kdump = NULL;
static gchar *odir = ".";
static gchar *datadir = DATADIR;

static const gchar **coords = NULL;

#if DEBIT_DEBUG > 0
unsigned int debit_debug = 0;
#endif

static void
debit_reverse(const altera_bitstream_t *altera,
	      const gchar ** acoords) {
  const gchar *scoord;

  if (!acoords)
    return;

  while ( (scoord = *acoords++) != NULL) {
    unsigned long coord = strtoul(scoord, NULL, 0);
    print_pos_from_bit_offset(altera, coord);
  }
}

static int
debit_file(gchar *input_file, gchar *output_dir) {
  altera_bitstream_t *altera;
  gint err = 0;

  altera = parse_bitstream(input_file);
  if (!altera) {
    g_warning("Bitstream parsing failed");
    err = -1;
    goto out_err_nofree;
  }

  debit_reverse(altera, coords);

  if (bitdump) {
    err = dump_raw_bit(NULL, bitdump, altera);
    if (err)
      goto out_err;
  }

  if (m4kdump) {
    err = dump_raw_m4k(NULL, m4kdump, altera);
    if (err)
      goto out_err;
  }

  if (lutdump)
    dump_lut_tables(altera);

  if (clearlut)
    zero_lut_tables(altera);

  if (labdump)
    dump_lab_data(odir, altera);

 out_err:
  free_bitstream(altera);
 out_err_nofree:
  return err;
}

static GOptionEntry entries[] =
{
  {"input", 'i', 0, G_OPTION_ARG_FILENAME, &ifile, "Read bitstream (sof file) <ifile>", "<ifile>"},
  {"coord", 'r', 0, G_OPTION_ARG_STRING_ARRAY, &coords, "Translate offset coordinate into device coordinates", NULL},
#if DEBIT_DEBUG > 0
  {"debug", 'g', 0, G_OPTION_ARG_INT, &debit_debug, "Debug verbosity", NULL},
#endif
  {"bitdump", 'b', 0, G_OPTION_ARG_FILENAME, &bitdump, "dump the raw bitstream data to <ofile>", "<ofile>"},
  {"lutdump", 'l', 0, G_OPTION_ARG_NONE, &lutdump, "dump lut tables", NULL},
  {"outdir", 'o', 0, G_OPTION_ARG_FILENAME, &odir, "Write data files in directory <odir>", "<odir>"},
  {"datadir", 'd', 0, G_OPTION_ARG_FILENAME, &datadir, "Read data files from directory <datadir>", "<datadir>"},
  {"clearlut", 'c', 0, G_OPTION_ARG_NONE, &clearlut, "Clear the LUT values before labdump", NULL},
  {"labdump", 'a', 0, G_OPTION_ARG_NONE, &labdump, "Dump raw lab data", NULL},
  {"pipdump", 'p', 0, G_OPTION_ARG_NONE, &pipdump, "Dump pips in the bitstream", NULL},
  /* some bitstream parameters */
  {"base_offset", 'f', 0, G_OPTION_ARG_INT, &base_off, "Initial offset in the bitfile", NULL},
  {"slice_offset", 's', 0, G_OPTION_ARG_INT, &slice_off, "offset at the smallest change", NULL},
  {"m4kdump", 'm', 0, G_OPTION_ARG_FILENAME, &m4kdump, "dump the m4k (opt 21 for now) bitstream data to <ofile>", "<ofile>"},
  { NULL }
};

int
main(int argc, char *argv[])
{
  int err;
  GError *error = NULL;
  GOptionContext *context = NULL;

  context = g_option_context_new ("- dump altera bitstream data");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_parse (context, &argc, &argv, &error);
  if (error != NULL) {
    g_warning("parse error: %s",error->message);
    g_error_free (error);
    return -1;
  }

  g_option_context_free(context);

  if (!ifile) {
    g_warning("You must specify a bitfile, %s --help for help", argv[0]);
    return -1;
  }

  err = debit_file(ifile, odir);

  /* cleanup of arguments -- should we do this ? */
/*   if (coords) */
/*     g_strfreev((gchar **)coords); */
/*   if (ifile) */
/*     g_free(ifile); */

  return err;
}
