/*
 * Copyright 2011 Emmanuel Engelhart <kelson@kiwix.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <getopt.h>
#include <unistd.h>
#include <kiwix/reader.h>
#include <kiwix/searcher.h>

void usage() {
    cout << "Usage: kiwix-search [--verbose|-v] ZIM_PATH SEARCH" << endl;
    exit(1);
}

int main(int argc, char **argv) {

  /* Init the variables */
  //const char *indexPath = "/home/itamar/.www.kiwix.org/kiwix/43k0i1j4.default/6d2e587b-d586-dc6a-dc6a-e4ef035a1495d15c.index";
  //const char *indexPath = "/home/itamar/testindex";
  const char *zimPath = NULL;
  const char *search = NULL;
  bool verboseFlag = false;
  int option_index = 0;
  int c = 0;

  kiwix::Searcher *searcher = NULL;
  kiwix::Reader *reader = NULL;

  /* Argument parsing */
  while (42) {

    static struct option long_options[] = {
      {"verbose", no_argument, 0, 'v'},
      {0, 0, 0, 0}
    };

    if (c != -1) {
      c = getopt_long(argc, argv, "vb:", long_options, &option_index);

      switch (c) {
	case 'v':
	  verboseFlag = true;
	  break;
      }
    } else {
      if (optind < argc) {
	if (zimPath == NULL) {
	  zimPath = argv[optind++];
	} else if (search == NULL) {
	  search = argv[optind++];
	} else {
	  cout << zimPath << endl;
	  usage();
	}
      } else {
	break;
      }
    }
  }

  /* Check if we have enough arguments */
  if (zimPath == NULL || search == NULL) {
    usage();
  }

  /* Try to prepare the indexing */
  try {
      reader = new kiwix::Reader(zimPath);
  } catch (...) {
      /* Cannot open the zimPath, maybe it is a plain old xapian database directory */
  }

  if ( reader ) {
      searcher = new kiwix::Searcher("", reader);
  } else {
      try {
          searcher = new kiwix::Searcher(zimPath, NULL);
      } catch (...) {
         cerr << "Unable to search through zim '" << zimPath << "'." << endl;
         exit(1);
      }
  }

  /* Start the indexing */
  if (searcher != NULL) {
    string searchString(search);
    searcher->search(searchString, 0, 10);
    kiwix::Result* p_result;
    while ( (p_result = searcher->getNextResult()) ) {
      cout << p_result->get_title() << endl;
      delete p_result;
    }

    delete searcher;
    delete reader;

    //      kiwix::XapianSearcher::terminate();
  } else {
    cerr << "Unable instanciate the Kiwix searcher." << endl;
    exit(1);
  }

  exit(0);
}
