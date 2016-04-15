/*
 * Copyright (c) 2002-2016 Balabit
 * Copyright (c) 1998-2016 Balázs Scheidler
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

#include "pdb-file.h"
#include "pdb-error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

gint
pdb_file_detect_version(const gchar *pdbfile, GError **error)
{
  FILE *pdb;
  gchar line[1024];
  gint result = 0;

  pdb = fopen(pdbfile, "r");
  if (!pdb)
    {
      g_set_error(error, PDB_ERROR, PDB_ERROR_FAILED, "Error opening file %s (%s)", pdbfile, g_strerror(errno));
      return 0;
    }

  while (fgets(line, sizeof(line), pdb))
    {
      gchar *patterndb_tag;

      patterndb_tag = strstr(line, "<patterndb");
      if (patterndb_tag)
        {
          gchar *version, *start_quote, *end_quote;

          /* ok, we do have the patterndb tag, look for the version attribute */
          version = strstr(patterndb_tag, "version=");

          if (!version)
            goto exit;
          start_quote = version + 8;
          end_quote = strchr(start_quote + 1, *start_quote);
          if (!end_quote)
            {
              goto exit;
            }
          *end_quote = 0;
          result = strtoll(start_quote + 1, NULL, 0);
          break;
        }
    }
 exit:
  fclose(pdb);
  if (!result)
    {
      g_set_error(error, PDB_ERROR, PDB_ERROR_FAILED,
                  "Error detecting pdbfile version, <patterndb> version attribute not found or <patterndb> is not on its own line");
    }
  return result;
}