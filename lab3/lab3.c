/*copied, modified by wukat*/
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <aio.h>

/* -------------------------------------------------------------------------------- */

static void print_type(struct stat *sb);
static void print_ino(const struct stat *sb);
static void print_perms(const struct stat *sb, const char * path);
static void print_linkc(const struct stat *sb);
static void print_owner(const struct stat *sb);
static void print_size(const struct stat *sb);
static void print_laststch(const struct stat *sb);
static void print_lastacc(const struct stat *sb);
static void print_lastmod(const struct stat *sb);
static void print_name(const struct stat *sb, char *name);
void printFormatedFileSize(long long size);
void printHowLongAgo(time_t lastTime);
static void print_content(char *name);
/* -------------------------------------------------------------------------------- */

int main(int argc, char *argv[]) {
	struct stat sb;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (stat(argv[1], &sb) == -1) {
		perror("stat");
		exit(EXIT_SUCCESS);
	}

	print_type(&sb);
	print_name(&sb, argv[1]);
	print_ino(&sb);
	print_perms(&sb, argv[1]);
	print_linkc(&sb);
	print_owner(&sb);
	print_size(&sb);
	print_laststch(&sb);
	print_lastacc(&sb);
	print_lastmod(&sb);
	print_content(argv[1]);

	exit(EXIT_SUCCESS);
}
/* -------------------------------------------------------------------------------- */

/* modified by wukat */
static void print_type(struct stat *sb) {
	printf("File type:                ");
	switch (sb->st_mode & S_IFMT) {
	case S_IFBLK:
		printf("block device\n");
		break;
	case S_IFCHR:
		printf("character device\n");
		break;
	case S_IFSOCK:
		printf("gniazdo\n");
		break;
	case S_IFLNK:
		printf("dowiazanie symboliczne\n");
		break;
	case S_IFREG:
		printf("plik regularny\n");
		break;
	case S_IFDIR:
		printf("katalog\n");
		break;
	case S_IFIFO:
		printf("kolejka FIFO\n");
		break;
	default:
		printf("unknown?\n");
		break;
	}
}
/* -------------------------------------------------------------------------------- */

static void print_ino(const struct stat *sb) {
	printf("I-node number:            %ld\n", (long) sb->st_ino);
}
/* -------------------------------------------------------------------------------- */

/* modified by wukat */
static void print_perms(const struct stat *sb, const char * path) {
	printf("Mode:                     %lo\n",
			(unsigned long) sb->st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
	printf("Your permisions: read: %s, write: %s, execute: %s.\n",
			access(path, R_OK) == 0 ? "yes" : "no",
			access(path, W_OK) == 0 ? "yes" : "no",
			access(path, X_OK) == 0 ? "yes" : "no");
}
/* -------------------------------------------------------------------------------- */

static void print_linkc(const struct stat *sb) {
	printf("Link count:               %ld\n", (long) sb->st_nlink);
}
/* -------------------------------------------------------------------------------- */

/* modified by wukat */
static void print_owner(const struct stat *sb) {
	struct passwd *pass;
	struct group *gr;
	if (((pass = getpwuid(sb->st_uid)) != NULL)
			&& ((gr = getgrgid(sb->st_gid)) != NULL)) {
		printf("Ownership:                %s(%ld)   %s(%ld)\n", pass->pw_name,
				(long) sb->st_uid, gr->gr_name, (long) sb->st_gid);
	} else {
		printf("Brak wpisu lub blad");
	}
}
/* -------------------------------------------------------------------------------- */

/* modified by wukat */
static void print_size(const struct stat *sb) {
	printf("Preferred I/O block size: %ld bytes\n", (long) sb->st_blksize);
	printf("File size:                ");
	printFormatedFileSize((long long) sb->st_size);
	printf("\nBlocks allocated:         %lld\n", (long long) sb->st_blocks);
}
/* -------------------------------------------------------------------------------- */

static void print_laststch(const struct stat *sb) {
	printf("Last status change:       %s", ctime(&sb->st_ctime));
	printHowLongAgo(sb->st_ctime);
}
/* -------------------------------------------------------------------------------- */

static void print_lastacc(const struct stat *sb) {
	printf("Last file access:         %s", ctime(&sb->st_atime));
	printHowLongAgo(sb->st_atime);
}
/* -------------------------------------------------------------------------------- */

static void print_lastmod(const struct stat *sb) {
	printf("Last file modification:   %s", ctime(&sb->st_mtime));
	printHowLongAgo(sb->st_mtime);
}
/* -------------------------------------------------------------------------------- */

/* modified by wukat */
static void print_name(const struct stat *sb, char *name) {
	ssize_t length = 0;
	char buf[65];
	char* bname = basename(name);
	if ((length = readlink(name, buf, 64)) > 0) { /* if 64 is not enough, name is truncated */
		buf[length] = '\0';
		printf("Name of the file:         %s -> %s\n", bname, buf);
	} else {	
		printf("Name of the file:         %s\n", bname);
	}
}
/* -------------------------------------------------------------------------------- */

/* written by wukat */
void printFormatedFileSize(long long size) {
	int temp;
	if (size >= 1024*1024*1024) { /* GB */
		temp = size/(1024*1024*1024);
		printf("%d GB ", temp);
		size -= temp * 1024*1024*1024; 
	}
	if (size >= 1024*1024) { /* MB */
		temp = size/(1024*1024);
		printf("%d MB ", temp);
		size -= temp * 1024*1024; 
	}
	if (size >= 1024) { /* KB */
		temp = size/1024;
		printf("%d KB ", temp);
		size -= temp * 1024; 
	} 
	if (size > 0) { /* B */
		printf("%d B ", (int) size);
	}
}

void printHowLongAgo(time_t lastTime) {
	time_t t;
	double diff = difftime(time(&t), lastTime);
	if (diff > 60*60*24*30*6) {
		printf("It's more than half a year ago\n");
	} else if (diff > 60*60*24*30) {
		printf("It's more than %d months ago\n", (int) diff/60*60*24*30);
	} else if (diff > 60*60*24) {
		printf("It's more than %d days ago\n", (int) diff/60*60*24);
	} else if (diff > 60*60) {
		printf("It's more than %d hours ago\n", (int) diff/60*60);
	} else if (diff > 60) {
		printf("It's more than %d minuts ago\n", (int) diff/60);
	} else {
		printf("It's more than %d seconds ago\n", (int) diff);
	}
}

static void print_content(char *name) {
	char response;
	int err, fd = -1, nread;
	char buf1[512], buf2[512];
	struct aiocb cbp;
	printf("Do you want me to show file content? [y/n]");
	while((response = getchar()) != 'y' && response != 'n');
	if (response == 'y') {
		memset(&cbp, 0, sizeof(cbp));
		cbp.aio_fildes = STDIN_FILENO;
    	cbp.aio_buf = buf2;
		cbp.aio_nbytes = sizeof(buf2);
		cbp.aio_sigevent.sigev_notify = SIGEV_NONE;
		if ((fd = open(name, O_RDONLY)) != -1) {
			while((nread = read(fd, buf1, sizeof(buf1))) != 0 && nread != -1) {
				if (aio_read(&cbp) == 0) {
					while((err = aio_error(&cbp)) != 0) {
						if (err == ECANCELED) {
							printf("Operation canceled");
							exit(EXIT_FAILURE);
						}
						else if (err > 0) {
							printf("Operation failed");
							exit(EXIT_FAILURE);
						}
					}
					write(STDOUT_FILENO, buf2, sizeof(buf2));
				}
			}
		}
		else 
			printf("Error while opening file: %s", strerror(errno));
	}
}