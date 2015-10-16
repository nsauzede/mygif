#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#pragma pack(1)

typedef struct {
	char signature[3];
	char version[3];
} header_t;

typedef struct {
	uint16_t width;
	uint16_t height;
	uint8_t glbl : 1, color : 3, sort : 1, size : 3;
	uint8_t bckgnd;
	uint8_t pixel_aspect_ratio;
} logical_screen_descriptor_t;

typedef struct {
	uint8_t img_sep;
	uint16_t img_left_pos;
	uint16_t img_top_pos;
	uint16_t img_width;
	uint16_t img_height;
	uint8_t packed;
} image_descriptor_t;

#pragma pack()

int main( int argc, char *argv[])
{
	char *file = 0;
	int arg = 1;
	if (arg < argc)
	{
		file = argv[arg++];
	}
	if (!file)
	{
		printf( "usage: %s <file.gif>\n", argv[0]);
		exit( 1);
	}
	int fd = open( file, O_RDONLY);
	if (fd == -1)
	{
		perror( "open");
		exit( 1);
	}
	header_t header;
	logical_screen_descriptor_t logical_screen_descriptor;
	image_descriptor_t image_descriptor;
	size_t size = sizeof( header);
	read( fd, &header, size);
	printf( "header: %zd bytes\n", size);
	printf( "signature=%c%c%c version=%c%c%c\n", header.signature[0], header.signature[1], header.signature[2], header.version[0], header.version[1], header.version[2]);
	size = sizeof( logical_screen_descriptor);
	read( fd, &logical_screen_descriptor, size);
	printf( "logical screen descriptor: %zd bytes\n", size);
	printf( "width=%" PRIu16 " height=%" PRIu16 "\n", logical_screen_descriptor.width, logical_screen_descriptor.height);
	printf( "glbl=%d color=%d sort=%d size=%d\n", (int)logical_screen_descriptor.glbl, (int)logical_screen_descriptor.color, (int)logical_screen_descriptor.sort, (int)logical_screen_descriptor.size);
	printf( "bckgnd=%" PRIu8 " pixel_aspect_ratio=%" PRIu8 "\n", logical_screen_descriptor.bckgnd, logical_screen_descriptor.pixel_aspect_ratio);
	printf( "-----------\n");
	int i;
	int bpp = 1;
	for (i = 0; i < logical_screen_descriptor.color; i++)
		bpp *= 2;
	bpp *= 3;
	bpp /= 8;
	printf( "bpp=%d\n", bpp);
	int plen = 1;
	for (i = 0; i <= logical_screen_descriptor.size; i++)
		plen *= 2;
	printf( "plen=%d\n", plen);
	int psize = plen * bpp;
	printf( "psize=%d\n", psize);
	if (logical_screen_descriptor.glbl)
	{
		void *pal = malloc( psize);
		read( fd, pal, psize);
		printf( "reading palette..\n");
		free( pal);
	}
	read( fd, &image_descriptor, sizeof( image_descriptor));
	printf( "sep=%" PRIx8 " left=%" PRIu16 " top=%" PRIu16 " width=%" PRIu16 " height=%" PRIu16 "\n", image_descriptor.img_sep, image_descriptor.img_left_pos, image_descriptor.img_top_pos, image_descriptor.img_width, image_descriptor.img_height);
	close( fd);
	return 0;
}
