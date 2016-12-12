#ifndef STATIC
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#endif
#include "parser.h"

nmea_parser_module_s **parsers;
int n_parsers;

#ifndef STATIC
/**
 * Where to find the parser modules.
 * Can be overridden by env variable NMEA_PARSER_PATH
 */
#define PARSER_PATH "/usr/lib/nmea/"
#define FILENAME_MAX 255

static int
_get_so_files(const char *path, char **files)
{
	int j, i = 0;
	size_t len;
	char *name;
	DIR *d = NULL;
	struct dirent *dir;

	d = opendir(path);
	if (NULL == d) {
		return -1;
	}

	while (NULL != (dir = readdir(d))) {
		if ('.' == dir->d_name[0]) {
			continue;
		}

		len = strlen(dir->d_name);

		#ifdef __APPLE__
		if (len < 6) {
			continue;
		}

		if (0 != strncmp(dir->d_name + len - 6, ".dylib", 6)) {
			continue;
		}
		#else
		if (len < 3) {
			continue;
		}

		if (0 != strncmp(dir->d_name + len - 3, ".so", 3)) {
			continue;
		}
		#endif

		name = malloc(FILENAME_MAX);
		if (NULL == name) {
			goto fail;
		}

		strcpy(name, path);
		strcat(name, dir->d_name);
		files[i++] = name;
	}

	if (NULL != d) {
		closedir(d);
	}

	return i;

fail:
	for (j = 0; j < i; i++) {
		free(files[j]);
	}
	if (NULL != d) {
		closedir(d);
	}

	return -1;
}

nmea_parser_module_s *
nmea_init_parser(const char *filename)
{
	nmea_parser_module_s *parser;
	void *plugin;
	init_f init;

	/* Allocate parser struct */
	parser = malloc(sizeof (nmea_parser_module_s));
	if (NULL == parser) {
		return (nmea_parser_module_s *) NULL;
	}

	plugin = dlopen(filename, RTLD_NOW);
	if (NULL == plugin) {
		return (nmea_parser_module_s *) NULL;
	}

 	parser->handle = plugin;

	init = dlsym(plugin, "init");
	if (NULL == init) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->allocate_data = dlsym(plugin, "allocate_data");
	if (NULL == parser->allocate_data) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->set_default = dlsym(plugin, "set_default");
	if (NULL == parser->set_default) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->free_data = dlsym(plugin, "free_data");
	if (NULL == parser->free_data) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->parse = dlsym(plugin, "parse");
	if (NULL == parser->parse) {
		return (nmea_parser_module_s *) NULL;
	}

	if (-1 == init((nmea_parser_s *) parser)) {
		return (nmea_parser_module_s *) NULL;
	}

	return parser;
}

int
nmea_load_parsers()
{
	int i;
	char *files[255];
	char *parser_path;
	nmea_parser_module_s *parser;

	/* Get list of so files */
	parser_path = getenv("NMEA_PARSER_PATH");

	if (NULL == parser_path) {
		/* Use default path */
		parser_path = PARSER_PATH;
	} else {
		parser_path = parser_path;
	}

	n_parsers = _get_so_files(parser_path, files);
	if (1 > n_parsers) {
		return -1;
	}

	/* Allocate parsers array */
	parsers = malloc((sizeof (nmea_parser_module_s *)) * n_parsers);
	if (NULL == parsers) {
		return -1;
	}
	memset(parsers, 0, (sizeof (nmea_parser_module_s *)) * n_parsers);

	i = n_parsers;
	while (i-- > 0) {
		parser = nmea_init_parser(files[i]);
		free(files[i]);

		if (NULL == parser) {
			return -1;
		}

		parsers[(int) parser->parser.type - 1] = parser;
	}

	return n_parsers;
}

void
nmea_unload_parsers()
{
	int i;
	nmea_parser_module_s *parser;

	for (i = 0; i < n_parsers; i++) {
		if (NULL == (parser = parsers[i])) {
			continue;
		}

		dlclose(parser->handle);
		free(parser);
	}

	free(parsers);
}
#else
extern init_f gprmc_init;
extern allocate_data_f gprmc_allocate_data;
extern set_default_f gprmc_set_default;
extern free_data_f gprmc_free_data;
extern parse_f gprmc_parse;

nmea_parser_module_s *
nmea_init_parser(const char *filename)
{
	/* This function intentionally returns NULL */
	return NULL;
}

int
nmea_load_parsers()
{

	nmea_parser_module_s *parser;
	init_f init;

	/* Allocate parser struct */
	parser = malloc(sizeof (nmea_parser_module_s));
	if (NULL == parser) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->handle = NULL;

	init = gprmc_init;

	parser->allocate_data = gprmc_allocate_data;
	parser->set_default = gprmc_set_default;
	parser->free_data = gprmc_free_data;
	parser->parse = gprmc_parse;

	if (-1 == init((nmea_parser_s *) parser)) {
		return (nmea_parser_module_s *) NULL;
	}


	/* Allocate parsers array */
	parsers = malloc((sizeof (nmea_parser_module_s *)) * 10);
	if (NULL == parsers) {
		return -1;
	}
	memset(parsers, 0, (sizeof (nmea_parser_module_s *)) * 10);

	parsers[(int) parser->parser.type - 1] = parser;

	return 1;
}

void
nmea_unload_parsers()
{
	/* This function body is intentionally left empty */
}
#endif

nmea_parser_module_s *
nmea_get_parser_by_type(nmea_t type)
{
	return parsers[(int) type - 1];
}

nmea_parser_module_s *
nmea_get_parser_by_sentence(const char *sentence)
{
	int i;
	nmea_parser_module_s *parser;

	for (i = 0; i < n_parsers; i++) {
		if (NULL == (parser = parsers[i])) {
			continue;
		}

		if (0 == strncmp(sentence + 1, parser->parser.type_word, NMEA_PREFIX_LENGTH)) {
			return parser;
		}
	}

	return (nmea_parser_module_s *) NULL;
}
